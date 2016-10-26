#include <marine/compress_zlib.hh>

#include "test.h"

TEST(CCompressorZlib, all)
{
    CCompressorZlib comp;
    ASSERT_TRUE(testComp("CCompressorZlib", comp));

    for(int i = 1;i <= 4;++i){
        comp.setLevel(i);
        ASSERT_EQ(i, comp.getLevel());
        ASSERT_TRUE(testComp("CCompressorZlib", comp));
    }
}
