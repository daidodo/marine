#include <vector>
#include <iter_adapter.hh>
#include <logger.hh>
#include <attr_stats.hh>
#include "engine_impl.hh"
#include "async_io.hh"

using namespace marine;

NS_HSHA_IMPL_BEGIN

CAsyncIO::CAsyncIO(CHshaEngineImpl & engine)
    : engine_(engine)
    , addingQue_(engine.addingQue_)
    , eventQue_(engine.eventQue_)
    , pkgQue_(engine.pkgQue_)
    , fdSockMap_(engine.fdSockMap_)
{}

void CAsyncIO::run()
{
    typedef std::vector<__SockPtr>          __SockPtrList;
    typedef __FdEventQue::container_type    __FdEventList;
    LOGGER_CRASH_HANDLER();
    __FdList addingList;
    __FdEventList eventList_;
    __SockPtrList sockList_;
    for(;;){
        //pop all events
        if(!eventQue_.popAll(eventList_))
            continue;
        if(eventList_.empty())
            continue;
        ATTR_ADD(ATTR_POP_EVENT_QUE, eventList_.size());
        //get sockets
        sockList_.resize(eventList_.size());
        fdSockMap_.getData(const_iter_adapt_fun<int>(eventList_.begin(), CFdEvent::ExtractFd)
                , const_iter_adapt_fun<int>(eventList_.end(), CFdEvent::ExtractFd)
                , sockList_.begin());
        //iterate
        __FdEventList::const_iterator i = eventList_.begin();
        __SockPtrList::iterator sock_i = sockList_.begin();
        for(;i != eventList_.end();++i, ++sock_i){
            const int fd = i->fd();
            __SockPtr & sock = *sock_i;
            //validate fd and socket
            if(!CHshaEngineImpl::Validate(fd, sock)){
                DEBUG("fd="<<fd<<" is not sock="<<tools::ToStringPtr(sock)<<" before handle events, ignore it");
                continue;
            }
            //handle events
            TRACE("handle events="<<events::ToString(i->events())<<" from sock="<<tools::ToStringPtr(sock));
            const __Events oldEv = sock->events();
            bool handled = false;
            bool ok = (!events::NeedClose(i->events()) && !events::NeedClose(oldEv));
            if(ok && events::NeedOutput(i->events())){
                ok = handleOutput(sock);
                handled = true;
            }
            if(ok && events::NeedInput(i->events())){
                ok = handleInput(sock);
                handled = true;
            }
            if(!ok){
                TRACE("IO finish for sock="<<tools::ToStringPtr(sock)<<", close it");
                sock->events(kEventClose);
                handled = true;
            }
            //update events
            const __Events newEv = sock->events();
            if(oldEv != newEv){
                TRACE("add fd="<<fd<<", newEv="<<events::ToString(newEv)<<" into addingList, oldEv="<<events::ToString(oldEv)<<", sock="<<tools::ToStringPtr(sock));
                addingList.push_back(fd);
            }else if(!handled){
                WARN("nothing to do for sock="<<tools::ToStringPtr(sock));
            }
        }
        sockList_.clear();
        //flush addingList
        if(!addingList.empty()){
            const size_t sz = addingList.size();
            if(!addingQue_.pushAll(addingList, 300)){
                ATTR_ADD(ATTR_PUSH_ADDING_QUE_ERR, sz);
                ERROR("addingQue_.PushAll(size="<<sz<<") failed, close all sockets");
                rmSockFd(addingList);
                addingList.clear();
            }else{
                ATTR_ADD(ATTR_PUSH_ADDING_QUE, sz);
            }
        }
    }
    DEBUG("async io thread exit");
}

bool CAsyncIO::handleOutput(const __SockPtr & sock)
{
    ASSERT(sock, "sock=NULL");
    TRACE("sock="<<tools::ToStringPtr(sock));
    switch(sock->fdType()){
        case CTcpConnSocket::kFdType:
            return sock->sendTcp();
        case CUdpSocket::kFdType:
            return sock->sendUdp();
        default:;
    }
    ATTR_ADD(ATTR_UNKNOWN_FD_TYPE, 1);
    ERROR("unknonw fdType="<<sock->fdType()<<" for sock="<<tools::ToStringPtr(sock));
    return false;
}

bool CAsyncIO::handleInput(const __SockPtr & sock)
{
    ASSERT(sock, "sock=NULL");
    TRACE("sock="<<tools::ToStringPtr(sock));
    //recv & accept
    switch(sock->fdType()){
        case CListenSocket::kFdType:
            return handleAccept(sock);
        case CTcpConnSocket::kFdType:
            if(!sock->recvTcp())
                return false;
            break;
        case CUdpSocket::kFdType:
            if(!sock->recvUdp())
                return false;
            break;
        default:
            ATTR_ADD(ATTR_UNKNOWN_FD_TYPE, 1);
            ERROR("unknonw fdType="<<sock->fdType()<<" for sock="<<tools::ToStringPtr(sock));
            return false;
    }
    //push pkg
    const size_t sz = sock->pkgList().size();
    if(!pkgQue_.pushAll(sock->pkgList(), 500)){
        ATTR_ADD(ATTR_PUSH_PKG_QUE_ERR, sz);
        ERROR("pkgQue_.PushAll(size="<<sz<<") failed, drop packages for sock="<<tools::ToStringPtr(sock));
        sock->pkgList().clear();
    }else{
        ATTR_ADD(ATTR_PUSH_PKG_QUE, sz);
    }
    return true;
}

bool CAsyncIO::handleAccept(const __SockPtr & sock)
{
    ASSERT(sock, "sock=NULL");
    TRACE("sock="<<tools::ToStringPtr(sock));
    for(;;){
        //accept
        __SockSession * client = NULL;
        if(!sock->acceptClient(&client)){
            ERROR("accept error for sock="<<tools::ToStringPtr(sock));
            return false;
        }
        if(!client)
            break;  //no more
        const __Events ev = kEventTcpRecv;
        TRACE("new client="<<tools::ToStringPtr(client)<<", events="<<events::ToString(ev)<<" accepted from sock="<<tools::ToStringPtr(sock));
        engine_.addSockSession(*client, ev);
    }
    return true;
}

void CAsyncIO::rmSockFd(const __FdList & fdList)
{
    typedef std::vector<__SockPtr>  __SockPtrList;
    __SockPtrList sockList(fdList.size());
    fdSockMap_.clearData(fdList.begin(), fdList.end(), sockList.begin());
    __SockPtrList::iterator sock_i = sockList.begin();
    for(;sock_i != sockList.end();++sock_i){
        __SockPtr & sock = *sock_i;
        if(sock)
            sock->onClose();
    }
}

NS_HSHA_IMPL_END
