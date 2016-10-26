#ifndef DOZERG_TOOLS_STRING_H_20130122
#define DOZERG_TOOLS_STRING_H_20130122

/*
    �ַ���������غ���
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

#include <cstring>      //basename
#include "../impl/tools_string_impl.hh"

NS_SERVER_BEGIN

namespace tools
{
    //ȥ��strͷβ�Ŀհ׷�
    inline std::string Trim(const std::string & str)
    {
        return NS_IMPL::CString::Trim(str);
    }

    //��src����ƥ��target��
    //�ҵ�����ƥ�䴮����ʼλ��
    //δ�ҵ�����-1
    inline ssize_t StringMatch(const char * src, size_t srclen, const char * target, size_t tarlen)
    {
        return NS_IMPL::CString::StringMatch(src, srclen, target, tarlen);
    }

    //��src����ƥ��target����ʹ��KMP�㷨
    //�ҵ�����ƥ�䴮����ʼλ��
    //δ�ҵ�����-1
    inline ssize_t StringMatchKmp(const char * src, size_t srclen, const char * target, size_t tarlen)
    {
        return NS_IMPL::CString::StringMatchKmp(src, srclen, target, tarlen);
    }

    //���fname���Ǿ���·������ȫ�ɾ���·���󷵻�
    inline std::string AbsFilename(const std::string & fname)
    {
        return NS_IMPL::CString::AbsFilename(fname);
    }

    //ȥ��fname���Ŀ¼���֣�ֻ���ļ���
    inline std::string Basename(const std::string & fname)
    {
        return fname.substr(fname.find_last_of('/') + 1);
    }

    inline const char * Basename(const char * fname)
    {
        if(!fname)
            return NULL;
        return ::basename(fname);
    }

    //��ȡ�ļ�������ȥ����׺����
    inline std::string FilePrimeName(const std::string & fname)
    {
        std::string bname = Basename(fname);
        const size_t pos = bname.find_last_of('.');
        return bname.substr(0, pos);
    }

    //��ȡ�ļ������ͣ���׺����
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

    //����������ѡ��
    //argstr: �������ַ���
    //pattern: ��Ҫƥ����ַ���
    //result: argstrȥ��pattern��ʣ�µĲ������֣�����ΪNULL
    //return:
    //  ���(result==NULL)���򷵻�(0==strcmp(argstr, pattern))
    //  ���򷵻�argstr�Ƿ�ƥ�䵽pattern
    //����:
    //argstr = "-file=example.txt"
    //��ƥ�� pattern = "-file=" �� result = NULL ʱ������false
    //��ƥ�� pattern = "-file=" �� result != NULL ʱ������true,�ҵõ� *result = "example.txt"
    //��ƥ�� pattern = "-logfile="ʱ,����false
    inline bool ExtractArg(const char * argstr, const char * pattern, const char ** result = NULL)
    {
        return NS_IMPL::CString::ExtractArg(argstr, pattern, result);
    }

    //��url��ȡ��host
    inline std::string GetHost(const std::string & url)
    {
        return NS_IMPL::CString::GetHost(url);
    }

    //urlת�봦��
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

