#ifndef DOZERG_TOOLS_DEBUG_H_20130122
#define DOZERG_TOOLS_DEBUG_H_20130122

/*
    ��Ҫ���ڵ�������ĺ���
        DumpHex
        DumpStr
        DumpVal
        DumpXxd
        Dump
        ToStringPtr
        ToStringBits
        UnHex
*/

#include <vector>
#include "../char_buffer.hh"
#include "../impl/tools_debug_impl.hh"

NS_SERVER_BEGIN

namespace tools
{
    //�õ�����v��16�����ַ�����ʾ
    //Ĭ�ϸ�ʽʾ����"abc" = (3)61 62 63
    //sepΪ�ָ���,Ĭ��' '
    //hasLen��ʾ�Ƿ���ǰ������ݳ���"(3)"
    inline std::string DumpHex(const char * v, size_t sz, char sep = ' ', bool hasLen = true)
    {
        return NS_IMPL::CDebug::DumpHex(v, sz, sep, hasLen);
    }

    inline std::string DumpHex(const signed char * v, size_t sz, char sep = ' ', bool hasLen = true)
    {
        return DumpHex(reinterpret_cast<const char *>(v), sz, sep, hasLen);
    }

    inline std::string DumpHex(const unsigned char * v, size_t sz, char sep = ' ', bool hasLen = true)
    {
        return DumpHex(reinterpret_cast<const char *>(v), sz, sep, hasLen);
    }

    //TODO: unit test
    template<typename CharT, size_t N>
    inline std::string DumpHex(const CharT (&v)[N], char sep = ' ', bool hasLen = true)
    {
        return DumpHex(v, N, sep, hasLen);
    }

    //TODO: unit test
    template<typename CharT>
    inline std::string DumpHex(const std::vector<CharT> & v, char sep = ' ', bool hasLen = true)
    {
        return (v.empty() ? (hasLen ? "(0)" : std::string()) : DumpHex(&v[0], v.size(), sep, hasLen));
    }

    //TODO: unit test
    template<typename CharT>
    inline std::string DumpHex(const std::basic_string<CharT> & v, char sep = ' ', bool hasLen = true)
    {
        return DumpHex(v.c_str(), v.length(), sep, hasLen);
    }

    template<typename CharT>
    inline std::string DumpHex(const CCharBuffer<CharT> & v, char sep = ' ', bool hasLen = true)
    {
        return DumpHex(&v[0], v.size(), sep, hasLen);
    }

    //�õ�����v�Ŀɴ�ӡ��ʽ���ǿɶ��ַ���replace����
    //Ĭ�ϸ�ʽʾ����"a\03\t\0bc" = (6)a.\t\0bc
    //replaceΪ�������Ĭ��'.'
    //hasLen��ʾ�Ƿ���ǰ������ݳ���"(6)"
    inline std::string DumpStr(const char * v, size_t sz, char replace = '.', bool hasLen = true)
    {
        return NS_IMPL::CDebug::DumpStr(v, sz, replace, hasLen);
    }

    inline std::string DumpStr(const signed char * v, size_t sz, char replace = '.', bool hasLen = true)
    {
        return DumpStr(reinterpret_cast<const char *>(v), sz, replace, hasLen);
    }

    inline std::string DumpStr(const unsigned char * v, size_t sz, char replace = '.', bool hasLen = true)
    {
        return DumpStr(reinterpret_cast<const char *>(v), sz, replace, hasLen);
    }

    //TODO: unit test
    template<typename CharT, size_t N>
    inline std::string DumpStr(const CharT (&v)[N], char replace = '.', bool hasLen = true)
    {
        return DumpStr(v, N, replace, hasLen);
    }

    //TODO: unit test
    template<typename CharT>
    inline std::string DumpStr(const std::vector<CharT> & v, char replace = '.', bool hasLen = true)
    {
        return (v.empty() ? (hasLen ? "(0)" : std::string()) : DumpStr(&v[0], v.size(), replace, hasLen));
    }

    //TODO: unit test
    template<typename CharT>
    inline std::string DumpStr(const std::basic_string<CharT> & str, char replace = '.', bool hasLen = true)
    {
        return DumpStr(str.c_str(), str.length(), replace, hasLen);
    }

    template<typename CharT>
    inline std::string DumpStr(const CCharBuffer<CharT> & v, char replace = '.', bool hasLen = true)
    {
        return DumpStr(&v[0], v.size(), replace, hasLen);
    }

    //�õ�����v�Ŀɴ�ӡ��ʽ�����ɴ�ӡ�ַ���base������ֵ��ʾ
    //Ĭ�ϸ�ʽʾ����"a\t\223bc" = (5)a\t\223bc
    //baseȡֵΪ8��16
    //hasLen��ʾ�Ƿ���ǰ������ݳ���"(5)"
    inline std::string DumpVal(const char * v, size_t sz, int base = 8, bool hasLen = true)
    {
        return NS_IMPL::CDebug::DumpVal(v, sz, base, hasLen);
    }

    inline std::string DumpVal(const signed char * v, size_t sz, char base = 8, bool hasLen = true)
    {
        return DumpVal(reinterpret_cast<const char *>(v), sz, base, hasLen);
    }

    inline std::string DumpVal(const unsigned char * v, size_t sz, char base = 8, bool hasLen = true)
    {
        return DumpVal(reinterpret_cast<const char *>(v), sz, base, hasLen);
    }

    //TODO: unit test
    template<typename CharT, size_t N>
    inline std::string DumpVal(const CharT (&v)[N], char base = 8, bool hasLen = true)
    {
        return DumpVal(v, N, base, hasLen);
    }

    //TODO: unit test
    template<typename CharT>
    inline std::string DumpVal(const std::vector<CharT> & v, char base = 8, bool hasLen = true)
    {
        return (v.empty() ? (hasLen ? "(0)" : std::string()) : DumpVal(&v[0], v.size(), base, hasLen));
    }

    //TODO: unit test
    template<typename CharT>
    inline std::string DumpVal(const std::basic_string<CharT> & str, char base = 8, bool hasLen = true)
    {
        return DumpVal(str.c_str(), str.length(), base, hasLen);
    }

    template<typename CharT>
    inline std::string DumpVal(const CCharBuffer<CharT> & v, char base = 8, bool hasLen = true)
    {
        return DumpVal(&v[0], v.size(), base, hasLen);
    }

    //����xxd�ĸ�ʽ��ʾ�ַ�����16���ƺͿ�����ʽ
    inline std::string DumpXxd(const char * v, size_t sz, bool hasLen = true)
    {
        return NS_IMPL::CDebug::DumpXxd(v, sz, hasLen);
    }

    inline std::string DumpXxd(const unsigned char * v, size_t sz, bool hasLen = true)
    {
        return DumpXxd(reinterpret_cast<const char *>(v), sz, hasLen);
    }

    inline std::string DumpXxd(const signed char * v, size_t sz, bool hasLen = true)
    {
        return DumpXxd(reinterpret_cast<const char *>(v), sz, hasLen);
    }

    //TODO: unit test
    template<typename CharT, size_t N>
    inline std::string DumpXxd(const CharT (&v)[N], bool hasLen = true)
    {
        return DumpXxd(v, N, hasLen);
    }

    //TODO: unit test
    template<typename CharT>
    inline std::string DumpXxd(const std::vector<CharT> & v, bool hasLen = true)
    {
        return DumpXxd(&v[0], v.size(), hasLen);
    }

    //TODO: unit test
    template<typename CharT>
    inline std::string DumpXxd(const std::basic_string<CharT> & v, bool hasLen = true)
    {
        return DumpXxd(v.c_str(), v.length(), hasLen);
    }

    template<typename CharT>
    inline std::string DumpXxd(const CCharBuffer<CharT> & v, bool hasLen = true)
    {
        return DumpXxd(&v[0], v.size(), hasLen);
    }

    //�õ�����v�Ŀɴ�ӡ��ʽ���Զ�ѡ��DumpHex/DumpStr/DumpVal/DumpXxd
    inline std::string Dump(const char * v, size_t sz)
    {
        return NS_IMPL::CDebug::Dump(v, sz);
    }

    inline std::string Dump(const signed char * v, size_t sz)
    {
        return Dump(reinterpret_cast<const char *>(v), sz);
    }

    inline std::string Dump(const unsigned char * v, size_t sz)
    {
        return Dump(reinterpret_cast<const char *>(v), sz);
    }

    //TODO: unit test
    template<typename CharT, size_t N>
    inline std::string Dump(const CharT (&v)[N])
    {
        return Dump(v, N);
    }

    //TODO: unit test
    template<typename CharT>
    inline std::string Dump(const std::vector<CharT> & v)
    {
        return (v.empty() ? "(0)" : Dump(&v[0], v.size()));
    }

    //TODO: unit test
    template<typename CharT>
    inline std::string Dump(const std::basic_string<CharT> & str)
    {
        return Dump(str.c_str(), str.length());
    }

    template<typename CharT>
    inline std::string Dump(const CCharBuffer<CharT> & v)
    {
        return Dump(&v[0], v.size());
    }

    //���ö���ָ��p��toString()����ʱ�����а�ȫ���
    template<class Ptr>
    std::string ToStringPtr(const Ptr & p){
        return (p ? p->toString() : "NULL");
    }

    //��uint32_t���͵�v��ÿ��bit��name��ʾ����
    //name: ָ��ÿ��bit�����ƣ�Ϊ0ʱ����ʾ
    //name_len: name����ĳ���
    //returnʾ��: 3(IN | OUT)
    inline std::string ToStringBits(uint32_t v, const char * const * name, size_t name_len)
    {
        return NS_IMPL::CDebug::ToStringBits(v, name, name_len);
    }

    //�õ�16�����ַ�a��ʾ��10������ֵ������ʱ����-1
    inline int UnHex(char a)
    {
        return NS_IMPL::CDebug::UnHex(a);
    }

    inline int UnHex(signed char a)
    {
        return UnHex(char(a));
    }

    inline int UnHex(unsigned char a)
    {
        return UnHex(char(a));
    }

    //�����ݵ�16���ƻ�ԭ�����ݱ���
    inline std::string UnHex(const char * v, size_t sz)
    {
        return NS_IMPL::CDebug::UnHex(v, sz);
    }

    inline std::string UnHex(const signed char * v, size_t sz)
    {
        return UnHex(reinterpret_cast<const char *>(v), sz);
    }

    inline std::string UnHex(const unsigned char * v, size_t sz)
    {
        return UnHex(reinterpret_cast<const char *>(v), sz);
    }

    //TODO: unit test
    template<typename CharT, size_t N>
    inline std::string UnHex(const CharT (&v)[N])
    {
        return UnHex(v, N);
    }

    //TODO: unit test
    template<typename CharT>
    inline std::string UnHex(const std::vector<CharT> & v)
    {
        return UnHex(&v[0], v.size());
    }

    //TODO: unit test
    template<typename CharT>
    inline std::string UnHex(const std::basic_string<CharT> & v)
    {
        return UnHex(v.c_str(), v.length());
    }

}//namespace tools

NS_SERVER_END

#endif

