#ifndef DOZERG_CHARSET_CONVERT_H_20130513
#define DOZERG_CHARSET_CONVERT_H_20130513

/*
    �ַ�����ת��
        CCharsetConvert     ֧�����������ת��
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
    //��ʼ��
    //fromCode: ��Դ���ݵı����������iconv --list
    //toCode: Ŀ�����ݵı����������iconv --list
    //mode: ת��ģʽ
    //  kNormal     ��ͨģʽ������Ŀ����벻֧�ֵ��ַ����򷵻�ʧ��
    //  kTranslit   ʹ��ֱ��ģʽ������Ŀ����벻֧�ֵ��ַ���ʹ�ý��Ƶ��ַ�����
    //  kIgnore     ʹ�ú���ģʽ������Ŀ����벻֧�ֵ��ַ�������˵�
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
    //�Ƿ��ʼ��
    bool valid() const{return (iconv_t(-1) != cv_);}
    //��ȡ��Դ������
    const std::string fromCode() const{return from_;}
    //��ȡĿ�������
    const std::string toCode() const{return to_;}
    //��ȡת��ģʽ
    EMode mode() const{return mode_;}
    //ת��
    //sourc: ��Դ��������
    //dest: Ŀ��������ݣ������ݽ�׷�ӵ�����
    //return:
    //  true    ת��ɹ�
    //  false   ת��ʧ��
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
    //ת���ɿɶ��ַ���
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

