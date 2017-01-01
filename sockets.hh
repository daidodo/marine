#ifndef DOZERG_SOCKETS_H_20080229
#define DOZERG_SOCKETS_H_20080229

/*
    对网络socket的简单包装，隐藏了IPv4和IPv6协议相关性
        CSocket         网络socket基类
        CUdpSocket      udp socket
        CListenSocket   tcp服务器端监听socket
        CTcpConnSocket  tcp客户端socket
//*/

#include "file.hh"
#include "sock_addr.hh"

NS_SERVER_BEGIN

class CSocket : public IFileDesc
{
protected:
    //目前支持的socket类型
    enum EType{kTcp, kUdp};
    CSockAddr peer_;    //opt peer addr
public:
    CSocket(){}
    //获取本端地址
    CSockAddr hostAddr() const{
        CSockAddr addr;
        if(valid()){
            struct sockaddr_storage sa;
            socklen_t len = sizeof sa;
            ::memset(&sa, 0, len);
            if(0 == ::getsockname(fd(), reinterpret_cast<struct sockaddr *>(&sa), &len))
                addr.setAddr(sa, len);
        }
        return addr;
    }
    //获取对端地址
    const CSockAddr & peerAddr() const{return peer_;}
    //设置close()时是否等待数据发送
    //on:
    //  false   timeout的值被忽略，等于内核缺省情况，close()会立即返回，如果可能将会传输任何未发送的数据
    //  true:
    //      timeout==0  close()时连接夭折，丢弃发送缓冲区的任何数据并发送一个RST给对方，而不是通常的四分组终止序列，能避免TIME_WAIT状态
    //      timeout!=0  close()时如果发送缓冲区中仍残留数据，进程将处于睡眠状态，直到
    //                  (a)所有数据发送完且被对方确认，之后进行正常的终止序列（描述字访问计数为0）
    //                  (b)延迟时间到，此时close()将返回EWOULDBLOCK错误，且发送缓冲区中的任何数据都丢失
    //timeoutS: 等待时间，Linux下单位为秒
    //return:
    //  true    设置成功
    //  false   设置失败
    bool linger(bool on, int timeoutS = 0){
        struct linger ling;
        ling.l_onoff = (on ? 1 : 0);
        ling.l_linger = timeoutS;
        return setOpt(SO_LINGER, ling);
    }
    //设置端口释放后是否可以立即再次使用
    bool reuseAddr(bool on){
        int flag = (on ? 1 : 0);
        return setOpt(SO_REUSEADDR, flag);
    }
    //设置/获取接收超时，仅对阻塞方式有效
    //timeMs: 超时时间，单位毫秒；0为永不超时
    bool recvTimeout(int timeMs){
        if(timeMs < 0)
            return false;
        struct timeval tv;
        tv.tv_sec = timeMs / 1000;
        tv.tv_usec = timeMs % 1000 * 1000;
        return setOpt(SO_RCVTIMEO, tv);
    }
    //return:
    //  <0  出错
    //  0   永不超时
    //  >0  超时时间，单位毫秒
    int recvTimeout() const{
        struct timeval tv;
        if(!getOpt(SO_RCVTIMEO, &tv))
            return -1;
        return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
    }
    //设置/获取发送超时，仅对阻塞方式有效
    //timeMs: 超时时间，单位毫秒；0为永不超时
    bool sendTimeout(uint32_t timeMs){
        if(timeMs < 0)
            return false;
        struct timeval tv;
        tv.tv_sec = timeMs / 1000;
        tv.tv_usec = timeMs % 1000 * 1000;
        return setOpt(SO_SNDTIMEO, tv);
    }
    //return:
    //  <0  出错
    //  0   永不超时
    //  >0  超时时间，单位毫秒
    int sendTimeout() const{
        struct timeval tv;
        if(!getOpt(SO_SNDTIMEO, &tv))
            return -1;
        return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
    }
    //设置/获取接收缓冲区大小
    //注意：
    //Linux实际上会设置缓冲区大小为(2*size)字节，对此的解释如下：
    //    People regularly wonder whether the "*2" here
    //    is correct. Linux reserves half of the socket
    //    buffer for metadata (skbuff headers etc.)
    //    BSD doesn't do that. Most programs using
    //    SO_SNDBUF/SO_RCVBUF didn't expect this, because
    //    traditional BSD does not do metadata accounting,
    //    and on Linux they ended up with too small effective
    //    buffers. To fix this Linux always doubles the
    //    buffer internally to stay compatible.
    //    See also socket(7).
    bool recvBufSize(int size){
        if(size < 0)
            return false;
        return setOpt(SO_RCVBUF, size);
    }
    //return:
    //  <0  出错
    //  >=0 接收缓冲区大小，单位字节
    int recvBufSize() const{
        int size;
        if(!getOpt(SO_RCVBUF, &size))
            return -1;
        return size;
    }
    //设置/获取发送缓冲区大小
    //注意：
    //Linux实际上会设置缓冲区大小为(2*size)字节，对此的解释如下：
    //    People regularly wonder whether the "*2" here
    //    is correct. Linux reserves half of the socket
    //    buffer for metadata (skbuff headers etc.)
    //    BSD doesn't do that. Most programs using
    //    SO_SNDBUF/SO_RCVBUF didn't expect this, because
    //    traditional BSD does not do metadata accounting,
    //    and on Linux they ended up with too small effective
    //    buffers. To fix this Linux always doubles the
    //    buffer internally to stay compatible.
    //    See also socket(7).
    bool sendBufSize(int size){
        if(size < 0)
            return false;
        return setOpt(SO_SNDBUF, size);
    }
    //return:
    //  <0  出错
    //  >=0 接收缓冲区大小，单位字节
    int sendBufSize() const{
        int size;
        if(!getOpt(SO_SNDBUF, &size))
            return -1;
        return size;
    }
    //接收数据
    //sz: 期望收到的数据大小，单位字节
    //waitAll: 是否加MSG_WAITALL标志
    //return:
    //  <0  失败
    //  0   对方关闭连接
    //  >0  实际收到的数据大小，单位字节
    ssize_t recvData(char * buf, size_t sz, bool waitAll = false){
        if(NULL == buf || !valid())
            return -1;
        return ::recv(fd(), buf, sz, (waitAll ? MSG_WAITALL : 0));
    }
    //收到的数据会追加到buf后面
    template<class BufT>
    ssize_t recvData(BufT & buf, size_t sz, bool waitAll = false){
        if(!valid())
            return -1;
        const size_t oldsz = buf.size();
        buf.resize(oldsz + sz);
        const ssize_t ret = recvData(&buf[oldsz], sz, waitAll);
        if(ret <= 0)
            buf.resize(oldsz);
        else if(size_t(ret) < sz)
            buf.resize(oldsz + ret);
        return ret;
    }
    //发送数据
    //return:
    //  <0  失败
    //  >=0 实际发送的数据大小，单位字节
    ssize_t sendData(const char * buf, size_t sz){
        if(!valid())
            return -1;
        if(NULL == buf || 0 == sz)
            return 0;
        return ::send(fd(), buf, sz, MSG_NOSIGNAL);
    }
    template<class BufT>
    ssize_t sendData(const BufT & buf){
        if(buf.empty())
            return 0;
        return sendData(&buf[0], buf.size());
    }
    //内部状态描述，主要用于log
    std::string toString() const{
        CToString oss;
        oss<<"{IFileDesc="<<IFileDesc::toString()
            <<", host="<<hostAddr().toString()
            <<", peer="<<peerAddr().toString()
            <<"}";
        return oss.str();
    }
protected:
    //初始化
    //family: 协议族(AF_INET/AF_INET6等)
    CSocket(int family, EType type){
        getSock(family, type);
    }
    bool getSock(int family, EType type){
        if(valid())
            return false;   //不允许重复初始化
        if(AF_INET6 == family)  //not supported
            family = AF_INET;
        switch(type){
            case kTcp:
                fd_ = ::socket(family, SOCK_STREAM, IPPROTO_TCP);
                break;
            case kUdp:
                fd_ = ::socket(family, SOCK_DGRAM, IPPROTO_UDP);
                break;
            default:
                return false;
        }
        return valid();
    }
    bool bindTo(const CSockAddr & addr){
        if(valid() && addr.valid())
            return (0 == ::bind(fd(), addr.sockaddr(), addr.socklen()));
        return false;
    }
    bool connectTo(const CSockAddr & addr){
        if(valid() && addr.valid())
            return (0 == ::connect(fd(), addr.sockaddr(), addr.socklen())
                    || EINPROGRESS == errno);   //非阻塞下，需要等待
        return false;
    }
private:
    template<class T>
    bool setOpt(int name, const T & v){
        return (valid()
                && 0 == ::setsockopt(fd(), SOL_SOCKET, name, &v, sizeof v));
    }
    template<class T>
    bool getOpt(int name, T * v) const{
        assert(v);
        socklen_t len = sizeof(T);
        return (valid()
                && 0 == ::getsockopt(fd(), SOL_SOCKET, name, v, &len));
    }
};

class CUdpSocket : public CSocket
{
public:
    static const int kFdType = 4;
    CUdpSocket(){}
    //初始化
    //family: 协议族(AF_INET/AF_INET6等)
    explicit CUdpSocket(int family)
        : CSocket(family, kUdp)
    {}
    bool getSock(int family){return CSocket::getSock(family, kUdp);}
    //返回fd类型和名称
    int fdType() const{return kFdType;}
    const char * fdTypeName() const{return "CUdpSocket";}
    //设置本端地址
    bool bindAddr(const CSockAddr & addr){
        if(!addr.valid())
            return false;
        if(!valid() && !getSock(addr.family()))
            return false;
        return bindTo(addr);
    }
    //设置对端地址，可以多次调用设置不同地址
    bool connectAddr(const CSockAddr & addr){
        if(!addr.valid())
            return false;
        if(!valid() && !getSock(addr.family()))
            return false;
        if(!connectTo(addr))
            return false;
        peer_ = addr;
        return true;
    }
    //接收数据
    //from: 返回对端地址，如果不需要，请调用CSocket::recvData
    //sz: 期望收到的数据大小，单位字节
    //waitAll: 是否加MSG_WAITALL标志
    //return:
    //  <0  失败
    //  >=0 实际收到的数据大小，单位字节
    using CSocket::recvData;
    ssize_t recvData(CSockAddr & from, char * buf, size_t sz, bool waitAll = false){
        if(NULL == buf || !valid())
            return -1;
        struct sockaddr_storage addr;
        socklen_t len = sizeof addr;
        ::memset(&addr, 0, len);
        ssize_t ret = ::recvfrom(fd(), buf, sz, (waitAll ? MSG_WAITALL : 0), reinterpret_cast<sockaddr *>(&addr), &len);
        if(ret >= 0)
            from.setAddr(addr, len);
        return ret;
    }
    //收到的数据会追加到buf后面
    template<class BufT>
    ssize_t recvData(CSockAddr & from, BufT & buf, size_t sz, bool waitAll = false){
        if(!valid())
            return -1;
        struct sockaddr_storage addr;
        socklen_t len = sizeof addr;
        ::memset(&addr, 0, len);
        size_t oldsz = buf.size();
        buf.resize(oldsz + sz);
        ssize_t ret = recvData(from, &buf[oldsz], sz, waitAll);
        if(ret >= 0){
            from.setAddr(addr, len);
            if(size_t(ret) < sz)
                buf.resize(oldsz + ret);
        }else
            buf.resize(oldsz);
        return ret;
    }
    //发送数据
    //to: 指定对端地址，如果无效，则调用CSocket::sendData
    //return:
    //  <0  失败
    //  >=0 实际发送的数据大小，单位字节
    using CSocket::sendData;
    ssize_t sendData(const CSockAddr & to, const char * buf, size_t sz){
        if(!valid() || NULL == buf)
            return -1;
        if(!sz)
            return 0;
        if(!to.valid())
            return CSocket::sendData(buf, sz);
        return ::sendto(fd(), buf, sz, MSG_NOSIGNAL, to.sockaddr(),to.socklen());
    }
    template<class BufT>
    ssize_t sendData(const CSockAddr & to, const BufT & buf){
        if(buf.empty())
            return 0;
        return sendData(to, &buf[0], buf.size());
    }
};

class CListenSocket;

class CTcpConnSocket : public CSocket
{
    friend class CListenSocket;
public:
    static const int kFdType = 2;
    //初始化
    //family: 协议族(AF_INET/AF_INET6等)
    CTcpConnSocket(){}
    explicit CTcpConnSocket(int family)
        : CSocket(family, kTcp)
    {}
    bool getSock(int family){return CSocket::getSock(family, kTcp);}
    //返回fd类型和名称
    int fdType() const{return kFdType;}
    const char * fdTypeName() const{return "CTcpConnSocket";}
    //设置本端地址
    bool bindAddr(const CSockAddr & addr){
        if(!addr.valid())
            return false;
        if(!valid() && !getSock(addr.family()))
            return false;
        return bindTo(addr);
    }
    //连接服务器
    //注意：非阻塞模式下，返回true不一定连接成功
    bool connectAddr(const CSockAddr & addr){
        if(!addr.valid())
            return false;
        if(!valid() && !getSock(addr.family()))
            return false;
        if(!connectTo(addr))
            return false;
        peer_ = addr;
        return true;
    }
    //断开已有连接，重新连接服务器
    //注意：非阻塞模式下，返回true不一定连接成功
    bool reconnect(){
        CSockAddr peer = peerAddr();
        if(!peer.valid())
            return false;
        if(valid())
            this->close();
        return (getSock(peer.family())
                && connectTo(peer));
    }
};

class CListenSocket : public CSocket
{
public:
    static const int kQueueDefault = 1024;
    static const int kFdType = 3;
    //初始化
    //family: 协议族(AF_INET/AF_INET6等)
    CListenSocket(){}
    explicit CListenSocket(int family)
        : CSocket(family, kTcp)
    {}
    bool getSock(int family){return CSocket::getSock(family, kTcp);}
    //返回fd类型和名称
    int fdType() const{return kFdType;}
    const char * fdTypeName() const{return "CListenSocket";}
    //设置本端地址
    bool bindAddr(const CSockAddr & addr){
        if(!addr.valid())
            return false;
        if(!valid() && !getSock(addr.family()))
            return false;
        return bindTo(addr);
    }
    //监听指定地址
    //queueSz: 等待队列的最大长度
    //  <=0     使用默认长度
    //  其他    指定长度
    //block:
    //  true    阻塞方式
    //  false   非阻塞方式
    bool listenAddr(const CSockAddr & addr, int queueSz = kQueueDefault){
        if(!addr.valid())
            return false;
        if(!valid() && !bindAddr(addr))
            return false;
        reuseAddr(true);
        if(queueSz <= 0)
            queueSz = kQueueDefault;
        return (0 == ::listen(fd(), queueSz));
    }
    bool listenAddr(const CSockAddr & addr, int queueSz, bool block){
        if(!addr.valid())
            return false;
        if(!valid() && (!getSock(addr.family()) || !bindTo(addr)))
            return false;
        reuseAddr(true);
        if(!this->block(block))
            return false;
        if(queueSz <= 0)
            queueSz = kQueueDefault;
        return (0 == ::listen(fd(), queueSz));
    }
    //接受远程连接
    //sock: 返回已连接的socket
    bool acceptSocket(CTcpConnSocket & sock) const{
        if(sock.valid() || !valid())
            return false;
        struct sockaddr_storage ss;
        socklen_t len = sizeof ss;
        sock.fd_ = ::accept(fd(), reinterpret_cast<struct sockaddr *>(&ss), &len);
        if(sock.valid())
            sock.peer_.setAddr(ss, len);
        return (sock.valid()
                || EINTR == errno
                || EAGAIN == errno
                || EWOULDBLOCK == errno); //非阻塞不作为错误
    }
};

NS_SERVER_END

#endif

