#include <marine/to_string.hh>

#include "../inc.h"
#include <sstream>

const int C = 50;

template<typename Int>
static std::string tobin(Int v)
{
    const int shift = 8 * sizeof v - 1;
    Int mask = 1;
    mask = (mask << shift) - 1;
    char e[65] = {0};
    char * s = e + sizeof e - 1;
    do{
        *--s = (v & 1) + '0';
        v = (v >> 1) & mask;
    }while(v);
    return std::string(s);
}

/*
   TS
showcase    0   0   0   1       1       1
charcase    0   1   2   0       1       2
   OSS(16)
showcase    0   0   0   0(0x)   1       1
uppercase   1   1   0   1       1       0
   OSS(2)
showcase    0   0   0   0(0b)   0(0B)   0(0b)
uppercase   0   0   0   0       0       0
*/

struct CFlags
{
    bool bin;
    const char * base;
    CFlags():bin(false),base(NULL){}
};

static void setflags(const CToString & ts, std::ostringstream & oss, std::ostringstream & tmp, CFlags & f)
{
    //set base
    switch(ts.base()){
        case 2:f.bin = true;break;
        case 8:oss<<std::oct;tmp<<std::oct;break;
        case 16:oss<<std::hex;tmp<<std::hex;break;
        default:;
    }
    //set case
    const char * kCase[] = {"", "0x", "0b", "0B"};
    int icase = 0;
    if(CToString::kLowerCase != ts.charcase()){
        oss<<std::uppercase;
        tmp<<std::uppercase;
    }
    if(ts.showbase()){
        switch(ts.base()){
            case 2:
                icase = (CToString::kUpperCase == ts.charcase() ? 3 : 2);
                break;
            case 16:
                if(CToString::kAutoCase == ts.charcase())
                    icase = 1;
                break;
            default:;
        }
        if(!icase){
            oss<<std::showbase;
            tmp<<std::showbase;
        }
    }
    f.base = kCase[icase];
    //showpos
    if(ts.showpos()){
        oss<<std::showpos;
        tmp<<std::showpos;
    }
}

#define __OutV  \
    if(sizeof v > 1 && v)   \
        tmp<<f.base;    \
    if(sizeof v > 1 && f.bin) \
        tmp<<tobin(v);  \
    else    \
        tmp<<v; \
    oss<<tmp.str(); \
    tmp.str(std::string())

template<typename Int>
static void out(CToString & ts, std::ostringstream & oss, int w)
{
    typedef Int __Int;
    typedef std::numeric_limits<__Int> __Limits;
    std::ostringstream tmp;
    CFlags f;
    setflags(ts, oss, tmp, f);
    //test
    __Int v = __Limits::min();
    for(int i = 0;i < C;++i, ++v){
        if((i & 1) && w){
            ts<<Manip::setw(w)<<v;
            oss<<setw(w);
        }else
            ts<<v;
        __OutV;
    }
    v = __Limits::min();
    for(int i = 0;i < C;++i, --v){
        if((i & 1) && w){
            ts<<Manip::setw(w)<<v;
            oss<<setw(w);
        }else
            ts<<v;
        __OutV;
    }
    v = __Limits::max();
    for(int i = 0;i < C;++i, ++v){
        if((i & 1) && w){
            ts.width(w);
            oss.width(w);
        }
        ts<<v;
        __OutV;
    }
    v = __Limits::max();
    for(int i = 0;i < C;++i, --v){
        if((i & 1) && w){
            ts.width(w);
            oss.width(w);
        }
        ts<<v;
        __OutV;
    }
    v = 0;
    for(int i = 0;i < C;++i, ++v){
        if(w){
            ts<<Manip::setw(w);
            oss<<setw(w);
        }
        ts<<v;
        __OutV;
    }
    v = 0;
    for(int i = 0;i < C;++i, --v){
        if(w){
            ts.width(w);
            oss.width(w);
        }
        ts<<v;
        __OutV;
    }
}

class CTest{int x;};

#define __TEST  if(oss.str() != ts.str())return

static void realtest(CToString & ts, std::ostringstream & oss, int w = 0)
{
    //set flags
    if(ts.boolalpha())
        oss<<std::boolalpha;
    if(!ts.adjustright())
        oss<<std::left;
    if(' ' != ts.fill())
        oss.fill(ts.fill());
    //bool
    if(w){
        ts<<Manip::setw(w);
        oss<<std::setw(w);
    }
    ts<<true<<false;
    oss<<true<<false;
    __TEST;
    //pointer
    CTest t;
    if(w){
        ts<<Manip::setw(w);
        oss<<std::setw(w);
    }
    ts<<&t<<&t;
    oss<<&t<<&t;
    __TEST;
    //char
    out<char>(ts, oss, w);
    __TEST;
    out<signed char>(ts, oss, w);
    __TEST;
    out<unsigned char>(ts, oss, w);
    __TEST;
    //short
    out<short>(ts, oss, w);
    __TEST;
    out<unsigned short>(ts, oss, w);
    __TEST;
    //int
    out<int>(ts, oss, w);
    __TEST;
    out<unsigned int>(ts, oss, w);
    __TEST;
    //long
    out<long>(ts, oss, w);
    __TEST;
    out<unsigned long>(ts, oss, w);
    __TEST;
    //long long
    out<long long>(ts, oss, w);
    __TEST;
    out<unsigned long long>(ts, oss, w);
    __TEST;
    //wchar_t
    out<wchar_t>(ts, oss, w);
    __TEST;
    out<wchar_t>(ts, oss, w);
    __TEST;
    //string
    std::string str = "a;waeg";
    if(w){
        ts<<Manip::setw(w);
        oss<<std::setw(w);
    }
    ts<<str<<str;
    oss<<str<<str;
    __TEST;
    //const string
    const std::string & cstr = str;
    if(w){
        ts<<Manip::setw(w);
        oss<<std::setw(w);
    }
    ts<<cstr<<cstr;
    oss<<cstr<<cstr;
    __TEST;
    //char *
    char * s = &str[0];
    if(w){
        ts<<Manip::setw(w);
        oss<<std::setw(w);
    }
    ts<<s<<s;
    oss<<s<<s;
    __TEST;
    //const char *
    const char * cs = &str[0];
    if(w){
        ts<<Manip::setw(w);
        oss<<std::setw(w);
    }
    ts<<cs<<cs;
    oss<<cs<<cs;
    __TEST;
    //char []
    char as[] = "b0-n3qnbo";
    if(w){
        ts<<Manip::setw(w);
        oss<<std::setw(w);
    }
    ts<<as<<as;
    oss<<as<<as;
    __TEST;
}

TEST(CToString, boolalpha)
{
    {
        CToString ts;
        std::ostringstream oss;
        ASSERT_EQ(oss.str(), ts.str());

        ASSERT_EQ(true, ts.boolalpha());
        realtest(ts, oss);
        ASSERT_EQ(oss.str(), ts.str());
    }{
        CToString ts;
        std::ostringstream oss;
        ASSERT_EQ(oss.str(), ts.str());

        ts.boolalpha(false);
        ASSERT_FALSE(ts.boolalpha());
        realtest(ts, oss);
        ASSERT_EQ(oss.str(), ts.str());
    }{
        CToString ts;
        std::ostringstream oss;
        ASSERT_EQ(oss.str(), ts.str());

        oss<<9999;
        ts<<Manip::noboolalpha<<9999;
        ASSERT_FALSE(ts.boolalpha());
        realtest(ts, oss);
        ASSERT_EQ(oss.str(), ts.str());

        oss<<9999;
        ts<<Manip::boolalpha<<9999;
        ASSERT_TRUE(ts.boolalpha());
        realtest(ts, oss);
        ASSERT_EQ(oss.str(), ts.str());
    }
}

TEST(CToString, width)
{
    {
        CToString ts;
        std::ostringstream oss;
        ASSERT_EQ(oss.str(), ts.str());
        ASSERT_EQ(0, ts.width());

        ts.width(15);
        ASSERT_EQ(15, ts.width());
        ts.width(-1);
        ASSERT_EQ(0, ts.width());
        ts.width(256);
        ASSERT_EQ(255, ts.width());
        ASSERT_EQ(oss.str(), ts.str());
    }{
        CToString ts;
        std::ostringstream oss;
        ASSERT_EQ(oss.str(), ts.str());

        realtest(ts, oss, 15);
        ASSERT_EQ(oss.str(), ts.str());
    }{
        CToString ts;
        std::ostringstream oss;
        ASSERT_EQ(oss.str(), ts.str());

        realtest(ts, oss, 255);
        ASSERT_EQ(oss.str(), ts.str());
    }{
        CToString ts;
        std::ostringstream oss;
        ASSERT_EQ(oss.str(), ts.str());

        realtest(ts, oss, 0);
        ASSERT_EQ(oss.str(), ts.str());
    }
}

TEST(CToString, adjust)
{
    {
        const int w = 15;
        {
            CToString ts;
            std::ostringstream oss;
            ASSERT_EQ(oss.str(), ts.str());

            ASSERT_TRUE(ts.adjustright());
            realtest(ts, oss, w);
            ASSERT_EQ(oss.str(), ts.str());
        }{
            CToString ts;
            std::ostringstream oss;
            ASSERT_EQ(oss.str(), ts.str());

            ts.adjustright(false);
            ASSERT_FALSE(ts.adjustright());
            realtest(ts, oss, w);
            ASSERT_EQ(oss.str(), ts.str());
        }{
            CToString ts;
            std::ostringstream oss;
            ASSERT_EQ(oss.str(), ts.str());

            oss<<99999;
            ts<<Manip::right<<99999;
            ASSERT_TRUE(ts.adjustright());
            realtest(ts, oss, w);
            ASSERT_EQ(oss.str(), ts.str());
        }{
            CToString ts;
            std::ostringstream oss;
            ASSERT_EQ(oss.str(), ts.str());

            oss<<99999;
            ts<<Manip::left<<99999;
            ASSERT_FALSE(ts.adjustright());
            realtest(ts, oss, w);
            ASSERT_EQ(oss.str(), ts.str());
        }
    }{
        const int w = 0;
        {
            CToString ts;
            std::ostringstream oss;
            ASSERT_EQ(oss.str(), ts.str());

            ASSERT_TRUE(ts.adjustright());
            realtest(ts, oss, w);
            ASSERT_EQ(oss.str(), ts.str());
        }{
            CToString ts;
            std::ostringstream oss;
            ASSERT_EQ(oss.str(), ts.str());

            ts.adjustright(false);
            ASSERT_FALSE(ts.adjustright());
            realtest(ts, oss, w);
            ASSERT_EQ(oss.str(), ts.str());
        }{
            CToString ts;
            std::ostringstream oss;
            ASSERT_EQ(oss.str(), ts.str());

            oss<<99999;
            ts<<Manip::right<<99999;
            ASSERT_TRUE(ts.adjustright());
            realtest(ts, oss, w);
            ASSERT_EQ(oss.str(), ts.str());
        }{
            CToString ts;
            std::ostringstream oss;
            ASSERT_EQ(oss.str(), ts.str());

            oss<<99999;
            ts<<Manip::left<<99999;
            ASSERT_FALSE(ts.adjustright());
            realtest(ts, oss, w);
            ASSERT_EQ(oss.str(), ts.str());
        }
    }
}

TEST(CToString, fill)
{
    {
        CToString ts;
        std::ostringstream oss;
        ASSERT_EQ(oss.str(), ts.str());

        ASSERT_EQ(' ', ts.fill());
        realtest(ts, oss);
        ASSERT_EQ(oss.str(), ts.str());
    }{  //fill()
        {   //right
            {
                CToString ts;
                std::ostringstream oss;
                ASSERT_EQ(oss.str(), ts.str());

                ts.fill('a');
                ASSERT_EQ('a', ts.fill());

                oss<<99999;
                ts<<Manip::right<<99999;
                realtest(ts, oss, 15);
                ASSERT_EQ(oss.str(), ts.str());
            }{
                CToString ts;
                std::ostringstream oss;
                ASSERT_EQ(oss.str(), ts.str());

                ts.fill('a');
                ASSERT_EQ('a', ts.fill());

                oss<<99999;
                ts<<Manip::right<<99999;
                realtest(ts, oss, 0);
                ASSERT_EQ(oss.str(), ts.str());
            }
        }{  //left
            {
                CToString ts;
                std::ostringstream oss;
                ASSERT_EQ(oss.str(), ts.str());

                ts.fill('a');
                ASSERT_EQ('a', ts.fill());

                oss<<99999;
                ts<<Manip::left<<99999;
                realtest(ts, oss, 15);
                ASSERT_EQ(oss.str(), ts.str());
            }{
                CToString ts;
                std::ostringstream oss;
                ASSERT_EQ(oss.str(), ts.str());

                ts.fill('a');
                ASSERT_EQ('a', ts.fill());

                oss<<99999;
                ts<<Manip::left<<99999;
                realtest(ts, oss, 0);
                ASSERT_EQ(oss.str(), ts.str());
            }
        }
    }{  //Manip::fill
        {   //right
            {
                CToString ts;
                std::ostringstream oss;
                ASSERT_EQ(oss.str(), ts.str());

                oss<<9999;
                ts<<Manip::fill('a')<<9999;
                ASSERT_EQ('a', ts.fill());

                oss<<99999;
                ts<<Manip::right<<99999;
                realtest(ts, oss, 15);
                ASSERT_EQ(oss.str(), ts.str());
            }{
                CToString ts;
                std::ostringstream oss;
                ASSERT_EQ(oss.str(), ts.str());

                oss<<9999;
                ts<<Manip::fill('a')<<9999;
                ASSERT_EQ('a', ts.fill());

                oss<<99999;
                ts<<Manip::right<<99999;
                realtest(ts, oss, 0);
                ASSERT_EQ(oss.str(), ts.str());
            }
        }{  //left
            {
                CToString ts;
                std::ostringstream oss;
                ASSERT_EQ(oss.str(), ts.str());

                oss<<9999;
                ts<<Manip::fill('a')<<9999;
                ASSERT_EQ('a', ts.fill());

                oss<<99999;
                ts<<Manip::left<<99999;
                realtest(ts, oss, 15);
                ASSERT_EQ(oss.str(), ts.str());
            }{
                CToString ts;
                std::ostringstream oss;
                ASSERT_EQ(oss.str(), ts.str());

                oss<<9999;
                ts<<Manip::fill('a')<<9999;
                ASSERT_EQ('a', ts.fill());

                oss<<99999;
                ts<<Manip::left<<99999;
                realtest(ts, oss, 0);
                ASSERT_EQ(oss.str(), ts.str());
            }
        }
    }
}

TEST(CToString, base)
{
    {
        CToString ts;
        std::ostringstream oss;
        ASSERT_EQ(oss.str(), ts.str());

        ASSERT_EQ(10, ts.base());
        realtest(ts, oss);
        ASSERT_EQ(oss.str(), ts.str());
    }{
        CToString ts;
        std::ostringstream oss;
        ASSERT_EQ(oss.str(), ts.str());

        ts.base(16);
        ASSERT_EQ(16, ts.base());
        realtest(ts, oss);
        ASSERT_EQ(oss.str(), ts.str());
    }{
        CToString ts;
        std::ostringstream oss;
        ASSERT_EQ(oss.str(), ts.str());

        ts.base(8);
        ASSERT_EQ(8, ts.base());
        realtest(ts, oss);
        ASSERT_EQ(oss.str(), ts.str());
    }{
        CToString ts;
        std::ostringstream oss;
        ASSERT_EQ(oss.str(), ts.str());

        ts.base(2);
        ASSERT_EQ(2, ts.base());
        realtest(ts, oss);
        ASSERT_EQ(oss.str(), ts.str());
    }{
        CToString ts;
        std::ostringstream oss;
        ASSERT_EQ(oss.str(), ts.str());

        oss<<99999;
        ts<<Manip::setbase(10)<<99999;
        ASSERT_EQ(10, ts.base());
        realtest(ts, oss);
        ASSERT_EQ(oss.str(), ts.str());
    }{
        CToString ts;
        std::ostringstream oss;
        ASSERT_EQ(oss.str(), ts.str());

        oss<<0;
        ts<<Manip::setbase(16)<<0;
        ASSERT_EQ(16, ts.base());
        realtest(ts, oss);
        ASSERT_EQ(oss.str(), ts.str());
    }{
        CToString ts;
        std::ostringstream oss;
        ASSERT_EQ(oss.str(), ts.str());

        oss<<0;
        ts<<Manip::setbase(8)<<0;
        ASSERT_EQ(8, ts.base());
        realtest(ts, oss);
        ASSERT_EQ(oss.str(), ts.str());
    }{
        CToString ts;
        std::ostringstream oss;
        ASSERT_EQ(oss.str(), ts.str());

        oss<<0;
        ts<<Manip::setbase(2)<<0;
        ASSERT_EQ(2, ts.base());
        realtest(ts, oss);
        ASSERT_EQ(oss.str(), ts.str());
    }{
        CToString ts;
        std::ostringstream oss;
        ASSERT_EQ(oss.str(), ts.str());

        oss<<99999;
        ts<<Manip::dec<<99999;
        ASSERT_EQ(10, ts.base());
        realtest(ts, oss);
        ASSERT_EQ(oss.str(), ts.str());
    }{
        CToString ts;
        std::ostringstream oss;
        ASSERT_EQ(oss.str(), ts.str());

        oss<<0;
        ts<<Manip::bin<<0;
        ASSERT_EQ(2, ts.base());
        realtest(ts, oss);
        ASSERT_EQ(oss.str(), ts.str());
    }{
        CToString ts;
        std::ostringstream oss;
        ASSERT_EQ(oss.str(), ts.str());

        oss<<0;
        ts<<Manip::oct<<0;
        ASSERT_EQ(8, ts.base());
        realtest(ts, oss);
        ASSERT_EQ(oss.str(), ts.str());
    }{
        CToString ts;
        std::ostringstream oss;
        ASSERT_EQ(oss.str(), ts.str());

        oss<<0;
        ts<<Manip::hex<<0;
        ASSERT_EQ(16, ts.base());
        realtest(ts, oss);
        ASSERT_EQ(oss.str(), ts.str());
    }
}

TEST(CToString, showbase)
{
    {
        CToString ts;
        std::ostringstream oss;
        ASSERT_EQ(oss.str(), ts.str());
        ASSERT_TRUE(ts.showbase());

        ts.showbase(true);
        ASSERT_TRUE(ts.showbase());

        ts.base(2);
        realtest(ts, oss);
        ASSERT_EQ(oss.str(), ts.str());

        oss<<std::oct;
        ts.base(8);
        realtest(ts, oss);
        ASSERT_EQ(oss.str(), ts.str());

        ts.base(10);
        realtest(ts, oss);
        ASSERT_EQ(oss.str(), ts.str());

        ts.base(16);
        realtest(ts, oss);
        ASSERT_EQ(oss.str(), ts.str());
    }{
        CToString ts;
        std::ostringstream oss;
        ASSERT_EQ(oss.str(), ts.str());

        ts.showbase(false);
        ASSERT_FALSE(ts.showbase());

        ts.base(2);
        realtest(ts, oss);
        ASSERT_EQ(oss.str(), ts.str());

        oss<<std::oct;
        ts.base(8);
        realtest(ts, oss);
        ASSERT_EQ(oss.str(), ts.str());

        ts.base(10);
        realtest(ts, oss);
        ASSERT_EQ(oss.str(), ts.str());

        ts.base(16);
        realtest(ts, oss);
        ASSERT_EQ(oss.str(), ts.str());
    }{
        CToString ts;
        std::ostringstream oss;
        ASSERT_EQ(oss.str(), ts.str());

        oss<<0;
        ts<<Manip::showbase<<0;
        ASSERT_TRUE(ts.showbase());

        ts.base(2);
        realtest(ts, oss);
        ASSERT_EQ(oss.str(), ts.str());

        oss<<std::oct;
        ts.base(8);
        realtest(ts, oss);
        ASSERT_EQ(oss.str(), ts.str());

        ts.base(10);
        realtest(ts, oss);
        ASSERT_EQ(oss.str(), ts.str());

        ts.base(16);
        realtest(ts, oss);
        ASSERT_EQ(oss.str(), ts.str());
    }{
        CToString ts;
        std::ostringstream oss;
        ASSERT_EQ(oss.str(), ts.str());

        oss<<0;
        ts<<Manip::noshowbase<<0;
        ASSERT_FALSE(ts.showbase());

        ts.base(2);
        realtest(ts, oss);
        ASSERT_EQ(oss.str(), ts.str());

        oss<<std::oct;
        ts.base(8);
        realtest(ts, oss);
        ASSERT_EQ(oss.str(), ts.str());

        ts.base(10);
        realtest(ts, oss);
        ASSERT_EQ(oss.str(), ts.str());

        ts.base(16);
        realtest(ts, oss);
        ASSERT_EQ(oss.str(), ts.str());
    }
}

TEST(CToString, charcase)
{
    {
        CToString ts;
        std::ostringstream oss;
        ASSERT_EQ(oss.str(), ts.str());
        ASSERT_TRUE(CToString::kAutoCase == ts.charcase());

        ts.charcase(CToString::kAutoCase);
        ASSERT_TRUE(CToString::kAutoCase == ts.charcase());

        ts.base(2);
        realtest(ts, oss);
        ASSERT_EQ(oss.str(), ts.str());

        ts.base(8);
        realtest(ts, oss);
        ASSERT_EQ(oss.str(), ts.str());

        ts.base(10);
        realtest(ts, oss);
        ASSERT_EQ(oss.str(), ts.str());

        ts.base(16);
        realtest(ts, oss);
        ASSERT_EQ(oss.str(), ts.str());
    }{
        CToString ts;
        std::ostringstream oss;
        ASSERT_EQ(oss.str(), ts.str());

        ts.charcase(CToString::kUpperCase);
        ASSERT_TRUE(CToString::kUpperCase == ts.charcase());

        ts.base(2);
        realtest(ts, oss);
        ASSERT_EQ(oss.str(), ts.str());

        ts.base(8);
        realtest(ts, oss);
        ASSERT_EQ(oss.str(), ts.str());

        ts.base(10);
        realtest(ts, oss);
        ASSERT_EQ(oss.str(), ts.str());

        ts.base(16);
        realtest(ts, oss);
        ASSERT_EQ(oss.str(), ts.str());
    }{
        CToString ts;
        std::ostringstream oss;
        ASSERT_EQ(oss.str(), ts.str());

        ts.charcase(CToString::kLowerCase);
        ASSERT_TRUE(CToString::kLowerCase == ts.charcase());

        ts.base(2);
        realtest(ts, oss);
        ASSERT_EQ(oss.str(), ts.str());

        ts.base(8);
        realtest(ts, oss);
        ASSERT_EQ(oss.str(), ts.str());

        ts.base(10);
        realtest(ts, oss);
        ASSERT_EQ(oss.str(), ts.str());

        ts.base(16);
        realtest(ts, oss);
        ASSERT_EQ(oss.str(), ts.str());
    }{
        CToString ts;
        std::ostringstream oss;
        ASSERT_EQ(oss.str(), ts.str());

        oss<<0;
        ts<<Manip::autocase<<0;
        ASSERT_TRUE(CToString::kAutoCase == ts.charcase());

        ts.base(2);
        realtest(ts, oss);
        ASSERT_EQ(oss.str(), ts.str());

        ts.base(8);
        realtest(ts, oss);
        ASSERT_EQ(oss.str(), ts.str());

        ts.base(10);
        realtest(ts, oss);
        ASSERT_EQ(oss.str(), ts.str());

        ts.base(16);
        realtest(ts, oss);
        ASSERT_EQ(oss.str(), ts.str());
    }{
        CToString ts;
        std::ostringstream oss;
        ASSERT_EQ(oss.str(), ts.str());

        oss<<0;
        ts<<Manip::uppercase<<0;
        ASSERT_TRUE(CToString::kUpperCase == ts.charcase());

        ts.base(2);
        realtest(ts, oss);
        ASSERT_EQ(oss.str(), ts.str());

        ts.base(8);
        realtest(ts, oss);
        ASSERT_EQ(oss.str(), ts.str());

        ts.base(10);
        realtest(ts, oss);
        ASSERT_EQ(oss.str(), ts.str());

        ts.base(16);
        realtest(ts, oss);
        ASSERT_EQ(oss.str(), ts.str());
    }{
        CToString ts;
        std::ostringstream oss;
        ASSERT_EQ(oss.str(), ts.str());

        oss<<0;
        ts<<Manip::nouppercase<<0;
        ASSERT_TRUE(CToString::kLowerCase == ts.charcase());

        ts.base(2);
        realtest(ts, oss);
        ASSERT_EQ(oss.str(), ts.str());

        ts.base(8);
        realtest(ts, oss);
        ASSERT_EQ(oss.str(), ts.str());

        ts.base(10);
        realtest(ts, oss);
        ASSERT_EQ(oss.str(), ts.str());

        ts.base(16);
        realtest(ts, oss);
        ASSERT_EQ(oss.str(), ts.str());
    }
}

TEST(CToString, showpos)
{
    {
        CToString ts;
        std::ostringstream oss;
        ASSERT_EQ(oss.str(), ts.str());
        ASSERT_FALSE(ts.showpos());

        ts.showpos(true);
        ASSERT_TRUE(ts.showpos());

        realtest(ts, oss);
        ASSERT_EQ(oss.str(), ts.str());
    }{
        CToString ts;
        std::ostringstream oss;
        ASSERT_EQ(oss.str(), ts.str());

        ts.showpos(false);
        ASSERT_FALSE(ts.showpos());

        realtest(ts, oss);
        ASSERT_EQ(oss.str(), ts.str());
    }{
        CToString ts;
        std::ostringstream oss;
        ASSERT_EQ(oss.str(), ts.str());

        oss<<(-1);
        ts<<Manip::showpos<<(-1);
        ASSERT_TRUE(ts.showpos());

        realtest(ts, oss);
        ASSERT_EQ(oss.str(), ts.str());
    }{
        CToString ts;
        std::ostringstream oss;
        ASSERT_EQ(oss.str(), ts.str());

        oss<<(-1);
        ts<<Manip::noshowpos<<(-1);
        ASSERT_FALSE(ts.showpos());

        realtest(ts, oss);
        ASSERT_EQ(oss.str(), ts.str());
    }
}

TEST(CToString, endl)
{
    CToString ts;
    std::ostringstream oss;
    ASSERT_EQ(oss.str(), ts.str());

    ts<<12345<<Manip::endl<<23456;
    oss<<12345<<std::endl<<23456;
    ASSERT_EQ(oss.str(), ts.str());
}

TEST(CToString, ends)
{
    CToString ts;
    std::ostringstream oss;
    ASSERT_EQ(oss.str(), ts.str());

    ts<<12345<<Manip::ends<<23456;
    oss<<12345<<std::ends<<23456;
    ASSERT_EQ(oss.str(), ts.str());
}

TEST(CToString, str)
{
    CToString ts;
    std::ostringstream oss;
    ASSERT_EQ(oss.str(), ts.str());

    ts<<12345<<Manip::endl<<23456;
    oss<<12345<<std::endl<<23456;
    ASSERT_EQ(oss.str(), ts.str());

    std::string str("aeawebaw");
    ts.str(str);
    oss.str(str);
    ASSERT_EQ(oss.str(), ts.str());

    ts<<12345<<Manip::endl;
    oss<<str<<12345<<std::endl;     //NOTE: diff from oss
    ASSERT_EQ(oss.str(), ts.str());
}
