#ifndef DOZERG_SOCK_SESSION_H_20130506
#define DOZERG_SOCK_SESSION_H_20130506

#include <deque>
#include <utility>      //std::pair
#include <lock_queue.hh>
#include <fd_data_map.hh>
#include <atomic_sync.hh>
#include <char_buffer.hh>
#include "../types.hh"
#include "events.hh"

NS_HSHA_BEGIN

class CServerCallback;

NS_HSHA_END

NS_HSHA_IMPL_BEGIN

class CSockName
{
public:
    CSockName()
        : callback_(NULL)
        , fdType_(0)
    {}
    CSockName(int type, CServerCallback * cb)
        : callback_(cb)
        , fdType_(type)
        , on_(false)
        , connect_(true)
    {}
    bool valid() const{return (0 != fdType_);}
    int fdType() const{return fdType_;}
    CServerCallback * callback() const{return callback_;}
    void hostAddr(const __SockAddr & addr){hostAddr_ = addr;}
    const __SockAddr & hostAddr() const{return hostAddr_;}
    void peerAddr(const __SockAddr & addr){peerAddr_ = addr;}
    const __SockAddr & peerAddr() const{return peerAddr_;}
    void start(){on_ = true;connect_ = false;}
    void stop() throw(){on_ = false;}
    void restart(bool r){connect_ = r;}
    bool restart() const{return (!on_ && connect_);}
    std::string toString() const;
private:
    __SockAddr hostAddr_, peerAddr_;
    CServerCallback * callback_;
    int fdType_;
    volatile bool on_;
    volatile bool connect_;  //need new socket
};

typedef std::basic_string<size_t>       __Lens;

class CPkgData
{
    std::string data_;
    __Lens lens_;
    __SockAddr from_;
public:
    typedef marine::CCharBuffer<const char>   __Buf;
    CPkgData(){}
    CPkgData(const std::string & buf, const __Lens & lens, const __SockAddr & from)
        : data_(buf)
        , lens_(lens)
        , from_(from)
    {}
    size_t size() const{return lens_.size();}
    __Buf buf(size_t index, size_t & off) const{
        assert(index < size());
        const size_t len = lens_[index];
        const size_t old = off;
        off += len;
        return __Buf(data_.c_str() + old, len, len);
    }
    const __SockAddr & from() const{return from_;}
    std::string toString() const;
};

class CSockSession;

class CSockData : public CPkgData
{
    CSockSession * sock_;
    int fd_;
public:
    CSockData():sock_(NULL), fd_(-1){}
    CSockData(CSockSession * s, const std::string & buf, const __Lens & lens, const __SockAddr & from);
    int fd() const{return fd_;}
    CSockSession * sock() const{return sock_;}
    std::string toString() const;
};

typedef marine::CLockQueue<CSockData>   __SockDataQue;
typedef marine::CSharedPtr<CSockName>   __NamePtr;

struct CSockSessionParams
{
    __Socket * sock_;
    CServerCallback * callback_;
    __NamePtr name_;
    //functions
    CSockSessionParams(__Socket * s, CServerCallback * c)
        : sock_(s)
        , callback_(c)
    {}
};

typedef std::pair<std::string, __SockAddr>  __SendElem;
typedef std::deque<__SendElem>              __SendList;

class CSockSession
{
    typedef marine::CAtomicSync<__Events>   __LockEvents;
    typedef marine::CSpinLock               __SendLock;
    typedef marine::CGuard<__SendLock>      __SendGuard;
    typedef __SockDataQue::container_type   __PkgList;
public:
    explicit CSockSession(const CSockSessionParams & params);
    ~CSockSession();
    //检查是否有效
    bool valid() const{return sock_ && sock_->valid();}
    //获取对应的fd
    int fd() const{return (sock_ ? sock_->fd() : __Socket::kInvalidFd);}
    //获取fd类型
    int fdType() const{return sock_->fdType();}
    //设置/获取事件标志
    void events(__Events ev){ev_ = ev;}
    __Events events() const{return ev_;}
    //可读写访问已收到的数据包
    __PkgList & pkgList(){return pkgList_;}
    //增加待发送数据
    void addSendList(__SendList & list);
    //接受新客户端
    bool acceptClient(CSockSession ** client);
    //接收数据
    bool recvTcp();
    bool recvUdp();
    //处理数据
    bool process(const CPkgData & data, __SendList & list);
    //发送数据
    bool sendTcp();
    bool sendUdp();
    //准备关闭
    void onClose();
    //内部状态
    std::string toString() const;
private:
    CSockSession(const CSockSession &);     //disable copy and assignment
    CSockSession & operator =(const CSockSession &);
    //设置事件标志
    void addEvents(__Events ev){ev_ |= ev;}
    //检查接收数据: 长度，格式等
    bool checkRecvBuf(__Socket & socket, const __SockAddr & addr);
    //修改发送数据队列(FIFO)，线程安全
    void pushSendElem(const __SendElem & elem, bool front);
    bool popSendElem(__SendElem & elem);
    //fields
    __Socket * const sock_;
    CServerCallback * const callback_;
    __NamePtr name_;
    __LockEvents ev_;
    std::string recvBuf_;
    __PkgList pkgList_;
    __SendList sendList_;
    __SendLock sendLock_;
};

typedef marine::CLockQueue<__NamePtr>       __NameQue;
typedef marine::CFdDataMap<CSockSession>    __FdSockMap;
typedef __FdSockMap::pointer                __SockPtr;
typedef __SockPtr::element_type             __SockSession;
typedef marine::CLockQueue<int>             __FdQue;
typedef marine::CLockQueue<CFdEvent>        __FdEventQue;

NS_HSHA_IMPL_END

#endif

