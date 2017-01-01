#ifndef DOZERG_TOOLS_DEBUG_IMPL_H_20130320
#define DOZERG_TOOLS_DEBUG_IMPL_H_20130320

#include <stdint.h>
#include <iomanip>      //std::setw
#include <string>
#include <algorithm>    //std::reverse
#include "../to_string.hh"

NS_IMPL_BEGIN

struct CDebug
{
    template<typename CharT>
    static std::string DumpChar(CharT ch){
        const char DIGIT[] = "0123456789ABCDEF";
        std::string ret;
        for(size_t i = 0;ch != 0 && i < sizeof ch * 2;++i, ch >>= 4)
            ret.push_back(DIGIT[ch & 0xF]);
        ret.resize(sizeof ch * 2, '0');
        std::reverse(ret.begin(), ret.end());
        return ret;
    }
    template<typename CharT>
    static std::string DumpHex(const CharT * v, size_t sz, char sep, bool hasLen){
        if(!v)
            return std::string("(NULL)");
        CToString oss;
        if(hasLen)
            oss<<"("<<sz<<")";
        for(size_t i = 0;i < sz;oss<<DumpChar(v[i++]))
            if(i && sep)
                oss<<sep;
        return oss.str();
    }
    static std::string DumpStr(const char * v, size_t sz, char replace, bool hasLen){
        const char TRAN_CHAR = '\\';
        const char FOLLOW_CHAR[] = "abtnvfr";
        if(!v)
            return std::string("(NULL)");
        if(!std::isprint(replace))
            replace = '.';
        std::string ret;
        if(hasLen){
            CToString oss;
            oss<<"("<<sz<<")";
            ret = oss.str();
        }
        ret.reserve(ret.size() + sz + (sz >> 2));
        for(;sz > 0;--sz,++v){
            if(*v == TRAN_CHAR){
                ret.push_back(TRAN_CHAR);
                ret.push_back(TRAN_CHAR);
            }else if(*v >= '\a' && *v <= '\r'){
                ret.push_back(TRAN_CHAR);
                ret.push_back(FOLLOW_CHAR[*v - '\a']);
            }else if(!*v){
                ret.push_back(TRAN_CHAR);
                ret.push_back('0');
            }else
                ret.push_back(std::isprint(*v) ? *v : replace);
        }
        return ret;
    }
    static std::string DumpVal(const char * v, size_t sz, int base, bool hasLen){
        const char TRAN_CHAR = '\\';
        const char FOLLOW_CHAR[] = "abtnvfr";
        const char DIGIT[] = "0123456789ABCDEF";
        if(!v)
            return std::string("(NULL)");
        std::string ret;
        if(hasLen){
            CToString oss;
            oss<<"("<<sz<<")";
            ret = oss.str();
        }
        ret.reserve(ret.length() + 2 * sz);
        for(;sz > 0;--sz,++v){
            if(*v == TRAN_CHAR){
                ret.push_back(TRAN_CHAR);
                ret.push_back(TRAN_CHAR);
            }else if(std::isprint(*v))
                ret.push_back(*v);
            else{
                ret.push_back(TRAN_CHAR);
                if(*v >= '\a' && *v <= '\r')
                    ret.push_back(FOLLOW_CHAR[*v - '\a']);
                else if(!*v)
                    ret.push_back('0');
                else{
                    switch(base){
                        case 16:       //16进制
                            ret.push_back('x');
                            ret.push_back(DIGIT[(*v >> 4) & 0xF]);
                            ret.push_back(DIGIT[*v & 0xF]);
                            break;
                        default:       //8进制
                            ret.push_back(DIGIT[(*v >> 6) & 3]);
                            ret.push_back(DIGIT[(*v >> 3) & 7]);
                            ret.push_back(DIGIT[*v & 7]);
                    }
                }
            }
        }
        return ret;
    }
    static std::string DumpXxd(const char * v, size_t sz, bool hasLen){
        if(!v)
            return "NULL";
        if(!sz)
            return (hasLen ? "(0)" : "");
        const size_t LINE_WIDTH = 4;
        const size_t CHARS_PER_LINE = 16;
        size_t lines = (sz + CHARS_PER_LINE - 1) / CHARS_PER_LINE;
        size_t lw = 0;
        for(;lines > 0;lines >>= 8, lw += 2);
        if(lw < LINE_WIDTH)
            lw = LINE_WIDTH;
        CToString oss;
        if(hasLen)
            oss<<'('<<sz<<")\n";
        oss<<Manip::fill('0')<<Manip::hex<<Manip::noshowbase;
        for(size_t ln = 0;ln < sz;ln += CHARS_PER_LINE){
            oss<<Manip::setw(lw)<<ln<<"h: ";
            const size_t left = std::min(CHARS_PER_LINE, sz - ln);
            oss<<DumpHex(v + ln, left, ' ', false);
            for(size_t i = left;i < CHARS_PER_LINE;++i)
                oss<<"   ";
            oss<<"; ";
            for(size_t i = 0;i < left;++i)
                oss<<((v[ln + i] > 31 && v[ln + i] < 127) ? v[ln + i] : '.');
            oss<<'\n';
        }
        return oss.str();
    }
    //预读取前PRE_READ个字符，统计可读字符个数，然后选择合适的转换函数
    static std::string Dump(const char * v, size_t sz){
        const size_t PRE_READ = 32;
        if(sz > 64)
            return DumpXxd(v, sz, true);
        size_t readable = 0;
        size_t check_len = std::min(sz, PRE_READ);
        for(const char *t = v, *e = v + check_len;t < e;++t)
            if(*t && std::isprint(*t))
                ++readable;
        if(readable <= (check_len >> 1))
            return DumpHex(v, sz, ' ', true);
        else if(readable < check_len)
            return DumpVal(v, sz, 16, true);
        return DumpStr(v, sz, '.', true);
    }
    static std::string ToStringBits(uint32_t v, const char * const * name, size_t name_len){
        CToString oss;
        oss<<v;
        if(name && name_len){
            bool zero = true, first = true;
            for(size_t i = 0;i < name_len && i < 32;++i){
                const uint32_t mask = (1 << i);
                if(0 == (v & mask))
                    continue;
                if(zero){
                    oss<<"(";
                    zero = false;
                }
                if(name[i]){
                    if(!first)
                        oss<<" | ";
                    oss<<name[i];
                    v &= ~mask;
                    first = false;
                }
            }
            if(!zero){
                if(v){
                    if(!first)
                        oss<<" | ";
                    oss<<Manip::hex<<v;
                }
                oss<<")";
            }
        }
        return oss.str();
    }
    static int UnHex(char a){
        if(a >= '0' && a <= '9')
            return a - '0';
        else if(a >= 'a' && a <= 'f')
            return a - 'a' + 0xa;
        else if(a >= 'A' && a <= 'F')
            return a - 'A' + 0xA;
        return -1;
    }
    static std::string UnHex(const char * v, size_t sz){
        typedef const char * __Ptr;
        if(!v)
            return std::string("(NULL)");
        std::string ret;
        ret.reserve(sz >> 1);
        int r = -1;
        for(__Ptr i = v, e = v + sz;i < e;++i){
            int t = UnHex(*i);
            if(r < 0){
                if(t >= 0)
                    r = t;
            }else{
                if(t >= 0)
                    r = (r << 4) + t;
                ret.push_back(r);
                r = -1;
            }
        }
        if(r >= 0)
            ret.push_back(r);
        return ret;
    }
};

NS_IMPL_END

#endif
