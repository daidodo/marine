#define UNIT_TEST
#include <marine/coroutine.hh>
#include <deque>
#include "../inc.h"

typedef CCoroutine<void> __Coro;

namespace ctor{
    void func(__Coro::yield_type & yield){}
}

TEST(CCoroutine, ctor)
{
    using namespace ctor;
    __Coro c1(func);
    __Coro c2(func, 12345);
}

namespace yield_main{
    int sum, i;
    void func(__Coro::yield_type & yield)
    {
        for(;;){
            sum += i;
            yield();
        }
    }
}

TEST(CCoroutine, yield_main)
{
    using namespace yield_main;
    __Coro c(func);
    sum = 0;
    for(i = 1;i <= 10;++i)
        c();
    ASSERT_EQ(55, sum);
}

namespace yield_val{
    typedef CCoroutine<int> __Coro2;
    int sum;
    void func1(__Coro2::yield_type & yield)
    {
        for(;;){
            sum += yield.get();
            yield();
        }
    }
    __Coro2 s(func1);
    void func2(__Coro::yield_type & yield)
    {
        for(int i = 1;i <= 10;++i)
            yield(s, i);
    }
}

TEST(CCoroutine, yield_val)
{
    using namespace yield_val;
    __Coro c(func2);
    sum = 0;
    c();
    ASSERT_EQ(55, sum);
}

namespace yield_ref{
    typedef CCoroutine<int &> __Coro2;
    int sum;
    void func1(__Coro2::yield_type & yield)
    {
        for(;;){
            sum += ++yield.get();
            yield();
        }
    }
    __Coro2 s(func1);
    void func2(__Coro::yield_type & yield)
    {
        for(int i = 0;i < 10;)
            yield(s, i);
    }
}

TEST(CCoroutine, yield_ref)
{
    using namespace yield_ref;
    __Coro c(func2);
    sum = 0;
    c();
    ASSERT_EQ(55, sum);
}

namespace yield_void{
    typedef CCoroutine<void> __Coro2;
    int sum, v;
    void func1(__Coro2::yield_type & yield)
    {
        for(;;){
            sum += ++v;
            yield();
        }
    }
    __Coro2 s(func1);
    void func2(__Coro::yield_type & yield)
    {
        for(int i = 0;i < 10;++i)
            yield(s);
    }
}

TEST(CCoroutine, yield_void)
{
    using namespace yield_void;
    __Coro c(func2);
    sum = v = 0;
    c();
    ASSERT_EQ(55, sum);
}
