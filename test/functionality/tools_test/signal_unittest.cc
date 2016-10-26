#include <marine/tools/signal.hh>

#include "../inc.h"

void globalFunc(int){}

static void staticFunc(int){}

extern "C"
{
    void c_globalFunc(int){}

    static void c_staticFunc(int){}
}

namespace N{
    struct A
    {
        virtual ~A(){}
        static void staticFunc(int){}
        void memFunc(int){}
        virtual void virtFunc(int){}
    };
}

int globalVar;

static int staticVar;

extern char ** environ;

TEST(CSymbolInfo, all)
{
    CSymbolInfo s;

    s.symbolize(reinterpret_cast<void *>(&globalFunc));
    ASSERT_STREQ("globalFunc(int)", s.symbolName());

    s.symbolize(reinterpret_cast<void *>(&staticFunc));
    ASSERT_STREQ("staticFunc(int)", s.symbolName());

    s.symbolize(reinterpret_cast<void *>(&c_globalFunc));
    ASSERT_STREQ("c_globalFunc", s.symbolName());

    s.symbolize(reinterpret_cast<void *>(&c_staticFunc));
    ASSERT_STREQ("c_staticFunc", s.symbolName());

    s.symbolize(reinterpret_cast<void *>(&N::A::staticFunc));
    ASSERT_STREQ("N::A::staticFunc(int)", s.symbolName());

    union{
        void (N::A::* p_)(int);
        void * a_;
    }u;

    u.p_ = &N::A::memFunc;
    s.symbolize(u.a_);
    ASSERT_STREQ("N::A::memFunc(int)", s.symbolName());

    u.p_ = &N::A::virtFunc;
    s.symbolize(u.a_);
    ASSERT_STREQ("", s.symbolName())<<u.a_;

    s.symbolize(reinterpret_cast<void *>(&globalVar));
    ASSERT_STREQ("globalVar", s.symbolName());

    s.symbolize(reinterpret_cast<void *>(&staticVar));
    ASSERT_STREQ("staticVar", s.symbolName());

    static int localStaticVar;
    s.symbolize(reinterpret_cast<void *>(&localStaticVar));
    ASSERT_STREQ("CSymbolInfo_all_Test::TestBody()::localStaticVar", s.symbolName());

    int stackVar;
    s.symbolize(reinterpret_cast<void *>(&stackVar));
    ASSERT_STREQ("", s.symbolName());
    ASSERT_STREQ("[stack]", s.fileName());

    int * heapVar = new int;
    s.symbolize(reinterpret_cast<void *>(heapVar));
    ASSERT_STREQ("", s.symbolName());
    ASSERT_STREQ("[heap]", s.fileName());

    //lib var
    s.symbolize(reinterpret_cast<void *>(&environ));
    ASSERT_TRUE(0 == memcmp("__environ@@GLIBC_", s.symbolName(), 17)
            || 0 == memcmp("environ@@GLIBC_", s.symbolName(), 15))<<s.symbolName();

    //lib function
    s.symbolize(reinterpret_cast<void *>(&::read));
    ASSERT_EQ(0, memcmp("read@@GLIBC_", s.symbolName(), 12))<<s.symbolName();
}
