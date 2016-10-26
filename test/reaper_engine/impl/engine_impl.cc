#include <logger.hh>
#include <scoped_ptr.hh>
#include <attr_stats.hh>
#include <tools/debug.hh>
#include "../server_callback.hh"
#include "../engine.hh"
#include "engine_impl.hh"

using namespace marine;

NS_HSHA_IMPL_BEGIN

CHshaEngineImpl::CHshaEngineImpl()
    : nameQue_(-1)
    , notify_(*this)
    , io_(*this)
    , handler_(*this)
    , reporter_(*this)
    , callback_(NULL)
{}

bool CHshaEngineImpl::addTcpServer(const __SockAddr & hostAddr, CServerCallback * callback)
{
    CSockName name(__ListenSocket::kFdType, callback);
    name.hostAddr(hostAddr);
    return pushSockName(name);
}

bool CHshaEngineImpl::addTcpClient(const __SockAddr & peerAddr, CServerCallback * callback)
{
    CSockName name(__TcpConnSocket::kFdType, callback);
    name.peerAddr(peerAddr);
    return pushSockName(name);
}

bool CHshaEngineImpl::addTcpClient(const __SockAddr & hostAddr, const __SockAddr & peerAddr, CServerCallback * callback)
{
    CSockName name(__TcpConnSocket::kFdType, callback);
    name.hostAddr(hostAddr);
    name.peerAddr(peerAddr);
    return pushSockName(name);
}

bool CHshaEngineImpl::addUdp(const __SockAddr & hostAddr, const __SockAddr & peerAddr, CServerCallback * callback)
{
    CSockName name(__UdpSocket::kFdType, callback);
    name.hostAddr(hostAddr);
    name.peerAddr(peerAddr);
    return pushSockName(name);
}

bool CHshaEngineImpl::run(const CReaperParams & params)
{
    //check
    LOGGER_CRASH_HANDLER();
    if(!stopped()){
        FATAL("cannot run again");
        return false;
    }
    //init
    callback_ = params.callback_;
    if(!notify_.init(params.maxFdNum_, params.epollTimeoutMs_)){
        FATAL("notify.init(maxFdNum_="<<params.maxFdNum_<<", epollTimeoutMs_="<<params.epollTimeoutMs_<<") failed");
        return false;
    }
    if(!handler_.init(params.processThreadCountMin_, params.processThreadCountMax_)){
        FATAL("handler.init(threadCountMin="<<params.processThreadCountMin_<<", threadCountMax="<<params.processThreadCountMax_<<") failed");
        return false;
    }
    if(callback_ && !reporter_.init(params.attrCount_, params.reportInterval_)){
        FATAL("reporter.init(attrCount="<<params.attrCount_<<", reportInterval_="<<params.reportInterval_<<") failed");
        return false;
    }
    //launch threads
    {
        CThreadAttr attr;
        attr.stackSize(16 << 10);
        if(callback_ && 1 != reporter_.startThreads(1, attr)){
            FATAL("launch reporter thread failed");
            return false;
        }
    }
    if(1 > handler_.startThreads(pkgQue_, 1, params.processStackSz_)){
        FATAL("launch handler threads failed");
        return false;
    }{
        CThreadAttr attr;
        if(params.ioStackSz_)
            attr.stackSize(params.ioStackSz_);
        if(1 != io_.startThreads(1, attr)){
            FATAL("launch io thread failed");
            return false;
        }
    }{
        CThreadAttr attr;
        if(params.notifyStackSz_)
            attr.stackSize(params.notifyStackSz_);
        if(1 != notify_.startThreads(1, attr)){
            FATAL("launch notify thread failed");
            return false;
        }
    }
#ifdef __USE_MT_ALLOC
    WARN("threads launched with MT_ALLOC");
#else
    WARN("threads launched");
#endif
    //add names
    checkNames();
    //wait
    if(params.nowait_){
        notify_.detachThreads();
        io_.detachThreads();
        if(callback_)
            reporter_.detachThreads();
    }else{
        notify_.joinThreads();
        io_.joinThreads();
        if(callback_)
            reporter_.joinThreads();
        while(!stopped())
            ::usleep(100000);
        WARN("engine stopped");
    }
    return true;
}

void CHshaEngineImpl::checkNames()
{
    typedef __NameQue::container_type __NameList;
    __NameList tmp;
    nameQue_.popAll(tmp, 0);
    if(tmp.empty())
        return;
    TRACE("find "<<tmp.size()<<" sock names");
    size_t err = 0;
    for(__NameList::iterator i = tmp.begin();i != tmp.end();++i){
        __NamePtr & n = *i;
        if(!n || !n->restart())
            continue;
        switch(n->fdType()){
            case __ListenSocket::kFdType:
                if(!addNameListen(n))
                    ++err;
                break;
            case __TcpConnSocket::kFdType:
                if(!addNameClient(n))
                    ++err;
                break;
            case __UdpSocket::kFdType:
                if(!addNameUdp(n))
                    ++err;
                break;
            default:
                ERROR("unknown fdType="<<n->fdType()<<" in SockName="<<n->toString());;
        }
    }
    if(err){
        WARN(err<<" socket names launch failed");
    }
    nameQue_.pushAll(tmp);
}

bool CHshaEngineImpl::addSockSession(__SockSession & session, __Events ev)
{
    const __SockPtr ptr(&session);
    DEBUG("add socket="<<tools::ToStringPtr(ptr)<<", events="<<events::ToString(ev)<<" to engine");
    const int fd = ptr->fd();
    fdSockMap_.setData(fd, ptr);
    if(!eventQue_.push(CFdEvent(fd, ev), 500)){
        ERROR("failed adding socket="<<tools::ToStringPtr(ptr)<<", events="<<events::ToString(ev)<<" to engine");
        return false;
    }
    ATTR_ADD(ATTR_PUSH_EVENT_QUE, 1);
    return true;
}

void CHshaEngineImpl::onAttrReport()
{
    if(callback_)
        callback_->onAttrReport();
}

void CHshaEngineImpl::reportAttr(int attr, uint64_t value)
{
    if(callback_)
        callback_->reportAttr(attr, value);
}

void CHshaEngineImpl::stop()
{
    if(callback_)
        reporter_.stopThreads();
    notify_.stopThreads();
    io_.stopThreads();
    handler_.stopThreads();
}

bool CHshaEngineImpl::stopped() const volatile
{
    return ((NULL == callback_ || reporter_.stopped())
            || notify_.stopped()
            || io_.stopped()
            || handler_.stopped());
}

std::string CHshaEngineImpl::toString() const
{
    CToString oss;
    oss<<"{callback_="<<callback_
        <<", notify_="<<notify_.toString()
        <<", io_="<<io_.toString()
        <<", handler_="<<handler_.toString()
        <<", reporter_="<<reporter_.toString()
        <<'}';
    return oss.str();
}

bool CHshaEngineImpl::pushSockName(const CSockName & name)
{
    __NamePtr p(tools::New1<CSockName>(name));
    if(!p){
        ERROR("cannot allocate CSockName for name="<<name.toString());
        return false;
    }
    if(!nameQue_.push(p)){
        ERROR("add name="<<tools::ToStringPtr(p)<<" failed");
        return false;
    }
    TRACE("add name="<<tools::ToStringPtr(p)<<" to engine");
    return true;
}

bool CHshaEngineImpl::addNameListen(__NamePtr & n)
{
    typedef CScopedPtr<__ListenSocket> __SockPtr;
    //check
    if(!n)
        return false;
    const CSockName & name = *n;
    if(!name.hostAddr().valid()){
        ERROR("invalid hostAddr in name="<<name.toString());
        return false;
    }
    //new sock
    __SockPtr sock(tools::New<__ListenSocket>());
    if(!sock){
        ERROR("cannot allocate ListenSocket for name="<<name.toString());
        return false;
    }
    //socket
    if(!sock->getSock(name.hostAddr().family())){
        ERROR("cannot getSock for name="<<name.toString()<<CSocket::ErrMsg());
        return false;
    }
    if(name.callback() && !name.callback()->onSocket(*sock)){
        INFO("callback onSocket failed for name="<<name.toString());
        return false;
    }
    //bind
    if(!sock->bindAddr(name.hostAddr())){
        ERROR("cannot bindAddr for name="<<name.toString()<<CSocket::ErrMsg());
        return false;
    }
    __SendList sendList;
    if(name.callback() && !name.callback()->onBind(*sock, sendList)){
        INFO("callback onBind failed for name="<<name.toString());
        return false;
    }
    if(!sendList.empty()){
        WARN("sendList.size()="<<sendList.size()<<" is not empty for ListenSocket name="<<name.toString()<<", ignore");
    }
    //listen
    if(!sock->listenAddr(name.hostAddr(), 0, false)){
        ERROR("cannot listenAddr for name="<<name.toString()<<CSocket::ErrMsg());
        return false;
    }
    if(name.callback() && !name.callback()->onListen(*sock)){
        INFO("callback onListen failed for name="<<name.toString());
        return false;
    }
    //add sock session
    CSockSessionParams params(sock.get(), name.callback());
    params.name_ = n;
    __SockSession * session = tools::New1<__SockSession>(params);
    if(!session){
        ERROR("cannot allocate SockSession for name="<<name.toString());
        return false;
    }
    sock.release();
    return addSockSession(*session, kEventAccept);
}

bool CHshaEngineImpl::addNameClient(__NamePtr & n)
{
    typedef CScopedPtr<__TcpConnSocket> __SockPtr;
    //check
    if(!n)
        return false;
    const CSockName & name = *n;
    if(!name.peerAddr().valid()){
        ERROR("invalid peerAddr in name="<<name.toString());
        return false;
    }
    //new sock
    __SockPtr sock(tools::New<__TcpConnSocket>());
    if(!sock){
        ERROR("cannot allocate TcpConnSocket for name="<<name.toString());
        return false;
    }
    //socket
    if(!sock->getSock(name.peerAddr().family())){
        ERROR("cannot getSock for name="<<name.toString()<<CSocket::ErrMsg());
        return false;
    }
    if(name.callback() && !name.callback()->onSocket(*sock)){
        INFO("callback onSocket failed for name="<<name.toString());
        return false;
    }
    //bind
    __SendList sendList;
    if(name.hostAddr().valid()){
        if(!sock->bindAddr(name.hostAddr())){
            ERROR("cannot bindAddr for name="<<name.toString()<<CSocket::ErrMsg());
            return false;
        }
        if(name.callback() && !name.callback()->onBind(*sock, sendList)){
            INFO("callback onBind failed for name="<<name.toString());
            return false;
        }
    }
    //connect
    if(!sock->connectAddr(name.peerAddr())){
        ERROR("cannot connectAddr for name="<<name.toString()<<CSocket::ErrMsg());
        return false;
    }
    if(name.callback() && !name.callback()->onConnect(*sock, sendList)){
        INFO("callback onConnect failed for name="<<name.toString());
        return false;
    }
    //non-block
    if(!sock->block(false)){
        ERROR("cannot set non-block for name="<<name.toString()<<CSocket::ErrMsg());
        return false;
    }
    //new sock session
    CSockSessionParams params(sock.get(), name.callback());
    params.name_ = n;
    __SockSession * session = tools::New1<__SockSession>(params);
    if(!session){
        ERROR("cannot allocate SockSession for name="<<name.toString());
        return false;
    }
    sock.release();
    //send list
    __Events ev = kEventTcpRecv;
    if(!sendList.empty()){
        session->addSendList(sendList);
        ev |= kEventTcpSend;
    }
    return addSockSession(*session, ev);
}

bool CHshaEngineImpl::addNameUdp(__NamePtr & n)
{
    typedef CScopedPtr<__UdpSocket> __SockPtr;
    //check
    if(!n)
        return false;
    const CSockName & name = *n;
    //new sock
    __SockPtr sock(tools::New<__UdpSocket>());
    if(!sock){
        ERROR("cannot allocate UdpSocket for name="<<name.toString());
        return false;
    }
    //socket
    if(!sock->getSock(AF_INET)){
        ERROR("cannot getSock for name="<<name.toString()<<CSocket::ErrMsg());
        return false;
    }
    if(name.callback() && !name.callback()->onSocket(*sock)){
        INFO("callback onSocket failed for name="<<name.toString());
        return false;
    }
    //bind
    __SendList sendList;
    if(name.hostAddr().valid()){
        if(!sock->bindAddr(name.hostAddr())){
            ERROR("cannot bindAddr for name="<<name.toString()<<CSocket::ErrMsg());
            return false;
        }
        if(name.callback() && !name.callback()->onBind(*sock, sendList)){
            INFO("callback onBind failed for name="<<name.toString());
            return false;
        }
    }
    //connect
    if(name.peerAddr().valid()){
        if(!sock->connectAddr(name.peerAddr())){
            ERROR("cannot connectAddr for name="<<name.toString()<<CSocket::ErrMsg());
            return false;
        }
        if(name.callback() && !name.callback()->onConnect(*sock, sendList)){
            INFO("callback onConnect failed for name="<<name.toString());
            return false;
        }
    }
    //non-block
    if(!sock->block(false)){
        ERROR("cannot set non-block for name="<<name.toString()<<CSocket::ErrMsg());
        return false;
    }
    //new sock session
    CSockSessionParams params(sock.get(), name.callback());
    params.name_ = n;
    __SockSession * session = tools::New1<__SockSession>(params);
    if(!session){
        ERROR("cannot allocate SockSession for name="<<name.toString());
        return false;
    }
    sock.release();
    //send list
    __Events ev = kEventUdpRecv;
    if(!sendList.empty()){
        session->addSendList(sendList);
        ev |= kEventUdpSend;
    }
    return addSockSession(*session, ev);
}

NS_HSHA_IMPL_END
