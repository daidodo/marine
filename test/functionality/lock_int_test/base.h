#include "../inc.h"
#include "comm.h"

#define CAT_TOKEN_1(t1,t2) t1##_##t2
#define CAT_TOKEN(t1,t2) CAT_TOKEN_1(t1,t2)
#define NAME    CAT_TOKEN(TYPE, VALN)

#ifdef __Int
#   undef __Int
#endif
#ifdef __Val
#   undef __Val
#endif
#define __Int   TYPE<VAL>
#define __Val   __Int::value_type

TEST(NAME, ctor)
{
    {
        __Int a;
        ASSERT_EQ(__Val(0), a);
    }{
        __Int a(10);
        ASSERT_EQ(__Val(10), a);
    }{
        __Int a(10);
        ASSERT_EQ(__Val(10), a);

        __Int b(a);
        ASSERT_EQ(__Val(10), b);
    }{
        __Int a(10);
        ASSERT_EQ(__Val(10), a);

        __Int b = a;
        ASSERT_EQ(__Val(10), b);
    }
}

TEST(NAME, store_load)
{
    __Int a;
    ASSERT_EQ(__Val(0), a.load());

    a.store(10);
    ASSERT_EQ(__Val(10), a.load());
}

TEST(NAME, fetch)
{
    {
        __Int a, b, c;
        ASSERT_EQ(__Val(0), a);
        ASSERT_EQ(__Val(0), b);
        ASSERT_EQ(__Val(0), c);
        __Val v = 0;
        for(__Val i = 0;i <= 100;++i){
            ASSERT_EQ(v, a.fetch_add(i));
            v += i;
            ASSERT_EQ(v, b.add_fetch(i));
            ASSERT_EQ(v, c += i);

            ASSERT_EQ(v, a);
            ASSERT_EQ(v, b);
            ASSERT_EQ(v, c);
        }
    }{
        __Int a, b, c;
        ASSERT_EQ(__Val(0), a);
        ASSERT_EQ(__Val(0), b);
        ASSERT_EQ(__Val(0), c);
        __Val v = 0;
        for(__Val i = 0;i <= 100;++i){
            ASSERT_EQ(v, a.fetch_sub(i));
            v -= i;
            ASSERT_EQ(v, b.sub_fetch(i));
            ASSERT_EQ(v, c -= i);

            ASSERT_EQ(v, a);
            ASSERT_EQ(v, b);
            ASSERT_EQ(v, c);
        }
    }{
        __Int a, b, c;
        ASSERT_EQ(__Val(0), a);
        ASSERT_EQ(__Val(0), b);
        ASSERT_EQ(__Val(0), c);
        __Val v = 0;
        for(__Val i = 0;i <= 100;++i){
            ASSERT_EQ(v, a.fetch_and(i));
            v &= i;
            ASSERT_EQ(v, b.and_fetch(i));
            ASSERT_EQ(v, c &= i);

            ASSERT_EQ(v, a);
            ASSERT_EQ(v, b);
            ASSERT_EQ(v, c);
        }
    }{
        __Int a, b, c;
        ASSERT_EQ(__Val(0), a);
        ASSERT_EQ(__Val(0), b);
        ASSERT_EQ(__Val(0), c);
        __Val v = 0;
        for(__Val i = 0;i <= 100;++i){
            ASSERT_EQ(v, a.fetch_or(i));
            v |= i;
            ASSERT_EQ(v, b.or_fetch(i));
            ASSERT_EQ(v, c |= i);

            ASSERT_EQ(v, a);
            ASSERT_EQ(v, b);
            ASSERT_EQ(v, c);
        }
    }{
        __Int a, b, c;
        ASSERT_EQ(__Val(0), a);
        ASSERT_EQ(__Val(0), b);
        ASSERT_EQ(__Val(0), c);
        __Val v = 0;
        for(__Val i = 0;i <= 100;++i){
            ASSERT_EQ(v, a.fetch_xor(i));
            v ^= i;
            ASSERT_EQ(v, b.xor_fetch(i));
            ASSERT_EQ(v, c ^= i);

            ASSERT_EQ(v, a);
            ASSERT_EQ(v, b);
            ASSERT_EQ(v, c);
        }
    }{
        /*
         __Int a, b;
        ASSERT_EQ(__Val(0), a);
        ASSERT_EQ(__Val(0), b);
        __Val v = 0;
        for(__Val i = 0;i <= 100;++i){
            ASSERT_EQ(v, a.fetch_nand(i));
            v = ~(v & i);
            ASSERT_EQ(v, b.nand_fetch(i));

            ASSERT_EQ(v, a);
            ASSERT_EQ(v, b);
        }
        */
    }
}

TEST(NAME, swap)
{
    __Int a;
    ASSERT_EQ(__Val(0), a);

    ASSERT_EQ(__Val(0), a.swap(0));
    ASSERT_EQ(__Val(0), a);

    ASSERT_EQ(__Val(0), a.swap(10));
    ASSERT_EQ(__Val(10), a);

    ASSERT_EQ(__Val(10), a.swap(20));
    ASSERT_EQ(__Val(20), a);

    ASSERT_EQ(__Val(20), a.swap(0));
    ASSERT_EQ(__Val(0), a);
}

TEST(NAME, compare_swap)
{
    {
        __Int a;
        ASSERT_EQ(__Val(0), a);
        __Val v;

        ASSERT_FALSE(a.compare_swap(10, 11, &v));
        ASSERT_EQ(__Val(0), v);
        ASSERT_EQ(__Val(0), a);

        ASSERT_TRUE(a.compare_swap(0, 10, &v));
        ASSERT_EQ(__Val(0), v);
        ASSERT_EQ(__Val(10), a);

        ASSERT_FALSE(a.compare_swap(11, 12, &v));
        ASSERT_EQ(__Val(10), v);
        ASSERT_EQ(__Val(10), a);

        ASSERT_TRUE(a.compare_swap(10, 11, &v));
        ASSERT_EQ(__Val(10), v);
        ASSERT_EQ(__Val(11), a);
    }{
        __Int a;
        ASSERT_EQ(__Val(0), a);

        ASSERT_FALSE(a.compare_swap(10, 11, NULL));
        ASSERT_EQ(__Val(0), a);

        ASSERT_TRUE(a.compare_swap(0, 10, NULL));
        ASSERT_EQ(__Val(10), a);

        ASSERT_FALSE(a.compare_swap(11, 12, NULL));
        ASSERT_EQ(__Val(10), a);

        ASSERT_TRUE(a.compare_swap(10, 11, NULL));
        ASSERT_EQ(__Val(11), a);
    }
}

TEST(NAME, assign)
{
    {
        __Int a;
        __Int b(10);
        ASSERT_EQ(__Val(0), a);
        ASSERT_EQ(__Val(10), b);

        a = b;
        ASSERT_EQ(__Val(10), a);
        ASSERT_EQ(__Val(10), b);

        __Int c;
        ASSERT_EQ(__Val(0), c);

        a = c;
        ASSERT_EQ(__Val(0), a);
        ASSERT_EQ(__Val(0), c);
    }{
        __Int a;
        ASSERT_EQ(__Val(0), a);

        a = 10;
        ASSERT_EQ(__Val(10), a);

        a = 0;
        ASSERT_EQ(__Val(0), a);
    }
}

TEST(NAME, inc_dec)
{
    __Int a, b, c, d;
    ASSERT_EQ(__Val(0), a);
    ASSERT_EQ(__Val(0), b);
    ASSERT_EQ(__Val(0), c);
    ASSERT_EQ(__Val(0), d);

    for(__Val i = 0, j = 0;i <= 100;){
        ASSERT_EQ(i, a++);
        ASSERT_EQ(j, b--);
        ++i;
        --j;
        ASSERT_EQ(i, ++c);
        ASSERT_EQ(j, --d);

        ASSERT_EQ(i, a);
        ASSERT_EQ(j, b);
        ASSERT_EQ(i, c);
        ASSERT_EQ(j, d);
    }
}
