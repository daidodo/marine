#ifndef DOZERG_ASYNC_IO_H_20130507
#define DOZERG_ASYNC_IO_H_20130507

#include <threads.hh>
#include "sock_session.hh"

NS_HSHA_IMPL_BEGIN

class CHshaEngineImpl;

class CAsyncIO : public marine::CThreadPool
{
    typedef marine::CThreadPool     __MyBase;
    typedef __FdQue::container_type __FdList;
public:
    explicit CAsyncIO(CHshaEngineImpl & engine);
protected:
    virtual void run();
private:
    bool handleOutput(const __SockPtr & sock);
    bool handleInput(const __SockPtr & sock);
    bool handleAccept(const __SockPtr & sock);
    bool handleRecv(const __SockPtr & sock, bool udp);
    void rmSockFd(const __FdList & fdList);
    //fields
    CHshaEngineImpl & engine_;
    __FdQue & addingQue_;
    __FdEventQue & eventQue_;
    __SockDataQue & pkgQue_;
    __FdSockMap & fdSockMap_;
};

NS_HSHA_IMPL_END

#endif

