#include <marine/charset_convert.hh>

#include "../inc.h"

#include <fstream>
#include <marine/tools/debug.hh>

inline void toFile(const std::string & s, const char * fname)
{
    assert(fname);
    std::ofstream of(fname, ios_base::out | ios_base::binary | ios_base::trunc);
    of.write(s.c_str(), s.size());
}

TEST(CCharsetConvert, init)
{
    {
        CCharsetConvert cv;
        ASSERT_FALSE(cv.valid());
        ASSERT_EQ("{cv_=0xffffffffffffffff, from_=, to_=, mode_=0}", cv.toString());
    }{
        CCharsetConvert cv("GBK", "UTF-8");
        ASSERT_TRUE(cv.valid());

        ASSERT_EQ("GBK", cv.fromCode());
        ASSERT_EQ("UTF-8", cv.toCode());
        ASSERT_EQ(CCharsetConvert::kNormal, cv.mode());
    }{
        CCharsetConvert cv("GBK", "UTF-8", CCharsetConvert::kTranslit);
        ASSERT_TRUE(cv.valid());

        ASSERT_EQ("GBK", cv.fromCode());
        ASSERT_EQ("UTF-8", cv.toCode());
        ASSERT_EQ(CCharsetConvert::kTranslit, cv.mode());
    }{
        CCharsetConvert cv;
        ASSERT_TRUE(cv.init("GBK", "UTF-8"));
        ASSERT_TRUE(cv.valid());

        ASSERT_EQ("GBK", cv.fromCode());
        ASSERT_EQ("UTF-8", cv.toCode());
        ASSERT_EQ(CCharsetConvert::kNormal, cv.mode());
    }{
        CCharsetConvert cv;
        ASSERT_TRUE(cv.init("GBK", "UTF-8", CCharsetConvert::kIgnore));
        ASSERT_TRUE(cv.valid());

        ASSERT_EQ("GBK", cv.fromCode());
        ASSERT_EQ("UTF-8", cv.toCode());
        ASSERT_EQ(CCharsetConvert::kIgnore, cv.mode());
    }
}

TEST(CCharsetConvert, convert_gbk_utf8)
{
    {
        const char * C1 = "GBK";
        const char * C2 = "UTF-8";
        {
            CCharsetConvert::EMode mode = CCharsetConvert::kNormal;
            const char hex1[] = "D5E2 CAC7 D2BB B6CE D6D0 CEC4 BABA D7D6";
            const char hex2[] = "E8BF 99E6 98AF E4B8 80E6 AEB5 E4B8 ADE6 9687 E6B1 89E5 AD97";
            const std::string s1 = tools::UnHex(hex1, sizeof hex1);
            const std::string s2 = tools::UnHex(hex2, sizeof hex2);
            {
                CCharsetConvert cv(C1, C2, mode);
                ASSERT_TRUE(cv.valid());

                std::string d;
                ASSERT_TRUE(cv.convert(s1, d));
                ASSERT_EQ(s2, d);
            }{
                CCharsetConvert cv(C2, C1, mode);
                ASSERT_TRUE(cv.valid());

                std::string d;
                ASSERT_TRUE(cv.convert(s2, d));
                ASSERT_EQ(s1, d);
            }
        }{
            CCharsetConvert::EMode mode = CCharsetConvert::kTranslit;
            const char hex1[] = "C1ED CDE2 D2BB B6CE 456E 676C 6973 68D6 D0CE C4";
            const char hex2[] = "E58F A6E5 A496 E4B8 80E6 AEB5 456E 676C 6973 68E4 B8AD E696 87";
            const std::string s1 = tools::UnHex(hex1, sizeof hex1);
            const std::string s2 = tools::UnHex(hex2, sizeof hex2);
            {
                CCharsetConvert cv(C1, C2, mode);
                ASSERT_TRUE(cv.valid());

                std::string d;
                ASSERT_TRUE(cv.convert(s1, d));
                ASSERT_EQ(s2, d);
            }{
                CCharsetConvert cv(C2, C1, mode);
                ASSERT_TRUE(cv.valid());

                std::string d;
                ASSERT_TRUE(cv.convert(s2, d));
                ASSERT_EQ(s1, d);
            }
        }{
            CCharsetConvert::EMode mode = CCharsetConvert::kIgnore;
            const char hex1[] = "D0E8 D2AA B5DA 33B8 F6C0 FDD7 D3";
            const char hex2[] = "E99C 80E8 A681 E7AC AC33 E4B8 AAE4 BE8B E5AD 90";
            const std::string s1 = tools::UnHex(hex1, sizeof hex1);
            const std::string s2 = tools::UnHex(hex2, sizeof hex2);
            {
                CCharsetConvert cv(C1, C2, mode);
                ASSERT_TRUE(cv.valid());

                std::string d;
                ASSERT_TRUE(cv.convert(s1, d));
                ASSERT_EQ(s2, d);
            }{
                CCharsetConvert cv(C2, C1, mode);
                ASSERT_TRUE(cv.valid());

                std::string d;
                ASSERT_TRUE(cv.convert(s2, d));
                ASSERT_EQ(s1, d);
            }
        }
    }
}

TEST(CCharsetConvert, convert_gbk_unicode)
{
    {
        const char * C1 = "GBK";
        const char * C2 = "UNICODE";
        {
            CCharsetConvert::EMode mode = CCharsetConvert::kNormal;
            const char hex1[] = "33C4 C4C5 C2B5 C4A3 BBB0 A2B2 BCB0 A2B5 C26E ";
            const char hex2[] = "fffe 3300 EA54 1560 8476 1BFF 3F96 035E 3F96 B75F 6E00 ";
            const std::string s1 = tools::UnHex(hex1, sizeof hex1);
            const std::string s2 = tools::UnHex(hex2, sizeof hex2);
            {
                CCharsetConvert cv(C1, C2, mode);
                ASSERT_TRUE(cv.valid());

                std::string d;
                ASSERT_TRUE(cv.convert(s1, d));
                ASSERT_EQ(s2, d);
            }{
                CCharsetConvert cv(C2, C1, mode);
                ASSERT_TRUE(cv.valid());

                std::string d;
                ASSERT_TRUE(cv.convert(s2, d));
                ASSERT_EQ(s1, d);
            }
        }{
            CCharsetConvert::EMode mode = CCharsetConvert::kTranslit;
            const char hex1[] = "33C4 E3A3 BB33 B0A1 A3BB C4E3 B0A1 B8BE C5AE ";
            const char hex2[] = "fffe 3300 604F 1BFF 3300 4A55 1BFF 604F 4A55 8759 7359 ";
            const std::string s1 = tools::UnHex(hex1, sizeof hex1);
            const std::string s2 = tools::UnHex(hex2, sizeof hex2);
            {
                CCharsetConvert cv(C1, C2, mode);
                ASSERT_TRUE(cv.valid());

                std::string d;
                ASSERT_TRUE(cv.convert(s1, d));
                ASSERT_EQ(s2, d);
            }{
                CCharsetConvert cv(C2, C1, mode);
                ASSERT_TRUE(cv.valid());

                std::string d;
                ASSERT_TRUE(cv.convert(s2, d));
                ASSERT_EQ(s1, d);
            }
        }{
            CCharsetConvert::EMode mode = CCharsetConvert::kIgnore;
            const char hex1[] = "B5C4 B8C2 C4D8 A3BB C1BD C4EA B0A1 A3BB CBAE B6E0 B8DF B0A2 CBB9 ";
            const char hex2[] = "fffe 8476 0E56 6254 1BFF 244E 745E 4A55 1BFF 346C 1A59 D89A 3F96 AF65 ";
            const std::string s1 = tools::UnHex(hex1, sizeof hex1);
            const std::string s2 = tools::UnHex(hex2, sizeof hex2);
            {
                CCharsetConvert cv(C1, C2, mode);
                ASSERT_TRUE(cv.valid());

                std::string d;
                ASSERT_TRUE(cv.convert(s1, d));
                ASSERT_EQ(s2, d);
            }{
                CCharsetConvert cv(C2, C1, mode);
                ASSERT_TRUE(cv.valid());

                std::string d;
                ASSERT_TRUE(cv.convert(s2, d));
                ASSERT_EQ(s1, d);
            }
        }
    }
}

TEST(CCharsetConvert, convert_utf8_unicode)
{
    {
        const char * C1 = "UTF-8";
        const char * C2 = "UNICODE";
        {
            CCharsetConvert::EMode mode = CCharsetConvert::kNormal;
            const char hex1[] = "E79A 84E5 988E E591 A2EF BC9B E4B8 A4E5 B9B4 E595 8AEF BC9B E6B0 B4E5 A49A E9AB 98E9 98BF E696 AF";
            const char hex2[] = "fffe 8476 0E56 6254 1BFF 244E 745E 4A55 1BFF 346C 1A59 D89A 3F96 AF65 ";
            const std::string s1 = tools::UnHex(hex1, sizeof hex1);
            const std::string s2 = tools::UnHex(hex2, sizeof hex2);
            {
                CCharsetConvert cv(C1, C2, mode);
                ASSERT_TRUE(cv.valid());

                std::string d;
                ASSERT_TRUE(cv.convert(s1, d));
                ASSERT_EQ(s2, d);
            }{
                CCharsetConvert cv(C2, C1, mode);
                ASSERT_TRUE(cv.valid());

                std::string d;
                ASSERT_TRUE(cv.convert(s2, d));
                ASSERT_EQ(s1, d);
            }
        }{
            CCharsetConvert::EMode mode = CCharsetConvert::kTranslit;
            const char hex1[] = "E5A4 A7E6 A682 E681 A9E7 9A84 E598 8EE7 9A84 E598 8E";
            const char hex2[] = "fffe 2759 8269 6960 8476 0E56 8476 0E56 ";
            const std::string s1 = tools::UnHex(hex1, sizeof hex1);
            const std::string s2 = tools::UnHex(hex2, sizeof hex2);
            {
                CCharsetConvert cv(C1, C2, mode);
                ASSERT_TRUE(cv.valid());

                std::string d;
                ASSERT_TRUE(cv.convert(s1, d));
                ASSERT_EQ(s2, d);
            }{
                CCharsetConvert cv(C2, C1, mode);
                ASSERT_TRUE(cv.valid());

                std::string d;
                ASSERT_TRUE(cv.convert(s2, d));
                ASSERT_EQ(s1, d);
            }
        }{
            CCharsetConvert::EMode mode = CCharsetConvert::kIgnore;
            const char hex1[] = "E8BF 87E5 B9B4 E5A4 A7EF BC9B E789 9BE8 82A1 E595 8AE7 9A84 E598 8E";
            const char hex2[] = "fffe C78F 745E 2759 1BFF 5B72 A180 4A55 8476 0E56 ";
            const std::string s1 = tools::UnHex(hex1, sizeof hex1);
            const std::string s2 = tools::UnHex(hex2, sizeof hex2);
            {
                CCharsetConvert cv(C1, C2, mode);
                ASSERT_TRUE(cv.valid());

                std::string d;
                ASSERT_TRUE(cv.convert(s1, d));
                ASSERT_EQ(s2, d);
            }{
                CCharsetConvert cv(C2, C1, mode);
                ASSERT_TRUE(cv.valid());

                std::string d;
                ASSERT_TRUE(cv.convert(s2, d));
                ASSERT_EQ(s1, d);
            }
        }
    }
}
