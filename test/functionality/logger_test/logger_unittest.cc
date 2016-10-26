#include <marine/logger.hh>

#include "../inc.h"

struct CTest
{
    void memFunc(int p1, const char * p2){
        TRACE("this is trace in member function for p1="<<p1<<", p2="<<p2);
        DEBUG("this is debug in member function for p1="<<p1<<", p2="<<p2);
        INFO("this is info in member function for p1="<<p1<<", p2="<<p2);
        WARN("this is warn in member function for p1="<<p1<<", p2="<<p2);
        ERROR("this is error in member function for p1="<<p1<<", p2="<<p2);
        //FATAL("this is fatal in member function for p1="<<p1<<", p2="<<p2);
    }
};

static void writeLog()
{
    const char a1[] = "char []";
    const std::string a2("std::string");
    int i = 1123;
    const char * msg = a1;
    ERROR("this is error msg "<<i<<", "<<msg);
    TRACE("this is trace msg "<<i<<", "<<msg);
    DEBUG("this is debug msg "<<i<<", "<<msg);
    INFO("this is info msg "<<i<<", "<<msg);
    WARN("this is warn msg "<<i<<", "<<msg);
    ERROR("char *, "<<a1
            <<std::string("std::string")
            <<char('a')
            <<(unsigned char)('b')
            <<(signed char)('b')
            <<short(1234)
            <<(unsigned short)(2345)
            <<int(123467)
            <<(unsigned int)(234578)
            <<long(123467)
            <<(unsigned long)(234578)
            <<(long long)(123467)
            <<(unsigned long long)(234578)
            <<true<<false
            <<&a2);
    //FATAL("this is fatal msg "<<i<<", "<<msg);
    CTest t;
    t.memFunc(i, msg);
}

TEST(logger, LOGGER_INIT)
{
#define FNAME   "./LOGGER_INIT.log"
    //before init
    {
        CFile file(FNAME);
        ASSERT_FALSE(file.valid());
    }
    writeLog();
    {
        CFile file(FNAME);
        ASSERT_FALSE(file.valid());
    }
    //init self
    ASSERT_TRUE(LOGGER_INIT("./logger_unittest", "OFF", 10240, 5));
    ASSERT_FALSE(LOGGER_INIT("./logger_unittest", "TRACE", 10240, 5));
    //init off
    ASSERT_TRUE(LOGGER_INIT(FNAME, "OFF", 10240, 5));
    {
        CFile file(FNAME);
        ASSERT_FALSE(file.valid());
    }
    writeLog();
    {
        CFile file(FNAME);
        ASSERT_FALSE(file.valid());
    }
    //init
    ASSERT_TRUE(LOGGER_INIT(FNAME, "TRACE", 10240, 5));
    {
        CFile file(FNAME);
        ASSERT_TRUE(file.valid());
        ASSERT_EQ(0, file.length());
    }
    writeLog();
    {
        CFile file(FNAME);
        ASSERT_TRUE(file.valid());
        ASSERT_LT(0, file.length());
    }
    system("rm -f "FNAME"*");
#undef FNAME
}

TEST(logger, LOGGER_SET_LEVEL)
{
#define FNAME   "./LOGGER_SET_LEVEL.log"
    LOGGER_INIT(FNAME, "TRACE", 10240, 5);
    system("rm -f "FNAME"*");
    {
        LOGGER_SET_LEVEL("OFF");
        {
            CFile file(FNAME);
            ASSERT_FALSE(file.valid());
        }
        FATAL("a");
        ERROR("a");
        WARN("a");
        INFO("a");
        DEBUG("a");
        TRACE("a");
        {
            CFile file(FNAME);
            ASSERT_FALSE(file.valid());
        }
        system("rm -f "FNAME"*");
    }{
        LOGGER_SET_LEVEL("FATAL");
        {
            CFile file(FNAME);
            ASSERT_FALSE(file.valid());
        }
        ERROR("a");
        WARN("a");
        INFO("a");
        DEBUG("a");
        TRACE("a");
        {
            CFile file(FNAME);
            ASSERT_FALSE(file.valid());
        }
        FATAL("a");
        {
            CFile file(FNAME);
            ASSERT_TRUE(file.valid());
            ASSERT_LT(0, file.length());
        }
        system("rm -f "FNAME"*");
    }{
        LOGGER_SET_LEVEL("ERROR");
        {
            CFile file(FNAME);
            ASSERT_FALSE(file.valid());
        }
        WARN("a");
        INFO("a");
        DEBUG("a");
        TRACE("a");
        {
            CFile file(FNAME);
            ASSERT_FALSE(file.valid());
        }
        FATAL("a");
        {
            CFile file(FNAME);
            ASSERT_TRUE(file.valid());
            ASSERT_LT(0, file.length());
        }
        system("rm -f "FNAME"*");
        {
            CFile file(FNAME);
            ASSERT_FALSE(file.valid());
        }
        ERROR("a");
        {
            CFile file(FNAME);
            ASSERT_TRUE(file.valid());
            ASSERT_LT(0, file.length());
        }
        system("rm -f "FNAME"*");
    }{
        LOGGER_SET_LEVEL("WARN");
        {
            CFile file(FNAME);
            ASSERT_FALSE(file.valid());
        }
        INFO("a");
        DEBUG("a");
        TRACE("a");
        {
            CFile file(FNAME);
            ASSERT_FALSE(file.valid());
        }
        FATAL("a");
        {
            CFile file(FNAME);
            ASSERT_TRUE(file.valid());
            ASSERT_LT(0, file.length());
        }
        system("rm -f "FNAME"*");
        {
            CFile file(FNAME);
            ASSERT_FALSE(file.valid());
        }
        ERROR("a");
        {
            CFile file(FNAME);
            ASSERT_TRUE(file.valid());
            ASSERT_LT(0, file.length());
        }
        system("rm -f "FNAME"*");
        {
            CFile file(FNAME);
            ASSERT_FALSE(file.valid());
        }
        WARN("a");
        {
            CFile file(FNAME);
            ASSERT_TRUE(file.valid());
            ASSERT_LT(0, file.length());
        }
        system("rm -f "FNAME"*");
    }{
        LOGGER_SET_LEVEL("INFO");
        {
            CFile file(FNAME);
            ASSERT_FALSE(file.valid());
        }
        DEBUG("a");
        TRACE("a");
        {
            CFile file(FNAME);
            ASSERT_FALSE(file.valid());
        }
        FATAL("a");
        {
            CFile file(FNAME);
            ASSERT_TRUE(file.valid());
            ASSERT_LT(0, file.length());
        }
        system("rm -f "FNAME"*");
        {
            CFile file(FNAME);
            ASSERT_FALSE(file.valid());
        }
        ERROR("a");
        {
            CFile file(FNAME);
            ASSERT_TRUE(file.valid());
            ASSERT_LT(0, file.length());
        }
        system("rm -f "FNAME"*");
        {
            CFile file(FNAME);
            ASSERT_FALSE(file.valid());
        }
        WARN("a");
        {
            CFile file(FNAME);
            ASSERT_TRUE(file.valid());
            ASSERT_LT(0, file.length());
        }
        system("rm -f "FNAME"*");
        {
            CFile file(FNAME);
            ASSERT_FALSE(file.valid());
        }
        INFO("a");
        {
            CFile file(FNAME);
            ASSERT_TRUE(file.valid());
            ASSERT_LT(0, file.length());
        }
        system("rm -f "FNAME"*");
    }{
        LOGGER_SET_LEVEL("DEBUG");
        {
            CFile file(FNAME);
            ASSERT_FALSE(file.valid());
        }
        TRACE("a");
        {
            CFile file(FNAME);
            ASSERT_FALSE(file.valid());
        }
        FATAL("a");
        {
            CFile file(FNAME);
            ASSERT_TRUE(file.valid());
            ASSERT_LT(0, file.length());
        }
        system("rm -f "FNAME"*");
        {
            CFile file(FNAME);
            ASSERT_FALSE(file.valid());
        }
        ERROR("a");
        {
            CFile file(FNAME);
            ASSERT_TRUE(file.valid());
            ASSERT_LT(0, file.length());
        }
        system("rm -f "FNAME"*");
        {
            CFile file(FNAME);
            ASSERT_FALSE(file.valid());
        }
        WARN("a");
        {
            CFile file(FNAME);
            ASSERT_TRUE(file.valid());
            ASSERT_LT(0, file.length());
        }
        system("rm -f "FNAME"*");
        {
            CFile file(FNAME);
            ASSERT_FALSE(file.valid());
        }
        INFO("a");
        {
            CFile file(FNAME);
            ASSERT_TRUE(file.valid());
            ASSERT_LT(0, file.length());
        }
        system("rm -f "FNAME"*");
        {
            CFile file(FNAME);
            ASSERT_FALSE(file.valid());
        }
        DEBUG("a");
        {
            CFile file(FNAME);
            ASSERT_TRUE(file.valid());
            ASSERT_LT(0, file.length());
        }
        system("rm -f "FNAME"*");
    }{
        LOGGER_SET_LEVEL("TRACE");
        {
            CFile file(FNAME);
            ASSERT_FALSE(file.valid());
        }
        FATAL("a");
        {
            CFile file(FNAME);
            ASSERT_TRUE(file.valid());
            ASSERT_LT(0, file.length());
        }
        system("rm -f "FNAME"*");
        {
            CFile file(FNAME);
            ASSERT_FALSE(file.valid());
        }
        ERROR("a");
        {
            CFile file(FNAME);
            ASSERT_TRUE(file.valid());
            ASSERT_LT(0, file.length());
        }
        system("rm -f "FNAME"*");
        {
            CFile file(FNAME);
            ASSERT_FALSE(file.valid());
        }
        WARN("a");
        {
            CFile file(FNAME);
            ASSERT_TRUE(file.valid());
            ASSERT_LT(0, file.length());
        }
        system("rm -f "FNAME"*");
        {
            CFile file(FNAME);
            ASSERT_FALSE(file.valid());
        }
        INFO("a");
        {
            CFile file(FNAME);
            ASSERT_TRUE(file.valid());
            ASSERT_LT(0, file.length());
        }
        system("rm -f "FNAME"*");
        {
            CFile file(FNAME);
            ASSERT_FALSE(file.valid());
        }
        DEBUG("a");
        {
            CFile file(FNAME);
            ASSERT_TRUE(file.valid());
            ASSERT_LT(0, file.length());
        }
        system("rm -f "FNAME"*");
        {
            CFile file(FNAME);
            ASSERT_FALSE(file.valid());
        }
        TRACE("a");
        {
            CFile file(FNAME);
            ASSERT_TRUE(file.valid());
            ASSERT_LT(0, file.length());
        }
        system("rm -f "FNAME"*");
    }
#undef FNAME
}

TEST(logger, LOGGER_SET_MAX_SIZE)
{
#define FNAME   "./LOGGER_SET_MAX_SIZE.log"
#define FNAME1  FNAME".1"
    LOGGER_INIT(FNAME, "TRACE", 10240, 5);
    system("rm -f "FNAME"*");
    {
        LOGGER_SET_MAX_SIZE(0); //4K
        {
            CFile file(FNAME1);
            ASSERT_FALSE(file.valid());
        }
        for(int i = 0;i < 100;++i)
            writeLog(); //1.2K * 100
        {
            CFile file(FNAME1);
            ASSERT_TRUE(file.valid());
            ASSERT_LE((4 << 10), file.length());
            ASSERT_GT((5 << 10), file.length());
        }
        system("rm -f "FNAME"*");
    }{
        LOGGER_SET_MAX_SIZE(10 << 10); //10K
        {
            CFile file(FNAME1);
            ASSERT_FALSE(file.valid());
        }
        for(int i = 0;i < 100;++i)
            writeLog(); //1.2K * 100
        {
            CFile file(FNAME1);
            ASSERT_TRUE(file.valid());
            ASSERT_LE((10 << 10), file.length());
            ASSERT_GT((11 << 10), file.length());
        }
        system("rm -f "FNAME"*");
    }
#undef FNAME
#undef FNAME1
}

static int logIndex(const char * base)
{
    assert(base);
    int i = 0;
    for(;;++i){
        CToString oss;
        oss<<base;
        if(i)
            oss<<"."<<i;
        CFile file(oss.str());
        if(!file.valid())
            break;
    }
    return i;
}

TEST(logger, LOGGER_SET_MAX_NUMBER)
{
#define FNAME   "./LOGGER_SET_MAX_NUMBER.log"
    LOGGER_INIT(FNAME, "TRACE", 4096, 5);
    system("rm -f "FNAME"*");
    for(int i = 0;i <= 20;++i){
        LOGGER_SET_MAX_NUMBER(i);
        ASSERT_EQ(0, logIndex(FNAME));
        for(int j = 0;j < 100;++j)
            writeLog(); //1.2K * 100
        ASSERT_EQ(i + 1, logIndex(FNAME));
        system("rm -f "FNAME"*");
    }
#undef FNAME
}

