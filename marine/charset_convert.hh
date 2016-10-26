#ifndef DOZERG_CHARSET_CONVERT_H_20130513
#define DOZERG_CHARSET_CONVERT_H_20130513

/*
    字符编码转换
        CCharsetConvert     支持任意编码间的转换
//*/

#include <iconv.h>
#include <cassert>
#include "to_string.hh"

NS_SERVER_BEGIN

class CCharsetConvert
{
public:
    enum EMode{
        kNormal = 0,
        kTranslit,
        kIgnore
    };
    CCharsetConvert()
        : cv_(iconv_t(-1))
        , mode_(kNormal)
    {}
    ~CCharsetConvert(){
        if(valid())
            ::iconv_close(cv_);
    }
    //初始化
    //fromCode: 来源数据的编码名，详见iconv --list
    //toCode: 目标数据的编码名，详见iconv --list
    //mode: 转码模式
    //  kNormal     普通模式，遇到目标编码不支持的字符，则返回失败
    //  kTranslit   使用直译模式，遇到目标编码不支持的字符，使用近似的字符代替
    //  kIgnore     使用忽略模式，遇到目标编码不支持的字符，则过滤掉
    CCharsetConvert(const std::string & fromCode, const std::string & toCode, EMode mode = kNormal)
        : cv_(iconv_t(-1))
        , mode_(kNormal)
    {
        init(fromCode, toCode, mode);
    }
    bool init(const std::string & fromCode, const std::string & toCode, EMode mode = kNormal){
        if(valid())
            return false;
        if(fromCode.empty() || toCode.empty())
            return false;
        std::string t = toCode;
        switch(mode){
            case kTranslit:t += "//TRANSLIT";break;
            case kIgnore:t += "//IGNORE";break;
            default:;
        }
        from_ = fromCode;
        to_ = toCode;
        mode_ = mode;
        cv_ = ::iconv_open(t.c_str(), fromCode.c_str());
        return valid();
    }
    //是否初始化
    bool valid() const{return (iconv_t(-1) != cv_);}
    //获取来源编码名
    const std::string fromCode() const{return from_;}
    //获取目标编码名
    const std::string toCode() const{return to_;}
    //获取转码模式
    EMode mode() const{return mode_;}
    //转码
    //sourc: 来源编码数据
    //dest: 目标编码数据，新数据将追加到后面
    //return:
    //  true    转码成功
    //  false   转码失败
    bool convert(const std::string & source, std::string & dest){
        if(!valid())
            return false;
        if(source.empty())
            return true;
        ::iconv(cv_, NULL, NULL, NULL, NULL);    //reset init state;
        char * inbuf = const_cast<char *>(&source[0]);  //iconv()'s bug
        size_t inLen = source.size();
        for(char buf[64];inLen > 0;){
            const size_t inOld = inLen;
            char * outbuf = buf;
            size_t outLen = sizeof buf;
            ::iconv(cv_, &inbuf, &inLen, &outbuf, &outLen);
            if(inOld <= inLen)   //cannot proceed
                return false;
            if(buf != outbuf){
                assert(buf < outbuf);
                dest.append(buf, outbuf - buf);
            }
        }
        return true;
    }
    //转化成可读字符串
    std::string toString() const{
        CToString oss;
        oss<<"{cv_="<<cv_
            <<", from_="<<from_
            <<", to_="<<to_
            <<", mode_="<<mode_
            <<'}';
        return oss.str();
    }
private:
    CCharsetConvert(const CCharsetConvert &);   //disable copy and assignment
    CCharsetConvert & operator =(const CCharsetConvert &);
    //members
    std::string from_, to_;
    iconv_t cv_;
    EMode mode_;
};

NS_SERVER_END

#endif

