#include <marine/tools/ssl.hh>

#include "../inc.h"

TEST(Md5, buf)
{
    const std::string e("\xD4\xA8\xAB\xCC\x94\xC5I\x14" "0yQ\x94\xF2\xA0" "a\xE8", 16);

    const char c[] = "-0bha3 s9-ban3sdfaup93rjqdJN:LKDJF_H#EN#DHBNdfa90hefn a30n";
    EXPECT_EQ(e, tools::Md5(c, sizeof c));

    std::string s(c, sizeof c);
    EXPECT_EQ(e, tools::Md5(s));

    std::vector<char> v(s.begin(), s.end());
    EXPECT_EQ(e, tools::Md5(v));
}

TEST(Md5, emtpy)
{
    const std::string e("\xD4\x1D\x8C\xD9\x8F\0\xB2\x4\xE9\x80\t\x98\xEC\xF8" "B~", 16);

    const char c[0] = {};
    EXPECT_EQ(e, tools::Md5(c, sizeof c));

    std::string s;
    EXPECT_EQ(e, tools::Md5(s));

    std::vector<char> v;
    EXPECT_EQ(e, tools::Md5(v));
}

