#include <marine/fd_map.hh>

#include "../inc.h"

typedef CFdMap<int> __FdMap;

TEST(CFdMap, all)
{
    __FdMap m1;
    ASSERT_EQ(100, (int)m1.capacity());

    __FdMap m2(200);
    ASSERT_EQ(200, (int)m2.capacity());

    m1.capacity(300);
    ASSERT_EQ(300, (int)m1.capacity());

    for(int i = 0;i < 400;++i)
        m1[i] = i * i;

    const __FdMap & m3 = m1;
    for(int i = 0;i < 400;++i)
        ASSERT_EQ(i * i, m3[i]);

    for(int i = 400;i < 500;++i)
        ASSERT_EQ(0, m3[i]);
}
