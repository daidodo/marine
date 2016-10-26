#include <marine/encrypt_aes.hh>

#include "test.h"

TEST(CEncryptorAes, all)
{
    CEncryptorAes comp;
    comp.setKey("abc");
    EXPECT_TRUE(testComp("CEncryptorAes", comp));

    comp.setIntensity(CEncryptorAes::L128);
    EXPECT_TRUE(testComp("CEncryptorAes", comp));

    comp.setIntensity(CEncryptorAes::L192);
    EXPECT_TRUE(testComp("CEncryptorAes", comp));

    comp.setKeyAndIntensity("gadhasdfk", CEncryptorAes::L256);
    EXPECT_TRUE(testComp("CEncryptorAes", comp));
}
