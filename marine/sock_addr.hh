#ifndef DOZERG_SOCKET_ADDR_H_20080229
#define DOZERG_SOCKET_ADDR_H_20080229

/*
    对网络地址的简单包装，隐藏了IPv4和IPv6协议相关性
        CSockAddr       IPv4或IPv6地址
//*/

#include <sys/types.h>
#include <sys/socket.h>     //socket
#include <arpa/inet.h>
#include <sys/ioctl.h>      //ioctl
#include <net/if.h>         //ifreq
#include <netdb.h>          //getaddrinfo
#include <unistd.h>
#include <cstring>          //memset,memcpy
#include <string>
#include <cassert>

#include "to_string.hh"

NS_SERVER_BEGIN

class CSockAddr
{
    typedef struct sockaddr_storage __SS;
    typedef struct sockaddr         __SA;
    typedef struct sockaddr_in      __SA4;
    typedef struct sockaddr_in6     __SA6;
public:
    //将ipv4转换成可读字符串
    //hostOrder: true-主机序; false-网络序
    static std::string IPv4String(uint32_t ipv4, bool hostOrder = true){
        struct in_addr in;
        in.s_addr = (hostOrder ? htonl(ipv4) : ipv4);
        return ipv4Str(in);
    }
    //将ipv6转换成可读字符串
    //return:
    //  true    成功
    //  false   失败
    static bool IPv6String(const struct in6_addr & ipv6, std::string * result){
        if(NULL == result)
            return false;
        std::string ret = ipv6Str(ipv6);
        if(ret.empty())
            return false;
        result->swap(ret);
        return true;
    }
    //将"ddd.ddd.ddd.ddd"形式字符串转换成为ipv4
    //hostOrder: true-主机序; false-网络序
    static uint32_t IPv4FromStr(const std::string & ipv4, bool hostOrder = true){
        struct in_addr in;
        if(ipv4.empty() || 0 >= inet_pton(AF_INET, strPtr(ipv4), &in))
            return 0;
        return (hostOrder ? ntohl(in.s_addr) : in.s_addr);
    }
    //将字符串转换成为ipv6
    //return:
    //  true    成功
    //  false   失败
    static bool IPv6FromStr(const std::string & ipv6, struct in6_addr * addr){
        return (NULL != addr && !ipv6.empty() && inet_pton(AF_INET6, strPtr(ipv6), addr) > 0);
    }
    //将接口名转换成为ipv4
    //hostOrder: true-主机序; false-网络序
    static uint32_t IPv4FromIf(const std::string & eth, bool hostOrder = true){
        __SS addr;
        const socklen_t len = tryIfr(strPtr(eth), &addr);
        if(sizeof(__SA4) != len)
            return 0;
        uint32_t ret = reinterpret_cast<const __SA4 *>(&addr)->sin_addr.s_addr;
        return (hostOrder ? ntohl(ret) : ret);
    }
    //默认构造
    CSockAddr(){}
    //设置地址
    //host: 主机名，域名，ip地址，本机接口名(eth1)
    //serv: 服务名或者端口号
    //ipv4: IPv4
    //ipv6: IPv6
    //port: 端口号
    //hostOrder: true-主机序; false-网络序
    //addr: 通用地址结构体
    //len: 地址结构体字节长度
    CSockAddr(const std::string & host, const std::string & serv){
        __SS addr;
        socklen_t len = tryGai(strPtr(host), strPtr(serv), &addr);
        if(setSs(addr, len))
            return;
        assert(!valid());
        if(!host.empty() && !setIpAux(host))
            return;         //invalid host
        if(!serv.empty() && !setPort(serv))
            clear();    //invalid serv
    }
    explicit CSockAddr(const std::string & host, uint16_t port = 0, bool hostOrder = true){
        if(!host.empty()){
            if(!setIpAux(host))
                return;     //invalid host
        }else if(!setPort("0")) //generate default ip
            return;
        assert(valid());
        if(port)
            setPortAux(port, hostOrder);
    }
    CSockAddr(const __SS & addr, socklen_t len){setAddr(addr, len);}
    CSockAddr(const __SA & addr, socklen_t len){setAddr(addr, len);}
    explicit CSockAddr(const __SA4 & addr){setAddr(addr);}
    explicit CSockAddr(const __SA6 & addr){setAddr(addr);}
    bool setIp(const std::string & host){
        const uint16_t port = getPort(false);   //old port
        if(!setIpAux(host))
            return false;
        assert(valid());
        if(port)
            setPortAux(port, false);
        return true;
    }
    bool setIp(uint32_t ipv4, bool hostOrder = true){
        if(setIpv4Aux(ipv4, hostOrder))
            return true;
        assert(!valid() || isIpv6());
        const uint16_t port = getPort(false);   //old port
        if(!setIpAux("0.0.0.0"))
            return false;
        assert(isIpv4());
        if(ipv4)
            setIpv4Aux(ipv4, hostOrder);
        if(port)
            setPortAux(port, false);
        return true;
    }
    bool setIp(const in6_addr & ipv6){
        //TODO: in case of invalid or ipv4
        if(!isIpv6())
            return false;
        sa<__SA6>()->sin6_addr = ipv6;
        return true;
    }
    bool setPort(const std::string & serv){
        char buf[sizeof(__SS)];
        __SS * const addr = reinterpret_cast<__SS *>(buf);
        socklen_t len = tryGai(NULL, strPtr(serv), addr);
        if(!valid())
            return setSs(*addr, len);
        assert(valid());
        uint16_t port = 0;
        if(sizeof(__SA4) == len){
            const __SA4 * const a4 = reinterpret_cast<const __SA4 *>(buf);
            port = a4->sin_port;
        }else if(sizeof(__SA6) == len){
            const __SA6 * const a6 = reinterpret_cast<const __SA6 *>(buf);
            port = a6->sin6_port;
        }else
            return false;
        return setPortAux(port, false);
    }
    bool setPort(uint16_t port, bool hostOrder = true){
        if(setPortAux(port, hostOrder))
            return true;
        assert(!valid());
        __SS addr;
        const socklen_t len = tryGai(NULL, "0", &addr);
        if(!setSs(addr, len))
            return false;
        assert(valid());
        if(port)
            setPortAux(port, hostOrder);
        return true;
    }
    bool setAddr(const __SS & addr, socklen_t len){return setSs(addr, len);}
    bool setAddr(const __SA & addr, socklen_t len){
        return setAddr(reinterpret_cast<const __SS &>(addr), len);
    }
    bool setAddr(const __SA4 & addr){
        return setAddr(reinterpret_cast<const __SS &>(addr), sizeof(__SA4));
    }
    bool setAddr(const __SA6 & addr){
        return setAddr(reinterpret_cast<const __SS &>(addr), sizeof(__SA6));
    }
    //获取IPv4
    //hostOrder: true-主机序; false-网络序
    uint32_t getIpv4(bool hostOrder = true) const{
        const uint32_t ret = (isIpv4() ? sa<__SA4>()->sin_addr.s_addr : 0);
        return (hostOrder ? htonl(ret) : ret);
    }
    //获取IPv6，返回是否成功
    bool getIpv6(struct in6_addr & ipv6) const{
        if(!isIpv6())
            return false;
        ipv6 = sa<__SA6>()->sin6_addr;
        return true;
    }
    //获取port
    //hostOrder: true-主机序; false-网络序
    uint16_t getPort(bool hostOrder = true) const{
        const uint16_t ret = (isIpv4() ? sa<__SA4>()->sin_port
                : (isIpv6() ? sa<__SA6>()->sin6_port
                    : 0));
        return (hostOrder ? htons(ret) : ret);
    }
    //清除地址
    void clear(){sa_.clear();}
    //返回地址是否有效
    bool valid() const{return (isIpv4() || isIpv6());}
    //返回可读字符串
    std::string toString() const{
        std::string ret = addr4Str();
        if(ret.empty())
            ret = addr6Str();
        if(ret.empty())
            return "unknown";
        return ret;
    }
    //获取协议族(AF_INET/AF_INET6等)
    int family() const{
        if(isIpv4())
            return AF_INET;
        else if(isIpv6())
            return AF_INET6;
        return AF_UNSPEC;
    }
    //转换成通用地址结构
    const __SA * sockaddr() const{
        return (sa_.empty() ? NULL : reinterpret_cast<const __SA *>(&sa_[0]));
    }
    //获取地址结构的字节长度
    socklen_t socklen() const{return sa_.size();}
private:
    bool isIpv4() const{
        return (NULL != sa<__SA4>() && AF_INET == sa<__SA4>()->sin_family);
    }
    bool isIpv6() const{
        return (NULL != sa<__SA6>() && AF_INET6 == sa<__SA6>()->sin6_family);
    }
    //re-interpret address
    template<class T>
    const T * sa() const{
        if(sa_.size() != sizeof(T))
            return NULL;
        return reinterpret_cast<const T *>(&sa_[0]);
    }
    template<class T>
    T * sa(){
        if(sa_.size() != sizeof(T))
            return NULL;
        return reinterpret_cast<T *>(&sa_[0]);
    }
    //translate ipv4 to readable string
    static std::string ipv4Str(const struct in_addr & addr){
        std::string buf(16, 0);
        if(NULL == inet_ntop(AF_INET, &addr, &buf[0], buf.size()))
            return std::string();
        buf.resize(std::char_traits<char>::length(buf.c_str()));
        return buf;
    }
    //translate ipv6 to readable string
    static std::string ipv6Str(const struct in6_addr & addr){
        std::string buf(128, 0);
        if(NULL == inet_ntop(AF_INET6, &addr, &buf[0], buf.size()))
            return std::string();
        buf.resize(std::char_traits<char>::length(buf.c_str()));
        return buf;
    }
    //try to translate current address to readable string
    std::string addr4Str() const{
        if(!isIpv4())
            return std::string();   //empty for error
        const __SA4 * const s = sa<__SA4>();
        CToString oss;
        oss<<ipv4Str(s->sin_addr)<<":"<<ntohs(s->sin_port);
        return oss.str();
    }
    std::string addr6Str() const{
        if(!isIpv6())
            return std::string();   //empty for error
        const __SA6 * const s = sa<__SA6>();
        CToString oss;
        oss<<'['<<ipv6Str(s->sin6_addr)<<"]:"<<ntohs(s->sin6_port);
        return oss.str();
    }
    //translate str to C string
    static const char * strPtr(const std::string & str){
        return (str.empty() ? NULL : str.c_str());
    }
    //parse node and service as host/ip and service/port
    //return:
    //  0   failed
    //  +n  length of addr
    static socklen_t tryGai(const char * node, const char * service, __SS * addr){
        assert(addr);
        if(NULL == node && NULL == service)
            return 0;
        struct addrinfo hints;
        ::memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        struct addrinfo * ai = NULL;
        if(0 != getaddrinfo(node, service, &hints, &ai) || NULL == ai)
            return 0;
        socklen_t len = ai->ai_addrlen;
        ::memcpy(addr, ai->ai_addr, len); //get first addr
        freeaddrinfo(ai);
        return len;
    }
    //parse node as interface name
    //return:
    //  0   failed
    //  +n  length of addr
    static socklen_t tryIfr(const char * node, __SS * addr){
        assert(addr);
        if(NULL == node)
            return 0;
        struct ifreq ifr;
        ::memset(&ifr, 0, sizeof ifr);
        strncpy(ifr.ifr_name, node, IFNAMSIZ);
        const int fd = socket(AF_INET, SOCK_STREAM, 0);
        if(fd < 0)
            return 0;
        int ret = ioctl(fd, SIOCGIFADDR, &ifr);
        ::close(fd);
        if(ret < 0)
            return 0;
        ::memset(addr, 0, sizeof(__SS));
        ret = ifr.ifr_addr.sa_family;
        socklen_t len = (AF_INET == ret ? sizeof(__SA4)
                : (AF_INET6 == ret ? sizeof(__SA6) : 0));
        if(len > sizeof ifr.ifr_addr)
            len = sizeof ifr.ifr_addr;
        ::memcpy(addr, &ifr.ifr_addr, len);
        return len;
    }
    //parse ip, set addr, clear port
    bool setIpAux(const std::string & host){
        __SS addr;
        socklen_t len = tryGai(strPtr(host), NULL, &addr);
        if(!len)
            len = tryIfr(strPtr(host), &addr);
        return setSs(addr, len);
    }
    //set ipv4 directly
    bool setIpv4Aux(uint32_t ipv4, bool hostOrder){
        if(!isIpv4())
            return false;
        if(hostOrder && ipv4)
            ipv4 = htonl(ipv4);
        sa<__SA4>()->sin_addr.s_addr = ipv4;
        return true;
    }
    //set port directly
    bool setPortAux(uint16_t port, bool hostOrder){
        if(hostOrder && port)
            port = htons(port);
        if(isIpv4())
            sa<__SA4>()->sin_port = port;
        else if(isIpv6())
            sa<__SA6>()->sin6_port = port;
        else
            return false;
        return true;
    }
    //set sockaddr_storage directly
    bool setSs(const __SS & addr, socklen_t len){
        if(sizeof(__SA4) != len && sizeof(__SA6) != len)
            return false;
        sa_.resize(len);
        ::memcpy(&sa_[0], &addr, sa_.size());
        assert(valid());
        return true;
    }
    //fields
    std::string sa_;
};

NS_SERVER_END

#endif

