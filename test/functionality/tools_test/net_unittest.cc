#include <marine/tools/net.hh>

#include "../inc.h"
#include <arpa/inet.h>    //htonl

const bool NOT_NB = (1 != htonl(1));

TEST(HostByteOrder, all)
{
    ASSERT_EQ(NOT_NB, tools::HostByteOrder())<<"BYTE_ORDER="<<BYTE_ORDER<<endl;
}

TEST(SwapByteOrder, all)
{
    ASSERT_EQ('a', tools::SwapByteOrder(int8_t('a')));
    ASSERT_EQ('a', tools::SwapByteOrder(uint8_t('a')));
    ASSERT_EQ(uint16_t(0xABCD), tools::SwapByteOrder(uint16_t(0xCDAB)));
    ASSERT_EQ(int16_t(0xABCD), tools::SwapByteOrder(int16_t(0xCDAB)));
    ASSERT_EQ(uint32_t(0x1234ABCD), tools::SwapByteOrder(uint32_t(0xCDAB3412)));
    ASSERT_EQ(int32_t(0x1234ABCD), tools::SwapByteOrder(int32_t(0xCDAB3412)));
    ASSERT_EQ(uint64_t(0x1234567890ABCDEFull), tools::SwapByteOrder(uint64_t(0xEFCDAB9078563412ull)));
    ASSERT_EQ(int64_t(0x1234567890ABCDEFull), tools::SwapByteOrder(int64_t(0xEFCDAB9078563412ull)));
}

TEST(Hton, all)
{
    ASSERT_EQ('a', tools::Hton(int8_t('a')));
    ASSERT_EQ('a', tools::Hton(uint8_t('a')));
    ASSERT_EQ(uint16_t(htons(0xABCD)), tools::Hton(uint16_t(0xABCD)));
    ASSERT_EQ(int16_t(htons(0xABCD)), tools::Hton(int16_t(0xABCD)));
    ASSERT_EQ(uint32_t(htonl(0x1234ABCD)), tools::Hton(uint32_t(0x1234ABCD)));
    ASSERT_EQ(int32_t(htonl(0x1234ABCD)), tools::Hton(int32_t(0x1234ABCD)));
    if(NOT_NB){
        ASSERT_EQ(uint64_t(0x1234567890ABCDEFull), tools::Hton(uint64_t(0xEFCDAB9078563412ull)));
        ASSERT_EQ(int64_t(0x1234567890ABCDEFull), tools::Hton(int64_t(0xEFCDAB9078563412ull)));
    }else{
        ASSERT_EQ(uint64_t(0x1234567890ABCDEFull), tools::Hton(uint64_t(0x1234567890ABCDEFull)));
        ASSERT_EQ(int64_t(0x1234567890ABCDEFull), tools::Hton(int64_t(0x1234567890ABCDEFull)));
    }
}

TEST(Ntoh, all)
{
    ASSERT_EQ('a', tools::Ntoh(int8_t('a')));
    ASSERT_EQ('a', tools::Ntoh(uint8_t('a')));
    ASSERT_EQ(uint16_t(ntohs(0xABCD)), tools::Ntoh(uint16_t(0xABCD)));
    ASSERT_EQ(int16_t(ntohs(0xABCD)), tools::Ntoh(int16_t(0xABCD)));
    ASSERT_EQ(uint32_t(ntohl(0x1234ABCD)), tools::Ntoh(uint32_t(0x1234ABCD)));
    ASSERT_EQ(int32_t(ntohl(0x1234ABCD)), tools::Ntoh(int32_t(0x1234ABCD)));
    if(NOT_NB){
        ASSERT_EQ(uint64_t(0x1234567890ABCDEFull), tools::Ntoh(uint64_t(0xEFCDAB9078563412ull)));
        ASSERT_EQ(int64_t(0x1234567890ABCDEFull), tools::Ntoh(int64_t(0xEFCDAB9078563412ull)));
    }else{
        ASSERT_EQ(uint64_t(0x1234567890ABCDEFull), tools::Ntoh(uint64_t(0x1234567890ABCDEFull)));
        ASSERT_EQ(int64_t(0x1234567890ABCDEFull), tools::Ntoh(int64_t(0x1234567890ABCDEFull)));
    }
}
