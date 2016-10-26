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
    //����Ƿ���Ч
    bool valid() const{return sock_ && sock_->valid();}
    //��ȡ��Ӧ��fd
    int fd() const{return (sock_ ? sock_->fd() : __Socket::kInvalidFd);}
    //��ȡfd����
    int fdType() const{return sock_->fdType();}
    //����/��ȡ�¼���־
    void events(__Events ev){ev_ = ev;}
    __Events events() const{return ev_;}
    //�ɶ�д�������յ������ݰ�
    __PkgList & pkgList(){return pkgList_;}
    //���Ӵ���������
    void addSendList(__SendList & list);
    //�����¿ͻ���
    bool acceptClient(CSockSession ** client);
    //��������
    bool recvTcp();
    bool recvUdp();
    //��������
    bool process(const CPkgData & data, __SendList & list);
    //��������
    bool sendTcp();
    bool sendUdp();
    //׼���ر�
    void onClose();
    //�ڲ�״̬
    std::string toString() const;
private:
    CSockSession(const CSockSession &);     //disable copy and assignment
    CSockSession & operator =(const CSockSession &);
    //�����¼���־
    void addEvents(__Events ev){ev_ |= ev;}
    //����������: ���ȣ���ʽ��
    bool checkRecvBuf(__Socket & socket, const __SockAddr & addr);
    //�޸ķ������ݶ���(FIFO)���̰߳�ȫ
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

