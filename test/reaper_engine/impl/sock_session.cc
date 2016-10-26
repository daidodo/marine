#include <shared_ptr.hh>
#include <logger.hh>
#include <to_string.hh>
#include <scoped_ptr.hh>
#include <attr_stats.hh>
#include <tools/memory.hh>
#include "../server_callback.hh"
#include "engine_impl.hh"
#include "sock_session.hh"

using namespace marine;

NS_HSHA_IMPL_BEGIN

//class CSockName

std::string CSockName::toString() const
{
    CToString oss;
    oss<<"{fdType_="<<fdType_
        <<", hostAddr_="<<hostAddr_.toString()
        <<", peerAddr_="<<peerAddr_.toString()
        <<", callback_="<<tools::ToStringPtr(callback_)
        <<", on_="<<on_
        <<", connect_="<<connect_
        <<"}";
    return oss.str();
}

//class CPkgData

std::string CPkgData::toString() const
{
    CToString oss;
    oss<<"{from_="<<from_.toString()
        <<", lens_={";
    for(size_t i = 0;i < lens_.size();++i){
        if(i)
            oss<<", ";
        oss<<lens_[i];
    }
    oss<<"}, data_="<<tools::Dump(data_)
        <<"}";
    return oss.str();
}

//class CSockData

CSockData::CSockData(CSockSession * s, const std::string & buf, const __Lens & lens, const __SockAddr & from)
    : CPkgData(buf, lens, from)
    , sock_(s)
    , fd_(s ? s->fd() : -1)
{}

std::string CSockData::toString() const
{
    CToString oss;
    oss<<"{fd_="<<fd_
        <<", sock_=@"<<sock_
        <<", CPkgData="<<CPkgData::toString()
        <<"}";
    return oss.str();
}

//class CSockSession

CSockSession::CSockSession(const CSockSessionParams & params)
    : sock_(params.sock_)
    , callback_(params.callback_)
    , name_(params.name_)
    , ev_(0)
{
    if(name_)
        name_->start();
}

#define __CASE_DELETE_SOCK(type) \
    case type::kFdType:{   \
        type * p = dynamic_cast<type *>(sock_); \
        tools::Delete(p);  \
        break;}

CSockSession::~CSockSession()
{
    if(sock_){
        switch(sock_->fdType()){
            __CASE_DELETE_SOCK(CListenSocket);
            __CASE_DELETE_SOCK(CTcpConnSocket);
            __CASE_DELETE_SOCK(CUdpSocket);
            default:
                ERROR("memory leak for unknown fdType="<<sock_->fdType()<<" for sock_="<<tools::ToStringPtr(sock_));
        }
    }
    if(name_)
        name_->stop();
}

#undef __CASE_DELETE_SOCK

std::string CSockSession::toString() const
{
    CToString oss;
    oss<<"{sock_="<<tools::ToStringPtr(sock_)
        <<", callback_="<<tools::ToStringPtr(callback_)
        <<", events_="<<events::ToString(ev_)
        <<", name_="<<tools::ToStringPtr(name_)
        <<"}";
    return oss.str();
}

void CSockSession::addSendList(__SendList & list)
{
    if(list.empty())
        return;
    __SendGuard g(sendLock_);
    if(sendList_.empty())
        sendList_.swap(list);
    else
        sendList_.insert(sendList_.end(), list.begin(), list.end());
}

bool CSockSession::acceptClient(CSockSession ** client)
{
    typedef CScopedPtr<CTcpConnSocket> __TcpConnPtr;
    //check
    if(!valid() || !client)
        return false;
    *client = NULL;
    //down cast
    CListenSocket * listen = dynamic_cast<CListenSocket *>(sock_);
    if(!listen){
        ERROR("cannot cast sock_="<<tools::ToStringPtr(sock_)<<" to CListenSocket");
        return false;
    }
    //accept
    __TcpConnPtr clientSock(tools::New<CTcpConnSocket>());
    if(!clientSock){
        ERROR("cannot new CTcpConnSocket object for sock="<<toString());
        return false;
    }
    if(!listen->acceptSocket(*clientSock)){
        ERROR("listen sock="<<tools::ToStringPtr(listen)<<" accept client error"<<CSocket::ErrMsg());
        return false;
    }
    if(!clientSock->valid()){     //no more
        addEvents(kEventAccept);
        return true;
    }
    ATTR_ADD(ATTR_ACCEPT_CLIENT, 1);
    //callback
    CServerCallback * clientCallback = NULL;
    if(callback_ && !callback_->onAccept(*listen, *clientSock, &clientCallback)){
        INFO("callback failed for onAccept(listen="<<tools::ToStringPtr(listen)<<", client="<<tools::ToStringPtr(clientSock)<<", close client");
        return true;
    }
    //set up client
    if(!clientSock->linger(true)){   //avoid TIME_WAIT
        WARN("set linger failed for client="<<tools::ToStringPtr(clientSock)<<" accepted from listen="<<tools::ToStringPtr(listen));
    }
    if(!clientSock->block(false)){  //non-block
        WARN("set non-block failed for client="<<tools::ToStringPtr(clientSock)<<" accepted from listen="<<tools::ToStringPtr(listen));
    }
    //new sock session
    CSockSessionParams params(clientSock.get(), clientCallback);
    *client = tools::New1<CSockSession>(params);
    if(!*client){
        ERROR("new sock session failed of client="<<tools::ToStringPtr(clientSock)<<", close client");
        return false;
    }
    clientSock.release();
    return true;
}

bool CSockSession::recvTcp()
{
    //check
    if(!valid()){
        ERROR("invalid sock="<<toString());
        return false;
    }
    //down cast
    CTcpConnSocket * sock = dynamic_cast<CTcpConnSocket *>(sock_);
    if(!sock){
        ERROR("cannot cast sock_="<<tools::ToStringPtr(sock_)<<" to CTcpConnSocket");
        return false;
    }
    //recv
    const size_t kRecvSize = 4096;
    for(;;){
        const ssize_t n = sock->recvData(recvBuf_, kRecvSize);
        if(n < 0){
            if(EAGAIN == errno || EWOULDBLOCK == errno)    //no more
                break;
            ERROR("recv failed for sock="<<tools::ToStringPtr(sock)<<CSocket::ErrMsg());
            return false;
        }else if(0 == n){
            DEBUG("peer closed for sock="<<tools::ToStringPtr(sock));
            return false;
        }
        ATTR_ADD(ATTR_TCP_RECV, 1);
        TRACE("recv "<<n<<" bytes into recvBuf_="<<tools::Dump(recvBuf_)<<" for sock="<<tools::ToStringPtr(sock));
        if(!checkRecvBuf(*sock_, sock->peerAddr()))
            return false;
        if(size_t(n) < kRecvSize)    //no more
            break;
    }
    addEvents(kEventTcpRecv);
    return true;
}

bool CSockSession::recvUdp()
{
    //check
    if(!valid()){
        ERROR("invalid sock="<<toString());
        return false;
    }
    //down cast
    CUdpSocket * sock = dynamic_cast<CUdpSocket *>(sock_);
    if(!sock){
        ERROR("cannot cast sock_="<<tools::ToStringPtr(sock_)<<" to CUdpSocket");
        return false;
    }
    //recv
    const size_t kRecvSize = 4096;
    CSockAddr addr;
    for(;;){
        const ssize_t n = sock->recvData(addr, recvBuf_, kRecvSize);
        if(n < 0){
            if(EAGAIN == errno || EWOULDBLOCK == errno)    //no more
                break;
            ERROR("recv failed for sock="<<tools::ToStringPtr(sock)<<CSocket::ErrMsg());
            return false;
        }
        ATTR_ADD(ATTR_UDP_RECV, 1);
        TRACE("recv "<<n<<" bytes from addr="<<addr.toString()<<" into recvBuf_="<<tools::Dump(recvBuf_)<<" for sock="<<tools::ToStringPtr(sock));
        if(!checkRecvBuf(*sock_, addr))
            return false;
    }
    addEvents(kEventUdpRecv);
    return true;
}

bool CSockSession::process(const CPkgData & data, __SendList & sendList)
{
    //check
    if(!valid()){
        ERROR("invalid sock="<<toString());
        return false;
    }
    //process
    for(size_t off = 0, i = 0, e = data.size();i < e;++i){
        CPkgData::__Buf buf = data.buf(i, off);
        if(buf.empty()){
            WARN("empty buf(index="<<i<<", offset="<<off<<") in data="<<data.toString()<<", ignore");
            continue;
        }
        if(callback_ && !callback_->onProcess(*sock_, &buf[0], buf.size(), data.from(), sendList)){
            DEBUG("callback return false for onProcess(socket="<<tools::ToStringPtr(sock_)<<", from="<<data.from().toString()<<", buf="<<tools::Dump(buf)<<", close socket");
            return false;
        }
    }
    return true;
}

bool CSockSession::checkRecvBuf(__Socket & sock, const __SockAddr & addr)
{
    if(!callback_){
        ERROR("no callback_ for sock="<<sock.toString()<<", addr="<<addr.toString()<<", recvBuf_="<<tools::Dump(recvBuf_));
        return false;
    }
    size_t off = 0;
    __Lens lenArr;
    while(off < recvBuf_.size()){
        const size_t len = recvBuf_.size() - off;
        const ssize_t m = callback_->onRecv(sock, &recvBuf_[off], len, addr);
        if(m < 0){
            ERROR("format error recvBuf_="<<tools::Dump(&recvBuf_[off], len)<<" for sock="<<sock.toString());
            return false;
        }else if(0 == m){   //need more
            break;
        }else if(size_t(m) > len){
            ERROR("invalid return="<<m<<" for callback onRecv(sock="<<sock.toString()<<", recvBuf_="<<tools::Dump(&recvBuf_[off], len)<<")");
            return false;
        }
        off += m;
        lenArr.push_back(m);
    }
    if(off){
        std::string data;
        if(off < recvBuf_.size()){
            data = recvBuf_.substr(0, off);
            recvBuf_.erase(0, off);
        }else
            data.swap(recvBuf_);
        pkgList_.push_back(CSockData(this, data, lenArr, addr));
        TRACE("push to pkgList_, sock data="<<pkgList_.back().toString());
    }
    return true;
}

bool CSockSession::sendTcp()
{
    //check
    if(!valid()){
        ERROR("invalid sock="<<toString());
        return false;
    }
    //down cast
    CTcpConnSocket * sock = dynamic_cast<CTcpConnSocket *>(sock_);
    if(!sock){
        ERROR("cannot cast sock_="<<tools::ToStringPtr(sock_)<<" to CTcpConnSocket");
        return false;
    }
    //send
    for(__SendElem elem;popSendElem(elem);){
        std::string & buf = elem.first;
        if(buf.empty())
            continue;
        const ssize_t sz = sock->sendData(buf);
        if(sz < 0){
            if(EAGAIN == errno || EWOULDBLOCK == errno){
                pushSendElem(elem, true);
                addEvents(kEventTcpSend);
                break;
            }
            ERROR("send failed for buf="<<tools::Dump(buf)<<" for sock="<<toString()<<CSocket::ErrMsg());
            return false;
        }
        ATTR_ADD(ATTR_TCP_SEND, 1);
        TRACE("send buf="<<tools::Dump(buf)<<" to sock="<<tools::ToStringPtr(sock));
        if(callback_)
            callback_->onSend(*sock_, buf.c_str(), sz, sock->peerAddr());
        if(size_t(sz) < buf.size()){
            TRACE("send sz="<<sz<<" of buf="<<tools::Dump(buf)<<" for sock="<<tools::ToStringPtr(sock));
            buf.erase(0, sz);
            pushSendElem(elem, true);
            addEvents(kEventTcpSend);
            break;
        }
    }
    return true;
}

bool CSockSession::sendUdp()
{
    //check
    if(!valid()){
        ERROR("invalid sock="<<toString());
        return false;
    }
    //down cast
    CUdpSocket * sock = dynamic_cast<CUdpSocket *>(sock_);
    if(!sock){
        ERROR("cannot cast sock_="<<tools::ToStringPtr(sock_)<<" to CUdpSocket");
        return false;
    }
    //send
    for(__SendElem elem;popSendElem(elem);){
        std::string & buf = elem.first;
        const CSockAddr & addr = elem.second;
        if(buf.empty())
            continue;
        const ssize_t sz = sock->sendData(addr, buf);
        if(sz < 0){
            if(EAGAIN == errno || EWOULDBLOCK == errno){
                pushSendElem(elem, true);
                addEvents(kEventUdpSend);
                break;
            }
            ERROR("send failed for addr="<<addr.toString()<<", buf="<<tools::Dump(buf)<<" for sock="<<toString()<<CSocket::ErrMsg());
            return false;
        }
        ATTR_ADD(ATTR_UDP_SEND, 1);
        TRACE("send buf="<<tools::Dump(buf)<<" to addr="<<addr.toString()<<" for sock="<<tools::ToStringPtr(sock));
        if(callback_)
            callback_->onSend(*sock_, buf.c_str(), buf.size(), addr);
    }
    return true;
}

void CSockSession::onClose()
{
    if(!valid())
        return;
    const bool r = (callback_ ? callback_->onClose(*sock_) : false);
    if(name_)
        name_->restart(r);
}

void CSockSession::pushSendElem(const __SendElem & elem, bool front)
{
    ASSERT(valid(), "invalid sock="<<toString());
    __SendGuard g(sendLock_);
    if(front)
        sendList_.push_front(elem);
    else
        sendList_.push_back(elem);
}

bool CSockSession::popSendElem(__SendElem & elem)
{
    ASSERT(valid(), "invalid sock="<<toString());
    __SendGuard g(sendLock_);
    if(sendList_.empty())
        return false;
    elem = sendList_.front();
    sendList_.pop_front();
    return true;
}

NS_HSHA_IMPL_END
