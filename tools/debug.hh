#ifndef DOZERG_TOOLS_DEBUG_H_20130122
#define DOZERG_TOOLS_DEBUG_H_20130122

/*
    主要用于调试输出的函数
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

// TODO: wchar_t, char16_t, char32_t
namespace tools
{
    //得到数据v的16进制字符串表示
    //默认格式示例："abc" = (3)61 62 63
    //sep为分隔符,默认' '
    //hasLen表示是否有前面的数据长度"(3)"
    inline std::string DumpHex(const char * v, size_t sz, char sep = ' ', bool hasLen = true)
    {
        return NS_IMPL::CDebug::DumpHex(v, sz, sep, hasLen);
    }

    inline std::string DumpHex(const signed char * v, size_t sz, char sep = ' ', bool hasLen = true)
    {
        return NS_IMPL::CDebug::DumpHex(v, sz, sep, hasLen);
    }

    inline std::string DumpHex(const unsigned char * v, size_t sz, char sep = ' ', bool hasLen = true)
    {
        return NS_IMPL::CDebug::DumpHex(v, sz, sep, hasLen);
    }

    inline std::string DumpHex(const wchar_t * v, size_t sz, char sep = ' ', bool hasLen = true)
    {
        return NS_IMPL::CDebug::DumpHex(v, sz, sep, hasLen);
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

    //得到数据v的可打印形式，非可读字符用replace代替
    //默认格式示例："a\03\t\0bc" = (6)a.\t\0bc
    //replace为替代符，默认'.'
    //hasLen表示是否有前面的数据长度"(6)"
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

    //得到数据v的可打印形式，不可打印字符用base进制数值表示
    //默认格式示例："a\t\223bc" = (5)a\t\223bc
    //base取值为8，16
    //hasLen表示是否有前面的数据长度"(5)"
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

    //按照xxd的格式显示字符串的16进制和可视形式
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

    //得到数据v的可打印形式，自动选择DumpHex/DumpStr/DumpVal/DumpXxd
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

    //调用对象指针p的toString()函数时，进行安全检查
    template<class Ptr>
    std::string ToStringPtr(const Ptr & p){
        return (p ? p->toString() : "NULL");
    }

    //将uint32_t类型的v及每个bit的name显示出来
    //name: 指定每个bit的名称，为0时不显示
    //name_len: name数组的长度
    //return示例: 3(IN | OUT)
    inline std::string ToStringBits(uint32_t v, const char * const * name, size_t name_len)
    {
        return NS_IMPL::CDebug::ToStringBits(v, name, name_len);
    }

    //得到16进制字符a表示的10进制数值，错误时返回-1
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

    //把数据的16进制还原成数据本身
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

