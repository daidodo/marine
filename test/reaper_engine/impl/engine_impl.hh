#ifndef DOZERG_ENGINE_IMPL_H_20130506
#define DOZERG_ENGINE_IMPL_H_20130506

#include "async_notify.hh"
#include "async_io.hh"
#include "async_handler.hh"
#include "attr_report.hh"

NS_HSHA_BEGIN

class CReaperParams;

NS_HSHA_END

NS_HSHA_IMPL_BEGIN

class CHshaEngineImpl
{
public:
    //functions
    static bool Validate(int fd, const __SockPtr & sock){
        return (sock && sock->valid() && fd == sock->fd());
    }
    CHshaEngineImpl();
    bool addTcpServer(const __SockAddr & hostAddr, CServerCallback * callback);
    bool addTcpClient(const __SockAddr & peerAddr, CServerCallback * callback);
    bool addTcpClient(const __SockAddr & hostAddr, const __SockAddr & peerAddr, CServerCallback * callback);
    bool addUdp(const __SockAddr & hostAddr, const __SockAddr & peerAddr, CServerCallback * callback);
    bool run(const CReaperParams & params);
    void checkNames();
    bool addSockSession(__SockSession & session, __Events ev);
    void onAttrReport();
    void reportAttr(int attr, uint64_t value);
    void stop();
    bool stopped() const volatile;
    std::string toString() const;
private:
    bool pushSockName(const CSockName & name);
    bool addNameListen(__NamePtr & name);
    bool addNameClient(__NamePtr & name);
    bool addNameUdp(__NamePtr & name);
public:
    //fields
    __FdQue addingQue_;
    __FdEventQue eventQue_;
    __SockDataQue pkgQue_;
    __FdSockMap fdSockMap_;
private:
    __NameQue nameQue_;
    CAsyncNotify notify_;
    CAsyncIO io_;
    CAsyncHandler handler_;
    CAttrReporter reporter_;
    CServerCallback * callback_;
};

NS_HSHA_IMPL_END

#endif

