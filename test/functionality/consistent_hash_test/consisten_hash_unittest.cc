#include <marine/consistent_hash.hh>

#include "../inc.h"

typedef CConsistentHash<uint64_t> __Hash;

TEST(CConsistentHash, all)
{
    __Hash h;
    h.setValue(100);
    ASSERT_EQ(100u, h.hash(12345));

    h.setValue(200);
    h.setValue(100, 0);
    ASSERT_EQ(200u, h.hash(12345));

    for(int i = 0;i < 200;++i)
        h.setValue(i, 100);
    h.setValue(200, 0);
    for(int i = 12345;i < 23456;++i){
        const size_t n = h.hash(i);
        ASSERT_TRUE(0 <= n && n < 200);
    }
}
