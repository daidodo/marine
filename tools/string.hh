#ifndef DOZERG_TOOLS_STRING_H_20130122
#define DOZERG_TOOLS_STRING_H_20130122

/*
    字符串处理相关函数
        Trim
        StringMatch
        StringMatchKmp
        AbsFilename
        Basename
        ExtractArg
        GetHost
        UrlEncode
        UrlDecode
        XmlEncode
        IsTextUtf8
        IsTextGbk
        IsTextGb2312
*/

#include <libgen.h>     //basename
#include "../impl/tools_string_impl.hh"

NS_SERVER_BEGIN

namespace tools
{
    //去除str头尾的空白符
    inline std::string Trim(const std::string & str)
    {
        return NS_IMPL::CString::Trim(str);
    }

    //在src串里匹配target串
    //找到返回匹配串的起始位置
    //未找到返回-1
    inline ssize_t StringMatch(const char * src, size_t srclen, const char * target, size_t tarlen)
    {
        return NS_IMPL::CString::StringMatch(src, srclen, target, tarlen);
    }

    //在src串里匹配target串，使用KMP算法
    //找到返回匹配串的起始位置
    //未找到返回-1
    inline ssize_t StringMatchKmp(const char * src, size_t srclen, const char * target, size_t tarlen)
    {
        return NS_IMPL::CString::StringMatchKmp(src, srclen, target, tarlen);
    }

    //如果fname不是绝对路径，补全成绝对路径后返回
    inline std::string AbsFilename(const std::string & fname)
    {
        return NS_IMPL::CString::AbsFilename(fname);
    }

    //去掉fname里的目录部分，只留文件名
    inline std::string Basename(const std::string & fname)
    {
        return fname.substr(fname.find_last_of('/') + 1);
    }

    inline const char * Basename(const char * fname)
    {
        if(!fname)
            return NULL;
        if(*fname == '\0')
            return "";  // compatible with other overloads
        return ::basename(const_cast<char *>(fname));
    }

    //获取文件主名（去掉后缀名）
    inline std::string FilePrimeName(const std::string & fname)
    {
        std::string bname = Basename(fname);
        const size_t pos = bname.find_last_of('.');
        return bname.substr(0, pos);
    }

    //获取文件的类型（后缀名）
    inline std::string FileType(const std::string & fname)
    {
        std::string bname = Basename(fname);
        size_t pos = bname.find_last_of('.');
        if(std::string::npos == pos)
            return std::string();
        return bname.substr(pos + 1);
    }

    inline const char * FileType(const char * fname)
    {
        if(!fname)
            return NULL;
        const char * r = NULL;
        for(const char * c = Basename(fname);c && *c;++c)
            if(*c == '.')
                r = c + 1;
        return r;
    }

    //解析命令行选项
    //argstr: 待解析字符串
    //pattern: 需要匹配的字符串
    //result: argstr去掉pattern后剩下的参数部分，可以为NULL
    //return:
    //  如果(result==NULL)，则返回(0==strcmp(argstr, pattern))
    //  否则返回argstr是否匹配到pattern
    //例如:
    //argstr = "-file=example.txt"
    //当匹配 pattern = "-file=" 且 result = NULL 时，返回false
    //当匹配 pattern = "-file=" 且 result != NULL 时，返回true,且得到 *result = "example.txt"
    //当匹配 pattern = "-logfile="时,返回false
    inline bool ExtractArg(const char * argstr, const char * pattern, const char ** result = NULL)
    {
        return NS_IMPL::CString::ExtractArg(argstr, pattern, result);
    }

    //从url里取出host
    inline std::string GetHost(const std::string & url)
    {
        return NS_IMPL::CString::GetHost(url);
    }

    //url转码处理
    inline std::string UrlEncode(const std::string & url)
    {
        return NS_IMPL::CString::UrlEncode(url);
    }

    inline std::string UrlDecode(const std::string & url)
    {
        return NS_IMPL::CString::UrlDecode(url);
    }

    //std::string XmlEncode(const std::string & val)
    //{
    //    return NS_IMPL::CString::XmlEncode(val);
    //}

    inline bool IsTextUtf8(const std::string & txt)
    {
        return NS_IMPL::CString::IsTextUtf8(txt);
    }

    //return:
    //  0   txt is NOT GBK encoded
    //  1   txt is Level GBK/1
    //  2   txt is Level GBK/2
    //  3   txt is Level GBK/3
    //  4   txt is Level GBK/4
    //  5   txt is Level GBK/5
    //  6   txt is user-defined GBK charset
    inline int IsTextGbk(const std::string & txt)
    {
        return NS_IMPL::CString::IsTextGbk(txt);
    }

    inline bool IsTextGb2312(const std::string & txt)
    {
        const int ret = IsTextGbk(txt);
        return (1 == ret || 2 == ret);
    }

}//namespace tools

NS_SERVER_END

#endif

