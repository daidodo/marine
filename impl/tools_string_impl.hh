#ifndef DOZERG_TOOLS_STRING_IMPL_H_20130320
#define DOZERG_TOOLS_STRING_IMPL_H_20130320

#include <unistd.h>
#include <cctype>   //std::isspace,std::isalnum
#include <cassert>
#include <vector>
#include "../to_string.hh"
#include "tools_debug_impl.hh"

NS_IMPL_BEGIN

struct CString
{
    static std::string Trim(const std::string & str){
        size_t i = 0;
        for(;i < str.length() && std::isspace(str[i]);++i);
        size_t j = str.length();
        for(;j > i + 1 && std::isspace(str[j - 1]);--j);
        return (i < j ? str.substr(i,j - i) : std::string());
    }
    static ssize_t StringMatch(const char * src, size_t srclen, const char * target, size_t tarlen){
        assert(src && srclen && target && tarlen);
        for(size_t i = 0, j = 0, k = i;i < srclen;){
            if(src[i++] == target[j]){
                if(!j)
                    k = i;
                if(++j >= tarlen)
                    return k - 1;
            }else if(j){
                j = 0;
                i = k;
            }
        }
        return -1;
    }
    //使用kmp算法的字符串匹配
    static ssize_t StringMatchKmp(const char * src, size_t srclen, const char * target, size_t tarlen){
        assert(src && srclen && target && tarlen);
        //compute next array
        std::vector<ssize_t> next(tarlen, -1);
        size_t i = 0,j = tarlen - 1;
        for(ssize_t k = -1;i < j;){
            while(k != -1 && target[i] != target[k])
                k = next[k];
            ++i, ++k;
            next[i] = (target[i] == target[k] ? next[k] : k);
        }
        //kmp match
        i = j = 0;
        while(i < tarlen && j < srclen){
            if(target[i] == src[j]){
                ++i, ++j;
            }else if(next[i] != -1)
                i = next[i];
            else{
                i = 0;
                ++j;
            }
        }
        return (i >= tarlen ? ssize_t(j - i) : -1);
    }
    static std::string AbsFilename(const std::string & fname){
        std::string cwd = fname;
        if(fname.empty() || '/' != fname[0]){
            for(size_t sz = 1024;sz <= 4096;sz *= 2){
                cwd.resize(sz);
                if(NULL == ::getcwd(&cwd[0], cwd.size()))
                    continue;   //not enough room
                const size_t len = cwd.find_first_of('\0');
                assert(len < cwd.size());
                cwd.resize(len);
                if(*cwd.rbegin() != '/')
                    cwd.push_back('/');
                cwd += fname;
                break;
            }
        }
        return cwd;
    }
    static bool ExtractArg(const char * argstr, const char * pattern, const char ** result){
        if(!argstr || !pattern)
            return false;
        for(;*pattern;++pattern, ++argstr)
            if(*pattern != *argstr)
                return false;
        if(result){
            *result = argstr;
            return true;
        }
        return ('\0' == *argstr);
    }
    static std::string GetHost(const std::string & url){
        size_t from = url.find("//");
        if(std::string::npos == from)
            from = 0;
        else
            from += 2;
        size_t to = url.find('/', from);
        if(std::string::npos == to)
            to = url.size();
        to -= from;
        if(!to)
            return std::string();
        return url.substr(from, to);
    }
    static std::string UrlEncode(const std::string & url){
        const char DIGIT[] = "0123456789ABCDEF";
        CToString oss;
        for(std::string::const_iterator i = url.begin();i != url.end();++i){
            if(std::isalnum(*i)
                    || *i == '-'
                    || *i == '_'
                    || *i == '.'
                    || *i == '~')
                oss<<*i;
            else if(*i == ' ')
                oss<<'+';
            else
                oss<<'%'<<DIGIT[(*i >> 4) & 0xF]<<DIGIT[*i & 0xF];
        }
        return oss.str();
    }
    static std::string UrlDecode(const std::string & url){
        CToString oss;
        for(std::string::const_iterator i = url.begin();i != url.end();++i){
            if(*i == '%'){
                if(i + 2 < url.end()){
                    oss<<char((CDebug::UnHex(*(i + 1)) << 4) + CDebug::UnHex(*(i + 2)));
                    i += 2;
                }//else format error
            }else if(*i == '+')
                oss<<' ';
            else
                oss<<*i;
        }
        return oss.str();
    }
/*
    static std::string XmlEncode(const std::string & val){
        CToString oss;
        for(std::string::const_iterator i = val.begin();i != val.end();++i){
            switch(*i){
                case '<':oss<<"&lt;";break;
                case '>':oss<<"&gt;";break;
                case '&':oss<<"&amp;";break;
                case '\'':oss<<"&apos;";break;
                case '"':oss<<"&quot;";break;
                default:oss<<*i;
            }
        }
        return oss.str();
    }
*/
    static bool IsTextUtf8(const std::string & txt){
        const uint8_t utf8d[] = {
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 00..1f
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 20..3f
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 40..5f
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 60..7f
            1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9, // 80..9f
            7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7, // a0..bf
            8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, // c0..df
            0xa,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x4,0x3,0x3, // e0..ef
            0xb,0x6,0x6,0x6,0x5,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8, // f0..ff
            0x0,0x1,0x2,0x3,0x5,0x8,0x7,0x1,0x1,0x1,0x4,0x6,0x1,0x1,0x1,0x1, // s0..s0
            1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,1, // s1..s2
            1,2,1,1,1,1,1,2,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1, // s3..s4
            1,2,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,3,1,3,1,1,1,1,1,1, // s5..s6
            1,3,1,1,1,1,1,3,1,3,1,1,1,1,1,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // s7..s8
        };
        int state = 0;
        for(size_t i = 0;i < txt.size();++i){
            const int type = utf8d[uint8_t(txt[i])];
            state = utf8d[256 + state * 16 + type];
            if(1 == state)
                break;
        }
        return (0 == state);
    }

    static int IsTextGbk(const std::string & txt){
        int ret = 1;
        for(size_t i = 0;i < txt.size();++i){
            const unsigned char c = txt[i];
            if(c < 128)
                ret = std::max(ret, 1);
            if(++i < txt.size()){
                const unsigned char c2 = txt[i];
                if(0xA1 <= c && c <= 0xA9 && 0xA1 <= c2 && c2 <= 0xFE)          //GBK/1
                    ret = std::max(ret, 1);
                else if(0xB0 <= c && c <= 0xF7 && 0xA1 <= c2 && c2 <= 0xFE)     //GBK/2
                    ret = std::max(ret, 2);
                else if(0x81 <= c && c <= 0xA0 && 0x40 <= c2 && c2 <= 0xFE && 0x7F != c2)   //GBK/3
                    ret = std::max(ret, 3);
                else if(0xAA <= c && c <= 0xFE && 0x40 <= c2 && c2 <= 0xA0 && 0x7F != c2)   //GBK/4
                    ret = std::max(ret, 4);
                else if(0xA8 <= c && c <= 0xA9 && 0x40 <= c2 && c2 <= 0xA0 && 0x7F != c2)   //GBK/5
                    ret = std::max(ret, 5);
                else if((0xAA <= c && c <= 0xAF && 0xA1 <= c2 && c2 <= 0xFE)     //user-defined
                        || (0xF8 <= c && c <= 0xFE && 0xA1 <= c2 && c2 <= 0xFE)
                        || (0xA1 <= c && c <= 0xA7 && 0x40 <= c2 && c2 <= 0xA0 && 0x7F != c2))
                    ret = std::max(ret, 6);
                else
                    return 0;
            }else
                return 0;
        }
        return ret;
    }

};

NS_IMPL_END

#endif
