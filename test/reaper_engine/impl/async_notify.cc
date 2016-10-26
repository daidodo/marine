#include <logger.hh>
#include <iter_adapter.hh>
#include <attr_stats.hh>
#include <tools/system.hh>
#include "engine_impl.hh"
#include "async_notify.hh"

using namespace marine;

NS_HSHA_IMPL_BEGIN

CAsyncNotify::CAsyncNotify(CHshaEngineImpl & engine)
    : engine_(engine)
    , addingQue_(engine.addingQue_)
    , eventQue_(engine.eventQue_)
    , fdSockMap_(engine.fdSockMap_)
{}

bool CAsyncNotify::init(uint32_t maxFdNum, int epollTimeoutMs)
{
    epTimeout_ = epollTimeoutMs;
    //adjust max file num
    uint32_t i = tools::GetMaxFileDescriptor();
    if(i < maxFdNum){
        tools::SetMaxFileDescriptor(maxFdNum);
        i = tools::GetMaxFileDescriptor();
    }
    if(i != maxFdNum){
        WARN("real MAX fd num="<<i<<" is not maxFdNum="<<maxFdNum);
        maxFdNum = i;
    }else{
        WARN("MAX fd num="<<maxFdNum);
    }
    //create epoll
    if(!epoll_.create()){
        FATAL("epoll_.create() failed"<<CEpoll::ErrMsg());
        return false;
    }
    return true;
}

void CAsyncNotify::run()
{
    typedef __FdEventQue::container_type __FdEventList;
    LOGGER_CRASH_HANDLER();
    __FdEventList fdEventList;
    time_t curTime, nameCheckTime = 0;
    for(;;){
        if(!epoll_.wait(epTimeout_)){
            ERROR("epoll wait(epTimeout_="<<epTimeout_<<") failed"<<CEpoll::ErrMsg());
            ATTR_ADD(ATTR_EPOLL_WAIT_ERR, 1);
            continue;
        }
        tools::Time(&curTime);
        //handle events
        for(size_t i = 0, sz = epoll_.size();i < sz;++i){
            const CEpollEvent & event = epoll_[i];
            const int fd = event.fd();
            if(!event.valid()){
                ERROR("invalid event="<<event.toString());
                continue;
            }else if(event.error()){
                WARN("error in event="<<event.toString());
                errFdArray_.push_back(fd);
                continue;
            }
            __Events revents = 0;
            if(event.canInput()){
                revents |= kEventIn;
                ATTR_ADD(ATTR_EVENT_INPUT, 1);
            }
            if(event.canOutput()){
                revents |= kEventOut;
                ATTR_ADD(ATTR_EVENT_OUTPUT, 1);
            }
            TRACE("push event="<<event.toString()<<", revents="<<events::ToString(revents)<<" into fdEventList");
            fdEventList.push_back(CFdEvent(fd, revents));
        }
        //add events to eventQue_
        if(!fdEventList.empty()){
            const size_t sz = fdEventList.size();
            if(!eventQue_.pushAll(fdEventList, 500)){
                ATTR_ADD(ATTR_PUSH_EVENT_QUE_ERR, sz);
                ERROR("eventQue_.PushAll(size="<<sz<<") failed, eventQue_.size()="<<eventQue_.size()<<", add to errFdArray_");
                errFdArray_.insert(errFdArray_.end()
                        , const_iter_adapt_fun<int>(fdEventList.begin(), CFdEvent::ExtractFd)
                        , const_iter_adapt_fun<int>(fdEventList.end(), CFdEvent::ExtractFd));
                fdEventList.clear();
            }else{
                ATTR_ADD(ATTR_PUSH_EVENT_QUE, sz);
            }
        }
        //add fd from addingQue_ to epoll
        addFdEvent();
        //close sockets
        if(!errFdArray_.empty()){
            ATTR_ADD(ATTR_CLOSE_FD, errFdArray_.size());
            rmSockFd(errFdArray_);
            errFdArray_.clear();
        }
        //check sock name queue
        if(nameCheckTime != curTime){   //check per second
            engine_.checkNames();
            nameCheckTime = curTime;
        }
    }
    DEBUG("async notify thread exit");
}

void CAsyncNotify::addFdEvent()
{
    typedef std::vector<__SockPtr>  __SockPtrList;
    typedef __FdQue::container_type __FdList;
    //pop all
    __FdList fdList;
    if(!addingQue_.popAll(fdList, 0))
        return;
    if(fdList.empty())
        return;
    ATTR_ADD(ATTR_POP_ADDING_QUE, fdList.size());
    //get sock ptr
    __SockPtrList sockList(fdList.size());
    fdSockMap_.getData(fdList.begin(), fdList.end(), sockList.begin());
    //iterate list
    __FdList::const_iterator i = fdList.begin();
    __SockPtrList::const_iterator sock_i = sockList.begin();
    for(;i != fdList.end();++i, ++sock_i){
        const int fd = *i;
        const __SockPtr & sock = *sock_i;
        TRACE("get fd="<<fd<<", sock="<<tools::ToStringPtr(sock)<<" from addingQue_");
        //validate fd and sock ptr
        if(!CHshaEngineImpl::Validate(fd, sock)){
            DEBUG("fd="<<fd<<" is not sock="<<tools::ToStringPtr(sock)<<" before adding to epoll, ignore it");
            continue;
        }
        const __Events sockEv = sock->events();
        if(events::NeedClose(sockEv)){
            TRACE("push closing sock="<<tools::ToStringPtr(sock)<<" into errFdArray_");
            errFdArray_.push_back(fd);
            continue;
        }
        //add fd and events to epoll
        uint32_t ev = 0;     //epoll flags
        if(events::NeedInput(sockEv))
            ev |= EPOLLIN;
        bool ret = events::NeedOutput(sockEv);
        if(ret)
            ev |= EPOLLOUT;
        if(0 == ev){
            ERROR("events=0 for for sock="<<tools::ToStringPtr(sock));
            continue;
        }
        TRACE("modify epoll events="<<CEpollEvent::EventsName(ev)<<" for sock="<<tools::ToStringPtr(sock));
        ret = (ret ? epoll_.modFd(fd, ev) : epoll_.addFd(fd, ev));
        if(!ret){
            ATTR_ADD(ATTR_EPOLL_ADD_ERR, 1);
            WARN("add epoll (fd="<<fd<<", ev="<<CEpollEvent::EventsName(ev)<<") failed for client="<<tools::ToStringPtr(sock)<<", close it");
            errFdArray_.push_back(fd);
        }else{
            ATTR_ADD(ATTR_EPOLL_ADD, 1);
        }
    }
}

void CAsyncNotify::rmSockFd(const __FdArray & fdList)
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
