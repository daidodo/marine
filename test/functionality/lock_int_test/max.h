#include "int.h"

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

TEST(NAME, resetMax)
{
    __Int a;
    ASSERT_EQ(__Val(0), a);
    ASSERT_EQ(__Val(0), a.max());

    ASSERT_EQ(__Val(0), a.resetMax());
    ASSERT_EQ(__Val(0), a);
    ASSERT_EQ(__Val(0), a.max());

    a = 100;
    ASSERT_EQ(__Val(100), a);
    ASSERT_EQ(__Val(100), a.max());

    ASSERT_EQ(__Val(100), a.resetMax());
    ASSERT_EQ(__Val(100), a);
    ASSERT_EQ(__Val(100), a.max());

    a = 10;
    ASSERT_EQ(__Val(10), a);
    ASSERT_EQ(__Val(100), a.max());

    ASSERT_EQ(__Val(100), a.resetMax());
    ASSERT_EQ(__Val(10), a);
    ASSERT_EQ(__Val(10), a.max());

    a = 0;
    ASSERT_EQ(__Val(0), a);
    ASSERT_EQ(__Val(10), a.max());

    ASSERT_EQ(__Val(10), a.resetMax());
    ASSERT_EQ(__Val(0), a);
    ASSERT_EQ(__Val(0), a.max());
}
