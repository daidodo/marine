#include "int.h"

TEST(NAME, min)
{
    __Int a(100);
    ASSERT_EQ(__Val(100), a);
    ASSERT_EQ(__Val(100), a.max());

    for(__Val i = 99;i > 0;--i){
        --a;
        ASSERT_EQ(__Val(i), a);
        ASSERT_EQ(__Val(i), a.min())<<"i="<<i;
    }
    for(__Val i = 2;i <= 100;++i){
        ++a;
        ASSERT_EQ(__Val(i), a);
        ASSERT_EQ(__Val(1), a.min())<<"i="<<i;
    }
}

TEST(NAME, max)
{
    __Int a;
    ASSERT_EQ(__Val(0), a);
    ASSERT_EQ(__Val(0), a.max());

    for(__Val i = 1;i <= 100;++i){
        ++a;
        ASSERT_EQ(__Val(i), a);
        ASSERT_EQ(__Val(i), a.max());
    }
    for(__Val i = 99;i > 0;--i){
        --a;
        ASSERT_EQ(__Val(i), a);
        ASSERT_EQ(__Val(100), a.max())<<"i="<<i;
    }
}

TEST(NAME, resetMinMax)
{
    __Int a;
    ASSERT_EQ(__Val(0), a);
    ASSERT_EQ(__Val(0), a.min());
    ASSERT_EQ(__Val(0), a.max());

    __Val mi, ma;
    a.resetMinMax(&mi, &ma);
    ASSERT_EQ(__Val(0), mi);
    ASSERT_EQ(__Val(0), ma);
    ASSERT_EQ(__Val(0), a);
    ASSERT_EQ(__Val(0), a.min());
    ASSERT_EQ(__Val(0), a.max());

    a = 100;
    ASSERT_EQ(__Val(100), a);
    ASSERT_EQ(__Val(0), a.min());
    ASSERT_EQ(__Val(100), a.max());

    a.resetMinMax(&mi, &ma);
    ASSERT_EQ(__Val(0), mi);
    ASSERT_EQ(__Val(100), ma);
    ASSERT_EQ(__Val(100), a);
    ASSERT_EQ(__Val(100), a.min());
    ASSERT_EQ(__Val(100), a.max());

    a = 10;
    ASSERT_EQ(__Val(10), a);
    ASSERT_EQ(__Val(10), a.min());
    ASSERT_EQ(__Val(100), a.max());

    a.resetMinMax(&mi, &ma);
    ASSERT_EQ(__Val(10), mi);
    ASSERT_EQ(__Val(100), ma);
    ASSERT_EQ(__Val(10), a);
    ASSERT_EQ(__Val(10), a.min());
    ASSERT_EQ(__Val(10), a.max());

    a = 0;
    ASSERT_EQ(__Val(0), a);
    ASSERT_EQ(__Val(0), a.min());
    ASSERT_EQ(__Val(10), a.max());

    a.resetMinMax(&mi, &ma);
    ASSERT_EQ(__Val(0), mi);
    ASSERT_EQ(__Val(10), ma);
    ASSERT_EQ(__Val(0), a);
    ASSERT_EQ(__Val(0), a.min());
    ASSERT_EQ(__Val(0), a.max());

    a.resetMinMax(&mi, NULL);
    a.resetMinMax(NULL, &ma);
    a.resetMinMax(NULL, NULL);
}
