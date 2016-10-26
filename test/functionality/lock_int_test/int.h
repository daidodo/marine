#include "base.h"

TEST(NAME, test_mutate)
{
    {
        __Int a;
        __Val oldVal = a;
        __Val realVal = 0;
        ASSERT_EQ(__Val(0), oldVal);
        for(__Val i = 0;i <= 100;++i, oldVal = a){
            __Val ov, nv;
            a.test_mutate(IsEven<__Val>(i), AddSquare<__Val>(i), &ov, &nv);

            ASSERT_EQ(oldVal, ov);
            ASSERT_EQ(realVal, ov);
            if((realVal + i) % 2 == 0)
                realVal += i * i;
            ASSERT_EQ(realVal, nv);
            ASSERT_EQ(realVal, a);
        }
    }{
        __Int a;
        __Val oldVal = a;
        __Val realVal = 0;
        ASSERT_EQ(__Val(0), oldVal);
        for(__Val i = 0;i <= 100;++i, oldVal = a){
            __Val nv;
            a.test_mutate(IsEven<__Val>(i), AddSquare<__Val>(i), NULL, &nv);

            if((realVal + i) % 2 == 0)
                realVal += i * i;
            ASSERT_EQ(realVal, nv);
            ASSERT_EQ(realVal, a);
        }
    }{
        __Int a;
        __Val oldVal = a;
        __Val realVal = 0;
        ASSERT_EQ(__Val(0), oldVal);
        for(__Val i = 0;i <= 100;++i, oldVal = a){
            __Val ov;
            a.test_mutate(IsEven<__Val>(i), AddSquare<__Val>(i), &ov, NULL);

            ASSERT_EQ(oldVal, ov);
            ASSERT_EQ(realVal, ov);
            if((realVal + i) % 2 == 0)
                realVal += i * i;
            ASSERT_EQ(realVal, a);
        }
    }{
        __Int a;
        __Val oldVal = a;
        __Val realVal = 0;
        ASSERT_EQ(__Val(0), oldVal);
        for(__Val i = 0;i <= 100;++i, oldVal = a){
            a.test_mutate(IsEven<__Val>(i), AddSquare<__Val>(i), NULL, NULL);

            if((realVal + i) % 2 == 0)
                realVal += i * i;
            ASSERT_EQ(realVal, a);
        }
    }
}

TEST(NAME, mutate)
{
    {
        __Int a;
        __Val oldVal = a;
        __Val realVal = 0;
        ASSERT_EQ(__Val(0), oldVal);
        for(__Val i = 0;i <= 100;++i, oldVal = a){
            __Val ov, nv;
            a.mutate(AddSquare<__Val>(i), &ov, &nv);

            ASSERT_EQ(oldVal, ov);
            ASSERT_EQ(realVal, ov);
            realVal += i * i;
            ASSERT_EQ(realVal, nv);
            ASSERT_EQ(realVal, a);
        }
    }{
        __Int a;
        __Val oldVal = a;
        __Val realVal = 0;
        ASSERT_EQ(__Val(0), oldVal);
        for(__Val i = 0;i <= 100;++i, oldVal = a){
            __Val nv;
            a.mutate(AddSquare<__Val>(i), NULL, &nv);

            realVal += i * i;
            ASSERT_EQ(realVal, nv);
            ASSERT_EQ(realVal, a);
        }
    }{
        __Int a;
        __Val oldVal = a;
        __Val realVal = 0;
        ASSERT_EQ(__Val(0), oldVal);
        for(__Val i = 0;i <= 100;++i, oldVal = a){
            __Val ov;
            a.mutate(AddSquare<__Val>(i), &ov, NULL);

            ASSERT_EQ(oldVal, ov);
            ASSERT_EQ(realVal, ov);
            realVal += i * i;
            ASSERT_EQ(realVal, a);
        }
    }{
        __Int a;
        __Val oldVal = a;
        __Val realVal = 0;
        ASSERT_EQ(__Val(0), oldVal);
        for(__Val i = 0;i <= 100;++i, oldVal = a){
            a.mutate(AddSquare<__Val>(i), NULL, NULL);

            realVal += i * i;
            ASSERT_EQ(realVal, a);
        }
    }
}

TEST(NAME, test_add)
{
    {
        __Int a;
        __Val oldVal = a;
        __Val realVal = 0;
        ASSERT_EQ(__Val(0), oldVal);
        for(__Val i = 0;i <= 100;++i, oldVal = a){
            __Val ov, nv;
            a.test_add(IsEven<__Val>(i), i, &ov, &nv);

            ASSERT_EQ(oldVal, ov);
            ASSERT_EQ(realVal, ov);
            if((realVal + i) % 2 == 0)
                realVal += i;
            ASSERT_EQ(realVal, nv);
            ASSERT_EQ(realVal, a);
        }
    }{
        __Int a;
        __Val oldVal = a;
        __Val realVal = 0;
        ASSERT_EQ(__Val(0), oldVal);
        for(__Val i = 0;i <= 100;++i, oldVal = a){
            __Val nv;
            a.test_add(IsEven<__Val>(i), i, NULL, &nv);

            if((realVal + i) % 2 == 0)
                realVal += i;
            ASSERT_EQ(realVal, nv);
            ASSERT_EQ(realVal, a);
        }
    }{
        __Int a;
        __Val oldVal = a;
        __Val realVal = 0;
        ASSERT_EQ(__Val(0), oldVal);
        for(__Val i = 0;i <= 100;++i, oldVal = a){
            __Val ov;
            a.test_add(IsEven<__Val>(i), i, &ov, NULL);

            ASSERT_EQ(oldVal, ov);
            ASSERT_EQ(realVal, ov);
            if((realVal + i) % 2 == 0)
                realVal += i;
            ASSERT_EQ(realVal, a);
        }
    }{
        __Int a;
        __Val oldVal = a;
        __Val realVal = 0;
        ASSERT_EQ(__Val(0), oldVal);
        for(__Val i = 0;i <= 100;++i, oldVal = a){
            a.test_add(IsEven<__Val>(i), i, NULL, NULL);

            if((realVal + i) % 2 == 0)
                realVal += i;
            ASSERT_EQ(realVal, a);
        }
    }
}

TEST(NAME, test_sub)
{
    {
        __Int a;
        __Val oldVal = a;
        __Val realVal = 0;
        ASSERT_EQ(__Val(0), oldVal);
        for(__Val i = 0;i <= 100;++i, oldVal = a){
            __Val ov, nv;
            a.test_sub(IsEven<__Val>(i), i, &ov, &nv);

            ASSERT_EQ(oldVal, ov);
            ASSERT_EQ(realVal, ov);
            if((realVal + i) % 2 == 0)
                realVal -= i;
            ASSERT_EQ(realVal, nv);
            ASSERT_EQ(realVal, a);
        }
    }{
        __Int a;
        __Val oldVal = a;
        __Val realVal = 0;
        ASSERT_EQ(__Val(0), oldVal);
        for(__Val i = 0;i <= 100;++i, oldVal = a){
            __Val nv;
            a.test_sub(IsEven<__Val>(i), i, NULL, &nv);

            if((realVal + i) % 2 == 0)
                realVal -= i;
            ASSERT_EQ(realVal, nv);
            ASSERT_EQ(realVal, a);
        }
    }{
        __Int a;
        __Val oldVal = a;
        __Val realVal = 0;
        ASSERT_EQ(__Val(0), oldVal);
        for(__Val i = 0;i <= 100;++i, oldVal = a){
            __Val ov;
            a.test_sub(IsEven<__Val>(i), i, &ov, NULL);

            ASSERT_EQ(oldVal, ov);
            ASSERT_EQ(realVal, ov);
            if((realVal + i) % 2 == 0)
                realVal -= i;
            ASSERT_EQ(realVal, a);
        }
    }{
        __Int a;
        __Val oldVal = a;
        __Val realVal = 0;
        ASSERT_EQ(__Val(0), oldVal);
        for(__Val i = 0;i <= 100;++i, oldVal = a){
            a.test_sub(IsEven<__Val>(i), i, NULL, NULL);

            if((realVal + i) % 2 == 0)
                realVal -= i;
            ASSERT_EQ(realVal, a);
        }
    }
}

TEST(NAME, test_and)
{
    {
        __Int a;
        __Val oldVal = a;
        __Val realVal = 0;
        ASSERT_EQ(__Val(0), oldVal);
        for(__Val i = 0;i <= 100;++i, oldVal = a){
            __Val ov, nv;
            a.test_and(IsEven<__Val>(i), i, &ov, &nv);

            ASSERT_EQ(oldVal, ov);
            ASSERT_EQ(realVal, ov);
            if((realVal + i) % 2 == 0)
                realVal &= i;
            ASSERT_EQ(realVal, nv);
            ASSERT_EQ(realVal, a);
        }
    }{
        __Int a;
        __Val oldVal = a;
        __Val realVal = 0;
        ASSERT_EQ(__Val(0), oldVal);
        for(__Val i = 0;i <= 100;++i, oldVal = a){
            __Val nv;
            a.test_and(IsEven<__Val>(i), i, NULL, &nv);

            if((realVal + i) % 2 == 0)
                realVal &= i;
            ASSERT_EQ(realVal, nv);
            ASSERT_EQ(realVal, a);
        }
    }{
        __Int a;
        __Val oldVal = a;
        __Val realVal = 0;
        ASSERT_EQ(__Val(0), oldVal);
        for(__Val i = 0;i <= 100;++i, oldVal = a){
            __Val ov;
            a.test_and(IsEven<__Val>(i), i, &ov, NULL);

            ASSERT_EQ(oldVal, ov);
            ASSERT_EQ(realVal, ov);
            if((realVal + i) % 2 == 0)
                realVal &= i;
            ASSERT_EQ(realVal, a);
        }
    }{
        __Int a;
        __Val oldVal = a;
        __Val realVal = 0;
        ASSERT_EQ(__Val(0), oldVal);
        for(__Val i = 0;i <= 100;++i, oldVal = a){
            a.test_and(IsEven<__Val>(i), i, NULL, NULL);

            if((realVal + i) % 2 == 0)
                realVal &= i;
            ASSERT_EQ(realVal, a);
        }
    }
}

TEST(NAME, test_or)
{
    {
        __Int a;
        __Val oldVal = a;
        __Val realVal = 0;
        ASSERT_EQ(__Val(0), oldVal);
        for(__Val i = 0;i <= 100;++i, oldVal = a){
            __Val ov, nv;
            a.test_or(IsEven<__Val>(i), i, &ov, &nv);

            ASSERT_EQ(oldVal, ov);
            ASSERT_EQ(realVal, ov);
            if((realVal + i) % 2 == 0)
                realVal |= i;
            ASSERT_EQ(realVal, nv);
            ASSERT_EQ(realVal, a);
        }
    }{
        __Int a;
        __Val oldVal = a;
        __Val realVal = 0;
        ASSERT_EQ(__Val(0), oldVal);
        for(__Val i = 0;i <= 100;++i, oldVal = a){
            __Val nv;
            a.test_or(IsEven<__Val>(i), i, NULL, &nv);

            if((realVal + i) % 2 == 0)
                realVal |= i;
            ASSERT_EQ(realVal, nv);
            ASSERT_EQ(realVal, a);
        }
    }{
        __Int a;
        __Val oldVal = a;
        __Val realVal = 0;
        ASSERT_EQ(__Val(0), oldVal);
        for(__Val i = 0;i <= 100;++i, oldVal = a){
            __Val ov;
            a.test_or(IsEven<__Val>(i), i, &ov, NULL);

            ASSERT_EQ(oldVal, ov);
            ASSERT_EQ(realVal, ov);
            if((realVal + i) % 2 == 0)
                realVal |= i;
            ASSERT_EQ(realVal, a);
        }
    }{
        __Int a;
        __Val oldVal = a;
        __Val realVal = 0;
        ASSERT_EQ(__Val(0), oldVal);
        for(__Val i = 0;i <= 100;++i, oldVal = a){
            a.test_or(IsEven<__Val>(i), i, NULL, NULL);

            if((realVal + i) % 2 == 0)
                realVal |= i;
            ASSERT_EQ(realVal, a);
        }
    }
}

TEST(NAME, test_xor)
{
    {
        __Int a;
        __Val oldVal = a;
        __Val realVal = 0;
        ASSERT_EQ(__Val(0), oldVal);
        for(__Val i = 0;i <= 100;++i, oldVal = a){
            __Val ov, nv;
            a.test_xor(IsEven<__Val>(i), i, &ov, &nv);

            ASSERT_EQ(oldVal, ov);
            ASSERT_EQ(realVal, ov);
            if((realVal + i) % 2 == 0)
                realVal ^= i;
            ASSERT_EQ(realVal, nv);
            ASSERT_EQ(realVal, a);
        }
    }{
        __Int a;
        __Val oldVal = a;
        __Val realVal = 0;
        ASSERT_EQ(__Val(0), oldVal);
        for(__Val i = 0;i <= 100;++i, oldVal = a){
            __Val nv;
            a.test_xor(IsEven<__Val>(i), i, NULL, &nv);

            if((realVal + i) % 2 == 0)
                realVal ^= i;
            ASSERT_EQ(realVal, nv);
            ASSERT_EQ(realVal, a);
        }
    }{
        __Int a;
        __Val oldVal = a;
        __Val realVal = 0;
        ASSERT_EQ(__Val(0), oldVal);
        for(__Val i = 0;i <= 100;++i, oldVal = a){
            __Val ov;
            a.test_xor(IsEven<__Val>(i), i, &ov, NULL);

            ASSERT_EQ(oldVal, ov);
            ASSERT_EQ(realVal, ov);
            if((realVal + i) % 2 == 0)
                realVal ^= i;
            ASSERT_EQ(realVal, a);
        }
    }{
        __Int a;
        __Val oldVal = a;
        __Val realVal = 0;
        ASSERT_EQ(__Val(0), oldVal);
        for(__Val i = 0;i <= 100;++i, oldVal = a){
            a.test_xor(IsEven<__Val>(i), i, NULL, NULL);

            if((realVal + i) % 2 == 0)
                realVal ^= i;
            ASSERT_EQ(realVal, a);
        }
    }
}

TEST(NAME, test_nand)
{
    {
        __Int a;
        __Val oldVal = a;
        __Val realVal = 0;
        ASSERT_EQ(__Val(0), oldVal);
        for(__Val i = 0;i <= 100;++i, oldVal = a){
            __Val ov, nv;
            a.test_nand(IsEven<__Val>(i), i, &ov, &nv);

            ASSERT_EQ(oldVal, ov);
            ASSERT_EQ(realVal, ov);
            if((realVal + i) % 2 == 0)
                realVal = ~(realVal & i);
            ASSERT_EQ(realVal, nv);
            ASSERT_EQ(realVal, a);
        }
    }{
        __Int a;
        __Val oldVal = a;
        __Val realVal = 0;
        ASSERT_EQ(__Val(0), oldVal);
        for(__Val i = 0;i <= 100;++i, oldVal = a){
            __Val nv;
            a.test_nand(IsEven<__Val>(i), i, NULL, &nv);

            if((realVal + i) % 2 == 0)
                realVal = ~(realVal & i);
            ASSERT_EQ(realVal, nv);
            ASSERT_EQ(realVal, a);
        }
    }{
        __Int a;
        __Val oldVal = a;
        __Val realVal = 0;
        ASSERT_EQ(__Val(0), oldVal);
        for(__Val i = 0;i <= 100;++i, oldVal = a){
            __Val ov;
            a.test_nand(IsEven<__Val>(i), i, &ov, NULL);

            ASSERT_EQ(oldVal, ov);
            ASSERT_EQ(realVal, ov);
            if((realVal + i) % 2 == 0)
                realVal = ~(realVal & i);
            ASSERT_EQ(realVal, a);
        }
    }{
        __Int a;
        __Val oldVal = a;
        __Val realVal = 0;
        ASSERT_EQ(__Val(0), oldVal);
        for(__Val i = 0;i <= 100;++i, oldVal = a){
            a.test_nand(IsEven<__Val>(i), i, NULL, NULL);

            if((realVal + i) % 2 == 0)
                realVal = ~(realVal & i);
            ASSERT_EQ(realVal, a);
        }
    }
}

