#include <marine/template.hh>

#include "../inc.h"
#include <stdint.h>

struct A
{
    int a;
    char b;
    short c;
};

TEST(CTypeTraits, POD)
{
    //kCanMemcpy
    EXPECT_TRUE( CTypeTraits<char>::kCanMemcpy == true );
    EXPECT_TRUE( CTypeTraits<signed char>::kCanMemcpy == true );
    EXPECT_TRUE( CTypeTraits<unsigned char>::kCanMemcpy == true );
    EXPECT_TRUE( CTypeTraits<short>::kCanMemcpy == true );
    EXPECT_TRUE( CTypeTraits<unsigned short>::kCanMemcpy == true );
    EXPECT_TRUE( CTypeTraits<int>::kCanMemcpy == true );
    EXPECT_TRUE( CTypeTraits<unsigned int>::kCanMemcpy == true );
    EXPECT_TRUE( CTypeTraits<long>::kCanMemcpy == true );
    EXPECT_TRUE( CTypeTraits<unsigned long>::kCanMemcpy == true );
    EXPECT_TRUE( CTypeTraits<long long>::kCanMemcpy == true );
    EXPECT_TRUE( CTypeTraits<unsigned long long>::kCanMemcpy == true );
    EXPECT_TRUE( CTypeTraits<wchar_t>::kCanMemcpy == true );
    EXPECT_TRUE( CTypeTraits<void *>::kCanMemcpy == true );
    EXPECT_TRUE( CTypeTraits<char *>::kCanMemcpy == true );
    EXPECT_TRUE( CTypeTraits<int *>::kCanMemcpy == true );
    EXPECT_TRUE( CTypeTraits<bool>::kCanMemcpy == true );

    EXPECT_TRUE( CTypeTraits<void>::kCanMemcpy == false );

    //kMaxBits
    EXPECT_TRUE( CTypeTraits<char>::kMaxBits == 8 );
    EXPECT_TRUE( CTypeTraits<signed char>::kMaxBits == 8 );
    EXPECT_TRUE( CTypeTraits<unsigned char>::kMaxBits == 8 );
    EXPECT_TRUE( CTypeTraits<short>::kMaxBits == 16 );
    EXPECT_TRUE( CTypeTraits<unsigned short>::kMaxBits == 16 );
    EXPECT_TRUE( CTypeTraits<int>::kMaxBits == 32 );
    EXPECT_TRUE( CTypeTraits<unsigned int>::kMaxBits == 32 );
    EXPECT_TRUE( CTypeTraits<long>::kMaxBits == sizeof(long) * 8 );
    EXPECT_TRUE( CTypeTraits<unsigned long>::kMaxBits == sizeof(long) * 8 );
    EXPECT_TRUE( CTypeTraits<long long>::kMaxBits == sizeof(long long) * 8 );
    EXPECT_TRUE( CTypeTraits<unsigned long long>::kMaxBits == sizeof(long long) * 8 );
    EXPECT_TRUE( CTypeTraits<wchar_t>::kMaxBits == sizeof(wchar_t) * 8 );
    EXPECT_TRUE( CTypeTraits<void *>::kMaxBits == sizeof(void *) * 8 );
    EXPECT_TRUE( CTypeTraits<char *>::kMaxBits == sizeof(void *) * 8 );
    EXPECT_TRUE( CTypeTraits<int *>::kMaxBits == sizeof(void *) * 8 );
}

TEST(CTypeTraits, A)
{
    EXPECT_TRUE( CTypeTraits<A>::kCanMemcpy == false );
}

TEST(CByteOrderTraits, 1)
{
    EXPECT_TRUE( ('a' == CByteOrderTraits<uint8_t, 1>::swap('a')) );
    EXPECT_TRUE( ('a' == CByteOrderTraits<int8_t, 1>::swap('a')) );
}

TEST(CByteOrderTraits, 2)
{
    EXPECT_TRUE( (uint16_t(0xABCD) == CByteOrderTraits<uint16_t, 2>::swap(uint16_t(0xCDAB))) );
    EXPECT_TRUE( (int16_t(0xABCD) == CByteOrderTraits<int16_t, 2>::swap(int16_t(0xCDAB))) );
}

TEST(CByteOrderTraits, 4)
{
    EXPECT_TRUE( (uint32_t(0x1234ABCD) == CByteOrderTraits<uint32_t, 4>::swap(uint32_t(0xCDAB3412))) );
    EXPECT_TRUE( (int32_t(0x1234ABCD) == CByteOrderTraits<int32_t, 4>::swap(int32_t(0xCDAB3412))) );
}

TEST(CByteOrderTraits, 8)
{
    EXPECT_TRUE( (uint64_t(0x1234567890ABCDEFull) == CByteOrderTraits<uint64_t, 8>::swap(uint64_t(0xEFCDAB9078563412ull))));
    EXPECT_TRUE( (int64_t(0x1234567890ABCDEFull) == CByteOrderTraits<int64_t, 8>::swap(int64_t(0xEFCDAB9078563412ull))) );
}

TEST(ARRAY_SIZE, all)
{
    int i[5];
    EXPECT_TRUE( 5 == ARRAY_SIZE(i) );
    //EXPECT_TRUE( 1 == ARRAY_SIZE(&i[0]) );    //should compile error

    A a[ARRAY_SIZE(i)];
    EXPECT_TRUE( 5 == ARRAY_SIZE(a) );
    //EXPECT_TRUE( 1 == ARRAY_SIZE(&a[0]) );      //should compile error

    const int ci[5] = {};
    EXPECT_TRUE( 5 == ARRAY_SIZE(ci) );
    //EXPECT_TRUE( 1 == ARRAY_SIZE(&ci[0]) );    //should compile error

    const A ca[ARRAY_SIZE(ci)] = {};
    EXPECT_TRUE( 5 == ARRAY_SIZE(ca) );
    //EXPECT_TRUE( 1 == ARRAY_SIZE(&ca[0]) );      //should compile error

    int i0[0];
    EXPECT_TRUE( 0 == ARRAY_SIZE(i0) );
    //EXPECT_TRUE( 1 == ARRAY_SIZE(&i0[0]) );    //should compile error

    A a0[ARRAY_SIZE(i0)];
    EXPECT_TRUE( 0 == ARRAY_SIZE(a0) );
    //EXPECT_TRUE( 1 == ARRAY_SIZE(&a0[0]) );      //should compile error

    const int ci0[0] = {};
    EXPECT_TRUE( 0 == ARRAY_SIZE(ci0) );
    //EXPECT_TRUE( 1 == ARRAY_SIZE(&ci0[0]) );    //should compile error

    const A ca0[ARRAY_SIZE(ci0)] = {};
    EXPECT_TRUE( 0 == ARRAY_SIZE(ca0) );
    //EXPECT_TRUE( 1 == ARRAY_SIZE(&ca0[0]) );      //should compile error
}

TEST(Compile, all)
{
    STATIC_ASSERT(1 == 1);
    //STATIC_ASSERT(0 == 1);    //compile error

    CHECK_TYPE_SIZE(A, 8);
    //CHECK_TYPE_SIZE(A, 4);    //compile error

    CHECK_MEM_OFFSET(A, a, 0);
    CHECK_MEM_OFFSET(A, b, 4);
    CHECK_MEM_OFFSET(A, c, 6);
}

/*
TEST(CSameType, POD)
{
    //int
    EXPECT_TRUE( (CSameType<int, int>::kResult == true) );
    EXPECT_TRUE( (CSameType<const int, const int>::kResult == true) );
    EXPECT_TRUE( (CSameType<volatile int, volatile int>::kResult == true) );
    EXPECT_TRUE( (CSameType<const volatile int, const volatile int>::kResult == true) );

    EXPECT_TRUE( (CSameType<int, const int>::kResult == false) );
    EXPECT_TRUE( (CSameType<int, volatile int>::kResult == false) );
    EXPECT_TRUE( (CSameType<int, const volatile int>::kResult == false) );
    EXPECT_TRUE( (CSameType<const int, volatile int>::kResult == false) );
    EXPECT_TRUE( (CSameType<const int, const volatile int>::kResult == false) );
    EXPECT_TRUE( (CSameType<volatile int, const volatile int>::kResult == false) );

    EXPECT_TRUE( (CSameType<int, char>::kResult == false) );
    EXPECT_TRUE( (CSameType<int, void>::kResult == false) );

    //void
    EXPECT_TRUE( (CSameType<void, void>::kResult == true) );
    EXPECT_TRUE( (CSameType<const void, const void>::kResult == true) );
    EXPECT_TRUE( (CSameType<volatile void, volatile void>::kResult == true) );
    EXPECT_TRUE( (CSameType<const volatile void, const volatile void>::kResult == true) );

    EXPECT_TRUE( (CSameType<void, const void>::kResult == false) );
    EXPECT_TRUE( (CSameType<void, volatile void>::kResult == false) );
    EXPECT_TRUE( (CSameType<void, const volatile void>::kResult == false) );
    EXPECT_TRUE( (CSameType<const void, volatile void>::kResult == false) );
    EXPECT_TRUE( (CSameType<const void, const volatile void>::kResult == false) );
    EXPECT_TRUE( (CSameType<volatile void, const volatile void>::kResult == false) );
}

TEST(CSameType, A)
{
    EXPECT_TRUE( (CSameType<A, A>::kResult == true) );
    EXPECT_TRUE( (CSameType<const A, const A>::kResult == true) );
    EXPECT_TRUE( (CSameType<volatile A, volatile A>::kResult == true) );
    EXPECT_TRUE( (CSameType<const volatile A, const volatile A>::kResult == true) );

    EXPECT_TRUE( (CSameType<A, const A>::kResult == false) );
    EXPECT_TRUE( (CSameType<A, volatile A>::kResult == false) );
    EXPECT_TRUE( (CSameType<A, const volatile A>::kResult == false) );
    EXPECT_TRUE( (CSameType<const A, volatile A>::kResult == false) );
    EXPECT_TRUE( (CSameType<const A, const volatile A>::kResult == false) );
    EXPECT_TRUE( (CSameType<volatile A, const volatile A>::kResult == false) );

    EXPECT_TRUE( (CSameType<A, char>::kResult == false) );
    EXPECT_TRUE( (CSameType<A, void>::kResult == false) );
}
*/
