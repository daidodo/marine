#include <marine/compress_quicklz.hh>

#include "test.h"

TEST(CCompressorQuickLZ, __NAME)
{
    CCompressorQuickLZ comp;
    EXPECT_TRUE(testComp("CCompressorQuickLZ", comp));
}
