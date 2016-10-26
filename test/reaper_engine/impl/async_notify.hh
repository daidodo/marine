#ifndef DOZERG_ASYNC_NOTIFY_H_20130506
#define DOZERG_ASYNC_NOTIFY_H_20130506

#include <epoll.hh>
#include <threads.hh>
#include "sock_session.hh"

NS_HSHA_IMPL_BEGIN

class CHshaEngineImpl;

class CAsyncNotify : public marine::CThreadPool
{
    typedef marine::CThreadPool __MyBase;
    typedef std::vector<int>    __FdArray;
public:
    explicit CAsyncNotify(CHshaEngineImpl & engine);
    bool init(uint32_t maxFdNum, int epollTimeoutMs);
protected:
    virtual void run();
private:
    void addFdEvent();
    void rmSockFd(const __FdArray & fdList);
    //fields
    CHshaEngineImpl & engine_;
    __FdQue & addingQue_;
    __FdEventQue & eventQue_;
    __FdSockMap & fdSockMap_;
    __FdArray errFdArray_;
    marine::CEpoll epoll_;
    int epTimeout_;
};

NS_HSHA_IMPL_END

#endif

