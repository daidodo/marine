#include <marine/compress_lzo.hh>

#include "test.h"

TEST(CCompressorLzo, all)
{
    CCompressorLzo comp;
    EXPECT_TRUE(testComp("CCompressorLzo", comp));
}
