#ifndef DOZERG_TO_STRING_H_20130411
#define DOZERG_TO_STRING_H_20130411

/*
    ����������ת���ɿɶ��ַ�����std::ostringstream���Ʒ
    ����:
        CToString       std::ostringstream���Ʒ
    ������:
        boolalpha       ��true/falseת����"true"/"false"��Ĭ��
        noboolalpha     ��true/falseת����"1"/"0"
        setw            ��������ַ�������������һ�������Ч
        fill            ��������ַ���Ĭ��' '
        left            ������ݿ�����룬����ַ����ұ�
        right           ������ݿ��Ҷ��룬����ַ�����ߣ�Ĭ��
        setbase         �����������ƣ�Ĭ��10
        dec             ������������Ϊ10
        hex             ������������Ϊ16
        oct             ������������Ϊ8
        bin             ������������Ϊ2
        showbase        ����������Ƶı�ʶ�����磺0x,0X(16����), 0(8����), 0b,0B(2����)��Ĭ��
        noshowbase      ������������Ƶı�ʶ
        uppercase       ��������ַ����ô�д
        nouppercase     ��������ַ�����Сд
        autocase        �����ַ���Сд����ֵ�ַ��ô�д�����磺"0xABC", "0b1001"��Ĭ��
        showpos         10����������ǰ��'+'
        noshowpos       10����������ǰ��'+'��Ĭ��
        endl            ������з�'\n'
        ends            ���������'\0'
//*/

#include <stdint.h>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <string>
#include <limits>       //std::numeric_limits
#include <algorithm>    //std::fill_n
#include "template.hh"  //CTypeTraits
#include "impl/to_string_impl.hh"

NS_SERVER_BEGIN

class CToString
{
    typedef CToString __Myt;
    struct __Flags{
        char fill_;
        uint8_t width_;
        uint8_t base_:2;       //print number at base of 10(0) or 16(1) or 8(2) or 2(3)
        uint8_t nobase_:1;     //print base(0, 0x, 0X, 0b, 0B)(1) or not(0)
        uint8_t autocase_:2;   //print number with autocase(0), uppercase(1) or lowercase(2)
        uint8_t adjustleft_:1; //adjust to left(1) or right(0)
        uint8_t boolnum_:1;    //print bool as number
        uint8_t showpos_:1;    //print non-negative numerical values preceded by a plus sign(+)
        __Flags():fill_(' '),width_(0),base_(0),nobase_(0),autocase_(0),adjustleft_(0),boolnum_(0),showpos_(0){}
    };
    //number base
    static const uint8_t kDec = 0;
    static const uint8_t kHex = 1;
    static const uint8_t kOct = 2;
    static const uint8_t kBin = 3;
public:
    //char case
    static const uint8_t kAutoCase = 0;
    static const uint8_t kUpperCase = 1;
    static const uint8_t kLowerCase = 2;
    //����/��ȡ�Ƿ�boolֵ��ʾ���ַ���
    void boolalpha(bool c){flags_.boolnum_ = (c ? 0 : 1);}
    bool boolalpha() const{return (0 == flags_.boolnum_);}
    //����/��ȡ�´ε�����ַ���(0-255)
    //ע�⣺������һ�������Ч
    void width(int n){flags_.width_ = (n >= 0 ? (n <= 255 ? n : 255) : 0);}
    int width() const{return flags_.width_;}
    //����/��ȡ�Ҷ���(true)���������(false)
    void adjustright(bool v){flags_.adjustleft_ = (v ? 0 : 1);}
    bool adjustright() const{return (0 == flags_.adjustleft_);}
    //����/��ȡ����ַ�
    void fill(char c){flags_.fill_ = c;}
    char fill() const{return flags_.fill_;}
    //����/��ȡ�����Ľ���
    //base:
    //  10      ��10�����������
    //  16      ��16�����������
    //  8       ��8�����������
    //  2       ��2�����������
    //  ����    ��10�����������
    void base(int base){
        switch(base){
            case 16:flags_.base_ = kHex;break;
            case 8:flags_.base_ = kOct;break;
            case 2:flags_.base_ = kBin;break;
            default:flags_.base_ = kDec;
        }
    }
    int base() const{return (kDec == fbase() ? 10 : (kHex == fbase() ? 16 : (kOct == fbase() ? 8 : 2)));}
    //����/��ȡ�Ƿ���ʾ����(0x, 0X, 0, 0b, 0B)
    void showbase(bool v){flags_.nobase_ = (v ? 0 : 1);}
    bool showbase() const{return (0 == flags_.nobase_);}
    //����/��ȡ�ַ���Сд��ʽ
    //return:
    //  0       autocase
    //  1       uppercase
    //  2       lowercase
    //  ����    δ֪
    void charcase(int v){flags_.autocase_ = (v & 3);}
    int charcase() const{return (flags_.autocase_ & 3);}
    //����/��ȡ������ǰ���Ƿ��'+'
    void showpos(bool c){flags_.showpos_ = (c ? 1 : 0);}
    bool showpos() const{return (0 != flags_.showpos_);}
    //print basic types
    __Myt & operator <<(bool c){
        if(boolalpha())
            return (c ? printStr("true", 4) : printStr("false", 5));
        return printChar(c ? '1' : '0');
    }
    __Myt & operator <<(char c){return printChar(c);}
    __Myt & operator <<(signed char c){return printChar(c);}
    __Myt & operator <<(unsigned char c){return printChar(c);}
    __Myt & operator <<(short c){return printInt(c);}
    __Myt & operator <<(unsigned short c){return printInt(c);}
    __Myt & operator <<(int c){return printInt(c);}
    __Myt & operator <<(unsigned int c){return printInt(c);}
    __Myt & operator <<(long c){return printInt(c);}
    __Myt & operator <<(unsigned long c){return printInt(c);}
    __Myt & operator <<(long long c){return printInt(c);}
    __Myt & operator <<(unsigned long long c){return printInt(c);}
    __Myt & operator <<(float c);   //TODO: precision, max length, scientific ...
    __Myt & operator <<(double c);  //TODO
    __Myt & operator <<(const char * c){return printStr(c);}
    __Myt & operator <<(char * c){return operator <<(static_cast<const char *>(c));}
    __Myt & operator <<(const std::string & c){
        if(c.empty())
            return *this;
        return printStr(c.c_str(), c.length());
    }
    __Myt & operator <<(const void * c){return printInt(reinterpret_cast<long>(c), true, kHex, kLowerCase);}
    __Myt & operator <<(void * c){return operator <<(static_cast<const void *>(c));}
    //manipulators
    __Myt & operator <<(const NS_IMPL::CToStringBase & c){
        base(c.base_);
        return *this;
    }
    __Myt & operator <<(const NS_IMPL::CToStringWidth & c){
        width(c.w_);
        return *this;
    }
    __Myt & operator <<(const NS_IMPL::CToStringFill & c){
        fill(c.fill_);
        return *this;
    }
    __Myt & operator <<(__Myt & (*func)(__Myt &)){
        if(func)
            func(*this);
        return *this;
    }
    //����/��ȡ����ַ���
    void str(const std::string & str){buf_ = str;}
    const std::string & str() const{return buf_;}
private:
    uint8_t fbase() const{return flags_.base_;};
    static bool baseUpper(int ccase){return (kUpperCase == ccase);}
    static bool valueUpper(int ccase){return (kLowerCase != ccase);}
    template<typename Int>
    bool realShowPos(Int c) const{
        typedef std::numeric_limits<Int> __Limits;
        return (__Limits::is_signed
#ifdef __0_NO_POSITIVE
                && 0 != c
#endif
                && showpos());
    }
    template<typename Int>
    __Myt & printInt(Int c){return printInt(c, showbase(), fbase(), charcase());}
    template<typename Int>
    __Myt & printInt(Int c, bool showbase, uint8_t base, int ccase){
        char tmp[72];   //max for all bases(2, 8, 10, 16)
        char * e = tmp + sizeof tmp;
        char * s = NULL;
        if(kDec == base)
            s = int2Str10(e, c);
        else
            s = int2Str(e, c, base, showbase, ccase);
        setWidth(s, e - s);
        return *this;
    }
    __Myt & printChar(char c){
        setWidth(&c, 1);
        return *this;
    }
    __Myt & printStr(const char * c, size_t len = 0){
        if(NULL == c)
            return printStr("(NULL)", 6);
        if(!len)
            len = ::strlen(c);
        if(len)
            setWidth(c, len);
        return *this;
    }
    template<typename Int>
    char * int2Str10(char * e, Int c){
        typedef typename CTypeTraits<Int>::__Unsigned __Unsigned;
        assert(e);
        const char * const kDig99 =
            "00010203040506070809"
            "10111213141516171819"
            "20212223242526272829"
            "30313233343536373839"
            "40414243444546474849"
            "50515253545556575859"
            "60616263646566676869"
            "70717273747576777879"
            "80818283848586878889"
            "90919293949596979899";
        __Unsigned cc = c;
        const bool negtive = !(c > 0 || c == 0);
        if(negtive)
            cc = -cc;
        for(__Unsigned i;cc > 99;){
            i = cc;
            cc /= 100;
            memcpy((e -= 2), kDig99 + ((i << 1) - cc * 200), 2);
        }
        if(cc < 10){
            *--e = '0' + cc;
        }else
            memcpy((e -= 2), kDig99 + cc * 2, 2);
        if(negtive)
            *--e = '-';
        else if(realShowPos(c))
            *--e = '+';
        return e;
    }
    template<typename Int>
    char * int2Str(char * e, Int c, uint8_t base, bool showbase, int ccase){
        typedef typename CTypeTraits<Int>::__Unsigned __Unsigned;
        assert(e);
        const bool sb = (showbase && 0 != c);   //0 need not showbase
        __Unsigned cc = c;
        size_t i = 0;
        switch(base){
            case kHex:
                i = (valueUpper(ccase) ? 0 : 16);
                do{
                    *--e = "0123456789ABCDEF0123456789abcdef"[(cc & 0xF) + i];
                    cc >>= 4;
                }while(cc);
                if(sb){
                    *--e = (baseUpper(ccase) ? 'X' : 'x');
                    *--e = '0';
                }
                break;
            case kOct:
                do{
                    *--e = (cc & 7) + '0';
                    cc >>= 3;
                }while(cc);
                if(sb)
                    *--e = '0';
                break;
            case kBin:
                do{
                    *--e = (cc & 1) + '0';
                    cc >>= 1;
                }while(cc);
                if(sb){
                    *--e = (baseUpper(ccase) ? 'B' : 'b');
                    *--e = '0';
                }
                break;
            default:assert(0);
        }
        return e;
    }
    void setWidth(const char * s, size_t len){
        assert(s && len);
        size_t w = width();
        assert(w <= 255);
        const bool r = adjustright();
        if(!r)
            buf_.append(s, len);
        if(len < w)
            buf_.append(w - len, fill());
        if(r)
            buf_.append(s, len);
        width(0);
    }
    //fields
    std::string buf_;
    __Flags flags_;
};

namespace Manip{

    inline CToString & boolalpha(CToString & tos)
    {
        tos.boolalpha(true);
        return tos;
    }

    inline CToString & noboolalpha(CToString & tos)
    {
        tos.boolalpha(false);
        return tos;
    }

    inline NS_IMPL::CToStringWidth setw(int w)
    {
        return NS_IMPL::CToStringWidth(w);
    }

    inline NS_IMPL::CToStringFill fill(char c)
    {
        return NS_IMPL::CToStringFill(c);
    }

    inline CToString & left(CToString & tos)
    {
        tos.adjustright(false);
        return tos;
    }

    inline CToString & right(CToString & tos)
    {
        tos.adjustright(true);
        return tos;
    }

    inline NS_IMPL::CToStringBase setbase(int base)
    {
        return NS_IMPL::CToStringBase(base);
    }

    inline CToString & dec(CToString & tos)
    {
        return (tos<<setbase(10));
    }

    inline CToString & hex(CToString & tos)
    {
        return (tos<<setbase(16));
    }

    inline CToString & oct(CToString & tos)
    {
        return (tos<<setbase(8));
    }

    inline CToString & bin(CToString & tos)
    {
        return (tos<<setbase(2));
    }

    inline CToString & showbase(CToString & tos)
    {
        tos.showbase(true);
        return tos;
    }

    inline CToString & noshowbase(CToString & tos)
    {
        tos.showbase(false);
        return tos;
    }

    inline CToString & autocase(CToString & tos)
    {
        tos.charcase(CToString::kAutoCase);
        return tos;
    }

    inline CToString & uppercase(CToString & tos)
    {
        tos.charcase(CToString::kUpperCase);
        return tos;
    }

    inline CToString & nouppercase(CToString & tos)
    {
        tos.charcase(CToString::kLowerCase);
        return tos;
    }

    inline CToString & showpos(CToString & tos)
    {
        tos.showpos(true);
        return tos;
    }

    inline CToString & noshowpos(CToString & tos)
    {
        tos.showpos(false);
        return tos;
    }

    inline CToString & endl(CToString & tos)
    {
        return (tos<<'\n');
    }

    inline CToString & ends(CToString & tos)
    {
        return (tos<<'\0');
    }

}//namespace Manip

NS_SERVER_END

#endif

