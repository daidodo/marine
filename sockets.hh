#ifndef DOZERG_SOCKETS_H_20080229
#define DOZERG_SOCKETS_H_20080229

/*
    ������socket�ļ򵥰�װ��������IPv4��IPv6Э�������
        CSocket         ����socket����
        CUdpSocket      udp socket
        CListenSocket   tcp�������˼���socket
        CTcpConnSocket  tcp�ͻ���socket
//*/

#include "file.hh"
#include "sock_addr.hh"

NS_SERVER_BEGIN

class CSocket : public IFileDesc
{
protected:
    //Ŀǰ֧�ֵ�socket����
    enum EType{kTcp, kUdp};
    CSockAddr peer_;    //opt peer addr
public:
    CSocket(){}
    //��ȡ���˵�ַ
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
    //��ȡ�Զ˵�ַ
    const CSockAddr & peerAddr() const{return peer_;}
    //����close()ʱ�Ƿ�ȴ����ݷ���
    //on:
    //  false   timeout��ֵ�����ԣ������ں�ȱʡ�����close()���������أ�������ܽ��ᴫ���κ�δ���͵�����
    //  true:
    //      timeout==0  close()ʱ����ز�ۣ��������ͻ��������κ����ݲ�����һ��RST���Է���������ͨ�����ķ�����ֹ���У��ܱ���TIME_WAIT״̬
    //      timeout!=0  close()ʱ������ͻ��������Բ������ݣ����̽�����˯��״̬��ֱ��
    //                  (a)�������ݷ������ұ��Է�ȷ�ϣ�֮�������������ֹ���У������ַ��ʼ���Ϊ0��
    //                  (b)�ӳ�ʱ�䵽����ʱclose()������EWOULDBLOCK�����ҷ��ͻ������е��κ����ݶ���ʧ
    //timeoutS: �ȴ�ʱ�䣬Linux�µ�λΪ��
    //return:
    //  true    ���óɹ�
    //  false   ����ʧ��
    bool linger(bool on, int timeoutS = 0){
        struct linger ling;
        ling.l_onoff = (on ? 1 : 0);
        ling.l_linger = timeoutS;
        return setOpt(SO_LINGER, ling);
    }
    //���ö˿��ͷź��Ƿ���������ٴ�ʹ��
    bool reuseAddr(bool on){
        int flag = (on ? 1 : 0);
        return setOpt(SO_REUSEADDR, flag);
    }
    //����/��ȡ���ճ�ʱ������������ʽ��Ч
    //timeMs: ��ʱʱ�䣬��λ���룻0Ϊ������ʱ
    bool recvTimeout(int timeMs){
        if(timeMs < 0)
            return false;
        struct timeval tv;
        tv.tv_sec = timeMs / 1000;
        tv.tv_usec = timeMs % 1000 * 1000;
        return setOpt(SO_RCVTIMEO, tv);
    }
    //return:
    //  <0  ����
    //  0   ������ʱ
    //  >0  ��ʱʱ�䣬��λ����
    int recvTimeout() const{
        struct timeval tv;
        if(!getOpt(SO_RCVTIMEO, &tv))
            return -1;
        return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
    }
    //����/��ȡ���ͳ�ʱ������������ʽ��Ч
    //timeMs: ��ʱʱ�䣬��λ���룻0Ϊ������ʱ
    bool sendTimeout(uint32_t timeMs){
        if(timeMs < 0)
            return false;
        struct timeval tv;
        tv.tv_sec = timeMs / 1000;
        tv.tv_usec = timeMs % 1000 * 1000;
        return setOpt(SO_SNDTIMEO, tv);
    }
    //return:
    //  <0  ����
    //  0   ������ʱ
    //  >0  ��ʱʱ�䣬��λ����
    int sendTimeout() const{
        struct timeval tv;
        if(!getOpt(SO_SNDTIMEO, &tv))
            return -1;
        return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
    }
    //����/��ȡ���ջ�������С
    //ע�⣺
    //Linuxʵ���ϻ����û�������СΪ(2*size)�ֽڣ��Դ˵Ľ������£�
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
    //  <0  ����
    //  >=0 ���ջ�������С����λ�ֽ�
    int recvBufSize() const{
        int size;
        if(!getOpt(SO_RCVBUF, &size))
            return -1;
        return size;
    }
    //����/��ȡ���ͻ�������С
    //ע�⣺
    //Linuxʵ���ϻ����û�������СΪ(2*size)�ֽڣ��Դ˵Ľ������£�
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
    //  <0  ����
    //  >=0 ���ջ�������С����λ�ֽ�
    int sendBufSize() const{
        int size;
        if(!getOpt(SO_SNDBUF, &size))
            return -1;
        return size;
    }
    //��������
    //sz: �����յ������ݴ�С����λ�ֽ�
    //waitAll: �Ƿ��MSG_WAITALL��־
    //return:
    //  <0  ʧ��
    //  0   �Է��ر�����
    //  >0  ʵ���յ������ݴ�С����λ�ֽ�
    ssize_t recvData(char * buf, size_t sz, bool waitAll = false){
        if(NULL == buf || !valid())
            return -1;
        return ::recv(fd(), buf, sz, (waitAll ? MSG_WAITALL : 0));
    }
    //�յ������ݻ�׷�ӵ�buf����
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
    //��������
    //return:
    //  <0  ʧ��
    //  >=0 ʵ�ʷ��͵����ݴ�С����λ�ֽ�
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
    //�ڲ�״̬��������Ҫ����log
    std::string toString() const{
        CToString oss;
        oss<<"{IFileDesc="<<IFileDesc::toString()
            <<", host="<<hostAddr().toString()
            <<", peer="<<peerAddr().toString()
            <<"}";
        return oss.str();
    }
protected:
    //��ʼ��
    //family: Э����(AF_INET/AF_INET6��)
    CSocket(int family, EType type){
        getSock(family, type);
    }
    bool getSock(int family, EType type){
        if(valid())
            return false;   //�������ظ���ʼ��
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
                    || EINPROGRESS == errno);   //�������£���Ҫ�ȴ�
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
    //��ʼ��
    //family: Э����(AF_INET/AF_INET6��)
    explicit CUdpSocket(int family)
        : CSocket(family, kUdp)
    {}
    bool getSock(int family){return CSocket::getSock(family, kUdp);}
    //����fd���ͺ�����
    int fdType() const{return kFdType;}
    const char * fdTypeName() const{return "CUdpSocket";}
    //���ñ��˵�ַ
    bool bindAddr(const CSockAddr & addr){
        if(!addr.valid())
            return false;
        if(!valid() && !getSock(addr.family()))
            return false;
        return bindTo(addr);
    }
    //���öԶ˵�ַ�����Զ�ε������ò�ͬ��ַ
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
    //��������
    //from: ���ضԶ˵�ַ���������Ҫ�������CSocket::recvData
    //sz: �����յ������ݴ�С����λ�ֽ�
    //waitAll: �Ƿ��MSG_WAITALL��־
    //return:
    //  <0  ʧ��
    //  >=0 ʵ���յ������ݴ�С����λ�ֽ�
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
    //�յ������ݻ�׷�ӵ�buf����
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
    //��������
    //to: ָ���Զ˵�ַ�������Ч�������CSocket::sendData
    //return:
    //  <0  ʧ��
    //  >=0 ʵ�ʷ��͵����ݴ�С����λ�ֽ�
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
    //��ʼ��
    //family: Э����(AF_INET/AF_INET6��)
    CTcpConnSocket(){}
    explicit CTcpConnSocket(int family)
        : CSocket(family, kTcp)
    {}
    bool getSock(int family){return CSocket::getSock(family, kTcp);}
    //����fd���ͺ�����
    int fdType() const{return kFdType;}
    const char * fdTypeName() const{return "CTcpConnSocket";}
    //���ñ��˵�ַ
    bool bindAddr(const CSockAddr & addr){
        if(!addr.valid())
            return false;
        if(!valid() && !getSock(addr.family()))
            return false;
        return bindTo(addr);
    }
    //���ӷ�����
    //ע�⣺������ģʽ�£�����true��һ�����ӳɹ�
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
    //�Ͽ��������ӣ��������ӷ�����
    //ע�⣺������ģʽ�£�����true��һ�����ӳɹ�
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
    //��ʼ��
    //family: Э����(AF_INET/AF_INET6��)
    CListenSocket(){}
    explicit CListenSocket(int family)
        : CSocket(family, kTcp)
    {}
    bool getSock(int family){return CSocket::getSock(family, kTcp);}
    //����fd���ͺ�����
    int fdType() const{return kFdType;}
    const char * fdTypeName() const{return "CListenSocket";}
    //���ñ��˵�ַ
    bool bindAddr(const CSockAddr & addr){
        if(!addr.valid())
            return false;
        if(!valid() && !getSock(addr.family()))
            return false;
        return bindTo(addr);
    }
    //����ָ����ַ
    //queueSz: �ȴ����е���󳤶�
    //  <=0     ʹ��Ĭ�ϳ���
    //  ����    ָ������
    //block:
    //  true    ������ʽ
    //  false   ��������ʽ
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
    //����Զ������
    //sock: ���������ӵ�socket
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
                || EWOULDBLOCK == errno); //����������Ϊ����
    }
};

NS_SERVER_END

#endif

