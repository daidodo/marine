#ifndef DOZERG_TOOLS_NET_H_20130122
#define DOZERG_TOOLS_NET_H_20130122

/*
    网络相关函数
        HostByteOrder   获取当前系统的字节序
        SwapByteOrder   交换变量字节序
        Hton            主机序转网络序
        Ntoh            网络序转主机序
*/

#include <endian.h>             //BYTE_ORDER
#include "../template.hh"       //CByteOrderTraits

NS_SERVER_BEGIN

namespace tools
{
    //得到主机的字节序
    //return:
    //  true    little endian
    //  false   big endian(网络序)
    inline bool HostByteOrder()
    {
#ifdef BYTE_ORDER
#   if BYTE_ORDER == LITTLE_ENDIAN
        return true;
#   else
        return false;
#   endif
#else
        int t = 1;
        char * p = reinterpret_cast<char *>(&t);
        return (*p == 1);
#endif
    }

    //交换变量的字节序
    //T: 整型POD类型
    //v: 变量值
    //return: 交换字节序后的值
    template<typename T>
    inline T SwapByteOrder(T v)
    {
        return CByteOrderTraits<T, sizeof(T)>::swap(v);
    }

    //主机序转网络序
    //T: 整型POD类型
    //v: 变量值
    //return: 转换后的值
    template<typename T>
    inline T Hton(T v)
    {
        static const bool kBO = HostByteOrder();
        return (kBO ? SwapByteOrder(v) : v);
    }

    //网络序转主机序
    //T: 整型POD类型
    //v: 变量值
    //return: 转换后的值
    template<typename T>
    inline T Ntoh(T v){return Hton(v);}

}//namespace tools

NS_SERVER_END

#endif

