#include <marine/tools/debug.hh>

#include "../inc.h"

struct A
{
    std::string toString() const{
        return "This is A";
    }
};

TEST(DumpHex, v)
{
    ASSERT_EQ("(3)61 62 63", tools::DumpHex("abc", 3));
    ASSERT_EQ("(3)61 62 63", tools::DumpHex((const unsigned char *)"abc", 3));
    ASSERT_EQ("(3)61 62 63", tools::DumpHex((const signed char *)"abc", 3));

    std::string s("abc");
    std::vector<char> v(s.begin(), s.end());
    ASSERT_EQ("(3)61 62 63", tools::DumpHex(s));
    ASSERT_EQ("(3)61 62 63", tools::DumpHex(v));
    {
        CCharBuffer<const char> cb(s.c_str());
        ASSERT_EQ("(3)61 62 63", tools::DumpHex(cb));
    }{
        CCharBuffer<const unsigned char> cb((const unsigned char *)s.c_str());
        ASSERT_EQ("(3)61 62 63", tools::DumpHex(cb));
    }{
        CCharBuffer<const signed char> cb((const signed char *)s.c_str());
        ASSERT_EQ("(3)61 62 63", tools::DumpHex(cb));
    }{
        CCharBuffer<char> cb(&s[0]);
        ASSERT_EQ("(3)61 62 63", tools::DumpHex(cb));
    }{
        CCharBuffer<unsigned char> cb((unsigned char *)&s[0]);
        ASSERT_EQ("(3)61 62 63", tools::DumpHex(cb));
    }{
        CCharBuffer<signed char> cb((signed char *)&s[0]);
        ASSERT_EQ("(3)61 62 63", tools::DumpHex(cb));
    }
}

TEST(DumpHex, sep)
{
    ASSERT_EQ("(3)61.62.63", tools::DumpHex("abc", 3, '.'));
    ASSERT_EQ("(3)61.62.63", tools::DumpHex((const unsigned char *)"abc", 3, '.'));
    ASSERT_EQ("(3)61.62.63", tools::DumpHex((const signed char *)"abc", 3, '.'));

    std::string s("abc");
    std::vector<char> v(s.begin(), s.end());
    ASSERT_EQ("(3)61.62.63", tools::DumpHex(s, '.'));
    ASSERT_EQ("(3)61.62.63", tools::DumpHex(v, '.'));
}

TEST(DumpHex, hasLen)
{
    ASSERT_EQ("61.62.63", tools::DumpHex("abc", 3, '.', false));
    ASSERT_EQ("61.62.63", tools::DumpHex((const unsigned char *)"abc", 3, '.', false));
    ASSERT_EQ("61.62.63", tools::DumpHex((const signed char *)"abc", 3, '.', false));

    std::string s("abc");
    std::vector<char> v(s.begin(), s.end());
    ASSERT_EQ("61.62.63", tools::DumpHex(s, '.', false));
    ASSERT_EQ("61.62.63", tools::DumpHex(v, '.', false));
}

TEST(DumpStr, v)
{
    const char * SRC = "a\03\t\0bc";
    const char * RES = "(6)a.\\t\\0bc";
    ASSERT_EQ(RES, tools::DumpStr(SRC, 6));
    ASSERT_EQ(RES, tools::DumpStr((const unsigned char *)SRC, 6));
    ASSERT_EQ(RES, tools::DumpStr((const signed char *)SRC, 6));

    std::string s(SRC, 6);
    std::vector<char> v(s.begin(), s.end());
    ASSERT_EQ(RES, tools::DumpStr(s));
    ASSERT_EQ(RES, tools::DumpStr(v));
    {
        CCharBuffer<const char> cb(s.c_str(), s.length(), s.length());
        ASSERT_EQ(RES, tools::DumpStr(cb));
    }{
        CCharBuffer<const unsigned char> cb((const unsigned char *)s.c_str(), s.length(), s.length());
        ASSERT_EQ(RES, tools::DumpStr(cb));
    }{
        CCharBuffer<const signed char> cb((const signed char *)s.c_str(), s.length(), s.length());
        ASSERT_EQ(RES, tools::DumpStr(cb));
    }{
        CCharBuffer<char> cb(&s[0], s.length(), s.length());
        ASSERT_EQ(RES, tools::DumpStr(cb));
    }{
        CCharBuffer<unsigned char> cb((unsigned char *)&s[0], s.length(), s.length());
        ASSERT_EQ(RES, tools::DumpStr(cb));
    }{
        CCharBuffer<signed char> cb((signed char *)&s[0], s.length(), s.length());
        ASSERT_EQ(RES, tools::DumpStr(cb));
    }
}

TEST(DumpStr, replace)
{
    ASSERT_EQ("(6)a \\t\\0bc", tools::DumpStr("a\03\t\0bc", 6, ' '));
    ASSERT_EQ("(6)a \\t\\0bc", tools::DumpStr((const unsigned char *)"a\03\t\0bc", 6, ' '));
    ASSERT_EQ("(6)a \\t\\0bc", tools::DumpStr((const signed char *)"a\03\t\0bc", 6, ' '));

    std::string s("a\03\t\0bc", 6);
    std::vector<char> v(s.begin(), s.end());
    ASSERT_EQ("(6)a \\t\\0bc", tools::DumpStr(s, ' '));
    ASSERT_EQ("(6)a \\t\\0bc", tools::DumpStr(v, ' '));
}

TEST(DumpStr, hasLen)
{
    ASSERT_EQ("a \\t\\0bc", tools::DumpStr("a\03\t\0bc", 6, ' ', false));
    ASSERT_EQ("a \\t\\0bc", tools::DumpStr((const unsigned char *)"a\03\t\0bc", 6, ' ', false));
    ASSERT_EQ("a \\t\\0bc", tools::DumpStr((const signed char *)"a\03\t\0bc", 6, ' ', false));

    std::string s("a\03\t\0bc", 6);
    std::vector<char> v(s.begin(), s.end());
    ASSERT_EQ("a \\t\\0bc", tools::DumpStr(s, ' ', false));
    ASSERT_EQ("a \\t\\0bc", tools::DumpStr(v, ' ', false));
}

TEST(DumpVal, v)
{
    const char * SRC = "a\t\223bc";
    const char * RES = "(5)a\\t\\223bc";
    ASSERT_EQ(RES, tools::DumpVal(SRC, 5));
    ASSERT_EQ(RES, tools::DumpVal((const unsigned char *)SRC, 5));
    ASSERT_EQ(RES, tools::DumpVal((const signed char *)SRC, 5));

    std::string s(SRC, 5);
    std::vector<char> v(s.begin(), s.end());
    ASSERT_EQ(RES, tools::DumpVal(s));
    ASSERT_EQ(RES, tools::DumpVal(v));
    {
        CCharBuffer<const char> cb(s.c_str());
        ASSERT_EQ(RES, tools::DumpVal(cb));
    }{
        CCharBuffer<const unsigned char> cb((const unsigned char *)s.c_str());
        ASSERT_EQ(RES, tools::DumpVal(cb));
    }{
        CCharBuffer<const signed char> cb((const signed char *)s.c_str());
        ASSERT_EQ(RES, tools::DumpVal(cb));
    }{
        CCharBuffer<char> cb(&s[0]);
        ASSERT_EQ(RES, tools::DumpVal(cb));
    }{
        CCharBuffer<unsigned char> cb((unsigned char *)&s[0]);
        ASSERT_EQ(RES, tools::DumpVal(cb));
    }{
        CCharBuffer<signed char> cb((signed char *)&s[0]);
        ASSERT_EQ(RES, tools::DumpVal(cb));
    }
}

TEST(DumpVal, base)
{
    const size_t LEN = 5;
    const char * const STR = "a\t\223bc";
    const char * const RES = "(5)a\\t\\x93bc";

    ASSERT_EQ(RES, tools::DumpVal(STR, LEN, 16));
    ASSERT_EQ(RES, tools::DumpVal((const unsigned char *)STR, LEN, 16));
    ASSERT_EQ(RES, tools::DumpVal((const signed char *)STR, LEN, 16));

    std::string s(STR, LEN);
    std::vector<char> v(s.begin(), s.end());
    ASSERT_EQ(RES, tools::DumpVal(s, 16));
    ASSERT_EQ(RES, tools::DumpVal(v, 16));
}

TEST(DumpVal, hasLen)
{
    const size_t LEN = 5;
    const char * const STR = "a\t\223bc";
    const char * const RES = "a\\t\\x93bc";

    ASSERT_EQ(RES, tools::DumpVal(STR, LEN, 16, false));
    ASSERT_EQ(RES, tools::DumpVal((const unsigned char *)STR, LEN, 16, false));
    ASSERT_EQ(RES, tools::DumpVal((const signed char *)STR, LEN, 16, false));

    std::string s(STR, LEN);
    std::vector<char> v(s.begin(), s.end());
    ASSERT_EQ(RES, tools::DumpVal(s, 16, false));
    ASSERT_EQ(RES, tools::DumpVal(v, 16, false));
}

TEST(DumpXxd, v)
{
    const size_t LEN = 22;
    const char * const STR = "a\t\223bc3rr3vvdsaaba3adva";
    const char * const RES="(22)\n"
        "0000h: 61 09 93 62 63 33 72 72 33 76 76 64 73 61 61 62; a..bc3rr3vvdsaab\n"
        "0010h: 61 33 61 64 76 61                              ; a3adva\n";

    ASSERT_EQ(RES, tools::DumpXxd(STR, LEN));
    ASSERT_EQ(RES, tools::DumpXxd((const unsigned char *)STR, LEN));
    ASSERT_EQ(RES, tools::DumpXxd((const signed char *)STR, LEN));

    std::string s(STR, LEN);
    std::vector<char> v(s.begin(), s.end());
    ASSERT_EQ(RES, tools::DumpXxd(s));
    ASSERT_EQ(RES, tools::DumpXxd(v));
    {
        CCharBuffer<const char> cb(s.c_str());
        ASSERT_EQ(RES, tools::DumpXxd(cb));
    }{
        CCharBuffer<const unsigned char> cb((const unsigned char *)s.c_str());
        ASSERT_EQ(RES, tools::DumpXxd(cb));
    }{
        CCharBuffer<const signed char> cb((const signed char *)s.c_str());
        ASSERT_EQ(RES, tools::DumpXxd(cb));
    }{
        CCharBuffer<char> cb(&s[0]);
        ASSERT_EQ(RES, tools::DumpXxd(cb));
    }{
        CCharBuffer<unsigned char> cb((unsigned char *)&s[0]);
        ASSERT_EQ(RES, tools::DumpXxd(cb));
    }{
        CCharBuffer<signed char> cb((signed char *)&s[0]);
        ASSERT_EQ(RES, tools::DumpXxd(cb));
    }
}

TEST(Dump, v_str)
{
    const size_t LEN = 50;
    const char * const STR = "a392hb na9fahb32 9-ha bnbc3rr3vvdsaaba3adva3gadgda";
    const char * const RES = "(50)a392hb na9fahb32 9-ha bnbc3rr3vvdsaaba3adva3gadgda";

    ASSERT_EQ(RES, tools::Dump(STR, LEN));
    ASSERT_EQ(RES, tools::Dump((const unsigned char *)STR, LEN));
    ASSERT_EQ(RES, tools::Dump((const signed char *)STR, LEN));

    std::string s(STR, LEN);
    std::vector<char> v(s.begin(), s.end());
    ASSERT_EQ(RES, tools::Dump(s));
    ASSERT_EQ(RES, tools::Dump(v));
    {
        CCharBuffer<const char> cb(s.c_str());
        ASSERT_EQ(RES, tools::Dump(cb));
    }{
        CCharBuffer<const unsigned char> cb((const unsigned char *)s.c_str());
        ASSERT_EQ(RES, tools::Dump(cb));
    }{
        CCharBuffer<const signed char> cb((const signed char *)s.c_str());
        ASSERT_EQ(RES, tools::Dump(cb));
    }{
        CCharBuffer<char> cb(&s[0]);
        ASSERT_EQ(RES, tools::Dump(cb));
    }{
        CCharBuffer<unsigned char> cb((unsigned char *)&s[0]);
        ASSERT_EQ(RES, tools::Dump(cb));
    }{
        CCharBuffer<signed char> cb((signed char *)&s[0]);
        ASSERT_EQ(RES, tools::Dump(cb));
    }
}

TEST(Dump, v_xxd)
{
    const char STR[] = "\a392h\b na9\fahb32 9-ha b\nbc3\r\r3vvdsaaba3\ad\va\3gadgda2p[3nn[31bt1po3nr13rn";
    const char * const RES =
        "(72)\n"
        "0000h: 07 33 39 32 68 08 20 6E 61 39 0C 61 68 62 33 32; .392h. na9.ahb32\n"
        "0010h: 20 39 2D 68 61 20 62 0A 62 63 33 0D 0D 33 76 76;  9-ha b.bc3..3vv\n"
        "0020h: 64 73 61 61 62 61 33 07 64 0B 61 03 67 61 64 67; dsaaba3.d.a.gadg\n"
        "0030h: 64 61 32 70 5B 33 6E 6E 5B 33 31 62 74 31 70 6F; da2p[3nn[31bt1po\n"
        "0040h: 33 6E 72 31 33 72 6E 00                        ; 3nr13rn.\n";

    ASSERT_EQ(RES, tools::Dump(STR, sizeof STR));
    ASSERT_EQ(RES, tools::Dump((const unsigned char *)STR, sizeof STR));
    ASSERT_EQ(RES, tools::Dump((const signed char *)STR, sizeof STR));

    std::string s(STR, sizeof STR);
    std::vector<char> v(s.begin(), s.end());
    ASSERT_EQ(RES, tools::Dump(s));
    ASSERT_EQ(RES, tools::Dump(v));
    {
        CCharBuffer<const char> cb(s.c_str(), s.length(), s.length());
        ASSERT_EQ(RES, tools::Dump(cb));
    }{
        CCharBuffer<const unsigned char> cb((const unsigned char *)s.c_str(), s.length(), s.length());
        ASSERT_EQ(RES, tools::Dump(cb));
    }{
        CCharBuffer<const signed char> cb((const signed char *)s.c_str(), s.length(), s.length());
        ASSERT_EQ(RES, tools::Dump(cb));
    }{
        CCharBuffer<char> cb(&s[0], s.length(), s.length());
        ASSERT_EQ(RES, tools::Dump(cb));
    }{
        CCharBuffer<unsigned char> cb((unsigned char *)&s[0], s.length(), s.length());
        ASSERT_EQ(RES, tools::Dump(cb));
    }{
        CCharBuffer<signed char> cb((signed char *)&s[0], s.length(), s.length());
        ASSERT_EQ(RES, tools::Dump(cb));
    }
}

TEST(Dump, v_hex)
{
    const size_t LEN = 50;
    const char * const STR = "\a\3x\2h\b n\a\a\f\ah\b\3\2 \a-h\a \b\n\bc\3\r\r\3\v\vds\a\a\b\a\3\ad\v\a\3g\adgd\a";
    const char * const RES = "(50)07 03 78 02 68 08 20 6E 07 07 0C 07 68 08 03 02 20 07 2D 68 07 20 08 0A 08 63 03 0D 0D 03 0B 0B 64 73 07 07 08 07 03 07 64 0B 07 03 67 07 64 67 64 07";

    ASSERT_EQ(RES, tools::Dump(STR, LEN));
    ASSERT_EQ(RES, tools::Dump((const unsigned char *)STR, LEN));
    ASSERT_EQ(RES, tools::Dump((const signed char *)STR, LEN));

    std::string s(STR, LEN);
    std::vector<char> v(s.begin(), s.end());
    ASSERT_EQ(RES, tools::Dump(s));
    ASSERT_EQ(RES, tools::Dump(v));
    {
        CCharBuffer<const char> cb(s.c_str());
        ASSERT_EQ(RES, tools::Dump(cb));
    }{
        CCharBuffer<const unsigned char> cb((const unsigned char *)s.c_str());
        ASSERT_EQ(RES, tools::Dump(cb));
    }{
        CCharBuffer<const signed char> cb((const signed char *)s.c_str());
        ASSERT_EQ(RES, tools::Dump(cb));
    }{
        CCharBuffer<char> cb(&s[0]);
        ASSERT_EQ(RES, tools::Dump(cb));
    }{
        CCharBuffer<unsigned char> cb((unsigned char *)&s[0]);
        ASSERT_EQ(RES, tools::Dump(cb));
    }{
        CCharBuffer<signed char> cb((signed char *)&s[0]);
        ASSERT_EQ(RES, tools::Dump(cb));
    }
}

TEST(ToStringPtr, all)
{
    A * p = NULL;
    ASSERT_EQ("NULL", tools::ToStringPtr(p));

    A a;
    p = &a;
    ASSERT_EQ("This is A", tools::ToStringPtr(p));
}

TEST(ToStringBits, all)
{
    const char * const NAME[] = {
        "A",
        "B",
        "C",
        NULL,   //0x8
        "E",
        "F",
        "G",
        "H",
        "I",
        "J",
        "K"
    };
    uint32_t n = 1001;
    ASSERT_EQ("1001(A | F | G | H | I | J | 0x8)", tools::ToStringBits(n, NAME, sizeof NAME / sizeof NAME[0]));
    ASSERT_EQ("1001", tools::ToStringBits(n, NULL, 0));
}

TEST(UnHex, byte)
{
    const int VAL[] = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
        0xa, 0xb, 0xc, 0xd, 0xe, 0xf,
        0xa, 0xb, 0xc, 0xd, 0xe, 0xf
    };
    const char REP[] = {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
        'a', 'b', 'c', 'd', 'e', 'f',
        'A', 'B', 'C', 'D', 'E', 'F',
    };

    for(size_t i = 0;i < sizeof(VAL) / sizeof(VAL[0]);++i){
        ASSERT_EQ(VAL[i], tools::UnHex(REP[i]));
        ASSERT_EQ(VAL[i], tools::UnHex((signed char)REP[i]));
        ASSERT_EQ(VAL[i], tools::UnHex((unsigned char)REP[i]));
    }
    ASSERT_EQ(-1, tools::UnHex('x'));
    ASSERT_EQ(-1, tools::UnHex((signed char)'x'));
    ASSERT_EQ(-1, tools::UnHex((unsigned char)'x'));
}

TEST(UnHex, buf)
{
    const size_t LEN = 149;
    const char REP[] = "07 03 78 02 68 08 20 6E 07 07 0C 07 68 08 03 02 20 07 2D 68 07 20 08 0A 08 63 03 0D 0D 03 0B 0B 64 73 07 07 08 07 03 07 64 0B 07 03 67 07 64 67 64 07";
    const std::string VAL("\a\3x\2h\b n\a\a\f\ah\b\3\2 \a-h\a \b\n\bc\3\r\r\3\v\vds\a\a\b\a\3\ad\v\a\3g\adgd\a", 50);

    ASSERT_EQ(VAL, tools::UnHex(REP, LEN));
    ASSERT_EQ(VAL, tools::UnHex((signed char *)REP, LEN));
    ASSERT_EQ(VAL, tools::UnHex((unsigned char *)REP, LEN));

    std::string s(REP, LEN);
    std::vector<char> v(s.begin(), s.end());
    ASSERT_EQ(VAL, tools::UnHex(s));
    ASSERT_EQ(VAL, tools::UnHex(v));
}
