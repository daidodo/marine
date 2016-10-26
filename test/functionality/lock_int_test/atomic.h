#include "base.h"

TEST(NAME, static_load_store)
{
    __Val v = 0;
    ASSERT_EQ(__Val(0), __Int::load(&v));

    __Int::store(&v, 10);
    ASSERT_EQ(__Val(10), v);
}

TEST(NAME, static_fetch)
{
    {
        __Val a = 0, b = 0;
        __Val v = 0;
        for(__Val i = 0;i <= 100;++i){
            ASSERT_EQ(v, __Int::fetch_add(&a, i));
            v += i;
            ASSERT_EQ(v, __Int::add_fetch(&b, i));

            ASSERT_EQ(v, a);
            ASSERT_EQ(v, b);
        }
    }{
        __Val a = 0, b = 0;
        __Val v = 0;
        for(__Val i = 0;i <= 100;++i){
            ASSERT_EQ(v, __Int::fetch_sub(&a, i));
            v -= i;
            ASSERT_EQ(v, __Int::sub_fetch(&b, i));

            ASSERT_EQ(v, a);
            ASSERT_EQ(v, b);
        }
    }{
        __Val a = 0, b = 0;
        __Val v = 0;
        for(__Val i = 0;i <= 100;++i){
            ASSERT_EQ(v, __Int::fetch_and(&a, i));
            v &= i;
            ASSERT_EQ(v, __Int::and_fetch(&b, i));

            ASSERT_EQ(v, a);
            ASSERT_EQ(v, b);
        }
    }{
        __Val a = 0, b = 0;
        __Val v = 0;
        for(__Val i = 0;i <= 100;++i){
            ASSERT_EQ(v, __Int::fetch_or(&a, i));
            v |= i;
            ASSERT_EQ(v, __Int::or_fetch(&b, i));

            ASSERT_EQ(v, a);
            ASSERT_EQ(v, b);
        }
    }{
        __Val a = 0, b = 0;
        __Val v = 0;
        for(__Val i = 0;i <= 100;++i){
            ASSERT_EQ(v, __Int::fetch_xor(&a, i));
            v ^= i;
            ASSERT_EQ(v, __Int::xor_fetch(&b, i));

            ASSERT_EQ(v, a);
            ASSERT_EQ(v, b);
        }
    }{
        /*
        __Val a = 0, b = 0;
        __Val v = 0;
        for(__Val i = 0;i <= 100;++i){
            ASSERT_EQ(v, __Int::fetch_nand(&a, i));
            v = ~(v & i);
            ASSERT_EQ(v, __Int::nand_fetch(&b, i));

            ASSERT_EQ(v, a);
            ASSERT_EQ(v, b);
        }
        */
    }
}

TEST(NAME, static_swap)
{
    __Val a = 0;
    ASSERT_EQ(__Val(0), a);

    ASSERT_EQ(__Val(0), __Int::swap(&a, 0));
    ASSERT_EQ(__Val(0), a);

    ASSERT_EQ(__Val(0), __Int::swap(&a, 10));
    ASSERT_EQ(__Val(10), a);

    ASSERT_EQ(__Val(10), __Int::swap(&a, 20));
    ASSERT_EQ(__Val(20), a);

    ASSERT_EQ(__Val(20), __Int::swap(&a, 0));
    ASSERT_EQ(__Val(0), a);
}

TEST(NAME, static_compare_swap)
{
    {
        __Val a = 0;
        ASSERT_EQ(__Val(0), a);
        __Val v;

        ASSERT_FALSE(__Int::compare_swap(&a, 10, 11, &v));
        ASSERT_EQ(__Val(0), v);
        ASSERT_EQ(__Val(0), a);

        ASSERT_TRUE(__Int::compare_swap(&a, 0, 10, &v));
        ASSERT_EQ(__Val(0), v);
        ASSERT_EQ(__Val(10), a);

        ASSERT_FALSE(__Int::compare_swap(&a, 11, 12, &v));
        ASSERT_EQ(__Val(10), v);
        ASSERT_EQ(__Val(10), a);

        ASSERT_TRUE(__Int::compare_swap(&a, 10, 11, &v));
        ASSERT_EQ(__Val(10), v);
        ASSERT_EQ(__Val(11), a);
    }{
        __Val a = 0;
        ASSERT_EQ(__Val(0), a);

        ASSERT_FALSE(__Int::compare_swap(&a, 10, 11, NULL));
        ASSERT_EQ(__Val(0), a);

        ASSERT_TRUE(__Int::compare_swap(&a, 0, 10, NULL));
        ASSERT_EQ(__Val(10), a);

        ASSERT_FALSE(__Int::compare_swap(&a, 11, 12, NULL));
        ASSERT_EQ(__Val(10), a);

        ASSERT_TRUE(__Int::compare_swap(&a, 10, 11, NULL));
        ASSERT_EQ(__Val(11), a);
    }
}

