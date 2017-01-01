#ifndef DOZERG_TOOLS_NET_H_20130122
#define DOZERG_TOOLS_NET_H_20130122

/*
    ������غ���
        HostByteOrder   ��ȡ��ǰϵͳ���ֽ���
        SwapByteOrder   ���������ֽ���
        Hton            ������ת������
        Ntoh            ������ת������
*/

#include <endian.h>             //BYTE_ORDER
#include "../template.hh"       //CByteOrderTraits

NS_SERVER_BEGIN

namespace tools
{
    //�õ��������ֽ���
    //return:
    //  true    little endian
    //  false   big endian(������)
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

    //�����������ֽ���
    //T: ����POD����
    //v: ����ֵ
    //return: �����ֽ�����ֵ
    template<typename T>
    inline T SwapByteOrder(T v)
    {
        return CByteOrderTraits<T, sizeof(T)>::swap(v);
    }

    //������ת������
    //T: ����POD����
    //v: ����ֵ
    //return: ת�����ֵ
    template<typename T>
    inline T Hton(T v)
    {
        static const bool kBO = HostByteOrder();
        return (kBO ? SwapByteOrder(v) : v);
    }

    //������ת������
    //T: ����POD����
    //v: ����ֵ
    //return: ת�����ֵ
    template<typename T>
    inline T Ntoh(T v){return Hton(v);}

}//namespace tools

NS_SERVER_END

#endif

