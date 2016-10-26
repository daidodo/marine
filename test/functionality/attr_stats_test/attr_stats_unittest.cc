#include <marine/attr_stats.hh>

#include "../inc.h"

TEST(attr, ATTR_INIT)
{
    ASSERT_TRUE(ATTR_INIT());

    ASSERT_FALSE(ATTR_INIT(100));
}

TEST(attr, ATTR_ADD)
{
    {
        ATTR_SET(100, 0);
        for(int i = 0;i < 10;++i)
            ATTR_ADD(100, 1);

        uint64_t x = 0;
        ATTR_SET_EX(100, 0, &x);
        ASSERT_EQ(10, int(x));
    }{
        int a;
        a = 100;

        ATTR_SET_SLOW(a, 0);
        for(int i = 0;i < 10;++i)
            ATTR_ADD_SLOW(a, 1);

        uint64_t x = 0;
        ATTR_SET_SLOW(a, 0, &x);
        ASSERT_EQ(10, int(x));
    }
}

TEST(attr, ATTR_SET)
{
    uint64_t x;
    {
        ATTR_SET(100, 0);
        ATTR_SET_EX(100, 1, &x);
        ASSERT_EQ(0, int(x));

        ATTR_SET(100, 10);
        ATTR_SET_EX(100, 1, &x);
        ASSERT_EQ(10, int(x));
    }{
        int a;
        a = 100;

        ATTR_SET_SLOW(a, 0);
        ATTR_SET_SLOW(a, 1, &x);
        ASSERT_EQ(0, int(x));

        ATTR_SET_SLOW(a, 10);
        ATTR_SET_SLOW(a, 1, &x);
        ASSERT_EQ(10, int(x));
    }
}

TEST(attr, ATTR_SET_EX)
{
    uint64_t x;
    {
        ATTR_SET(100, 0);
        for(int i = 1;i <= 10;++i){
            ATTR_SET_EX(100, i, &x);
            ASSERT_EQ(i - 1, int(x));
        }
        ATTR_SET_EX(100, 0, &x);
        ASSERT_EQ(10, int(x));
    }{
        int a;
        a = 100;

        ATTR_SET_SLOW(a, 0);
        for(int i = 1;i <= 10;++i){
            ATTR_SET_SLOW(a, i, &x);
            ASSERT_EQ(i - 1, int(x));
        }
        ATTR_SET_SLOW(a, 0, &x);
        ASSERT_EQ(10, int(x));
    }
}

struct A
{
    static int s;
    void operator ()(int attr, uint64_t value) const
    {
        ASSERT_EQ(attr, int(value));
        s += value;
    }
};

int A::s = 0;

TEST(attr, ATTR_ITERATE)
{
    for(int i = 0;i <= 100;++i)
        ATTR_SET_SLOW(i, i);

    ATTR_ITERATE(A());
    ASSERT_EQ(5050, A::s);
}
