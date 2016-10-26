#include <logger.hh>
#include <attr_stats.hh>
#include "engine_impl.hh"
#include "async_handler.hh"

using namespace marine;

NS_HSHA_IMPL_BEGIN

CAsyncHandler::CAsyncHandler(CHshaEngineImpl & engine)
    : addingQue_(engine.addingQue_)
    , eventQue_(engine.eventQue_)
    , pkgQue_(engine.pkgQue_)
    , fdSockMap_(engine.fdSockMap_)
{}

bool CAsyncHandler::init(int threadCountMin, int threadCountMax)
{
    if(threadCountMin < 0 || threadCountMax < 0)
        return false;
    if(threadCountMin)
        __MyBase::threadCountMin(threadCountMin);
    if(threadCountMax)
        __MyBase::threadCountMax(threadCountMax);
    else{
        threadCountMax = tools::GetProcessorCount();
        if(threadCountMax > 2)  //notify + io
            __MyBase::threadCountMax(threadCountMax - 2);
    }
    WARN("CAsyncHandler thread count ["<<__MyBase::threadCountMin()<<", "<<__MyBase::threadCountMax()<<"]");
    return true;
}

void CAsyncHandler::run(__Task & task)
{
    ATTR_ADD(ATTR_POP_PKG_QUE, 1);
    TRACE("process task="<<task.toString());
    //check sock
    const int fd = task.fd();
    __SockPtr sock;
    fdSockMap_.getData(fd, &sock);
    if(!sock || &*sock != task.sock()){
        WARN("before process task="<<task.toString()<<", old sock is replaced by new sock="<<tools::ToStringPtr(sock)<<", drop task");
        return;
    }
    //process
    __SendList sendList;
    const bool ret = sock->process(task, sendList);
    //check sock again
    __SockPtr sock2;
    fdSockMap_.getData(fd, &sock2);
    if(sock != sock2){
        WARN("after process task="<<task.toString()<<", old sock is replaced by new sock="<<tools::ToStringPtr(sock2)<<", ignore");
        return;
    }
    //check result
    if(!ret){
        TRACE("process return false for task="<<task.toString()<<", sock="<<tools::ToStringPtr(sock)<<", close it");
        sock->events(kEventClose);
        if(!addingQue_.push(fd, 300)){
            ATTR_ADD(ATTR_PUSH_ADDING_QUE_ERR, 1);
            ERROR("addingQue_.push(fd="<<fd<<") failed, close sock="<<tools::ToStringPtr(sock));
            rmSockFd(fd);
        }else{
            ATTR_ADD(ATTR_PUSH_ADDING_QUE, 1);
        }
        return;
    }
    //send list
    if(!sendList.empty()){
        TRACE("sendList.size()="<<sendList.size()<<" after task="<<task.toString()<<" for sock="<<tools::ToStringPtr(sock));
        sock->addSendList(sendList);
        if(!events::NeedOutput(sock->events())){
            if(!eventQue_.push(CFdEvent(fd, kEventOut), 200)){
                ATTR_ADD(ATTR_PUSH_EVENT_QUE_ERR, 1);
                ERROR("eventQue_.push(fd="<<fd<<", "<<events::ToString(kEventOut)<<") failed after task="<<task.toString()<<" for sock="<<tools::ToStringPtr(sock)<<", ignore");
            }else{
                ATTR_ADD(ATTR_PUSH_EVENT_QUE, 1);
            }
        }
    }
}

void CAsyncHandler::rmSockFd(int fd)
{
    __SockPtr sock;
    fdSockMap_.clearData(fd, &sock);
    if(sock)
        sock->onClose();
}

NS_HSHA_IMPL_END

