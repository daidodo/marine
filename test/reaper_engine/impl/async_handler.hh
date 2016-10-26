#ifndef DOZERG_ASYNC_HANDLER_H_20130026
#define DOZERG_ASYNC_HANDLER_H_20130026

#include <threads.hh>
#include "sock_session.hh"

NS_HSHA_IMPL_BEGIN

class CHshaEngineImpl;

class CAsyncHandler : public marine::CThreadManager<CSockData>
{
    typedef marine::CThreadManager<CSockData>  __MyBase;
public:
    explicit CAsyncHandler(CHshaEngineImpl & engine);
    bool init(int threadCountMin, int threadCountMax);
protected:
    virtual void run(__Task & task);
private:
    void rmSockFd(int fd);
    //fields
    __FdQue & addingQue_;
    __FdEventQue & eventQue_;
    __SockDataQue & pkgQue_;
    __FdSockMap & fdSockMap_;
};

NS_HSHA_IMPL_END

#endif

