#include <marine/compress_snappy.hh>

#include "test.h"

TEST(CCompressorSnappy, all)
{
    CCompressorSnappy comp;
    EXPECT_TRUE(testComp("CCompressorSnappy", comp));
}
