#include <marine/tools/string.hh>

#include <fstream>
#include <marine/charset_convert.hh>
#include "../inc.h"

TEST(Trim, all)
{
    ASSERT_EQ("\b\n55612 dg\tgg  \t\b", tools::Trim("   \n\t\b\n55612 dg\tgg  \t\b\n  \t"));
    ASSERT_EQ("", tools::Trim("   \n\t\n  \t"));
    ASSERT_EQ("", tools::Trim(""));
    ASSERT_EQ("", tools::Trim(std::string()));
}

TEST(StringMatch, all)
{
    const char SRC[] = "bakln;enafblenipna3e";
    const char TAR[] = "len";
    ASSERT_EQ(11, tools::StringMatch(SRC, sizeof SRC - 1, TAR, sizeof TAR - 1));
    ASSERT_EQ(-1, tools::StringMatch(SRC, sizeof SRC - 1, "adkg", 4));

    ASSERT_EQ(11, tools::StringMatchKmp(SRC, sizeof SRC - 1, TAR, sizeof TAR - 1));
    ASSERT_EQ(-1, tools::StringMatchKmp(SRC, sizeof SRC - 1, "adkg", 4));
}

TEST(AbsFilename, all)
{
    ASSERT_EQ("/tmp/1", tools::AbsFilename(std::string("/tmp/1")));
    ASSERT_NE("../abc.conf", tools::AbsFilename(std::string("../abc.conf")));

    ASSERT_EQ("/tmp/1", tools::AbsFilename("/tmp/1"));
    ASSERT_NE("../abc.conf", tools::AbsFilename("../abc.conf"));
}

TEST(Basename, all)
{
    ASSERT_EQ("1.txt", tools::Basename(std::string("/usr/local/bin/1.txt")));
    ASSERT_EQ("1.txt", tools::Basename(std::string("1.txt")));
    ASSERT_EQ("1.txt", tools::Basename(std::string("../conf/1.txt")));
    ASSERT_EQ("", tools::Basename(std::string("")));
    ASSERT_EQ("", tools::Basename(std::string()));

    ASSERT_STREQ("1.txt", tools::Basename("/usr/local/bin/1.txt"));
    ASSERT_STREQ("1.txt", tools::Basename("1.txt"));
    ASSERT_STREQ("1.txt", tools::Basename("../conf/1.txt"));
    ASSERT_STREQ("", tools::Basename(""));
    ASSERT_EQ(NULL, tools::Basename(NULL));
}

TEST(FilePrimeName, all)
{
    ASSERT_EQ("top good_1.t.a.b", tools::FilePrimeName(std::string("/usr/local/bin/top good_1.t.a.b.txt")));
    ASSERT_EQ("top good_1.t.a.b", tools::FilePrimeName(std::string("top good_1.t.a.b.txt")));
    ASSERT_EQ("top good_1.t.a.b", tools::FilePrimeName(std::string("./top good_1.t.a.b.txt")));
    ASSERT_EQ("top good_1.t.a.b", tools::FilePrimeName(std::string("../conf/top good_1.t.a.b.txt")));
    ASSERT_EQ("top good_1", tools::FilePrimeName(std::string("/usr/local/bin/top good_1")));
    ASSERT_EQ("top good_1", tools::FilePrimeName(std::string("top good_1")));
    ASSERT_EQ("top good_1", tools::FilePrimeName(std::string("./top good_1")));
    ASSERT_EQ("top good_1", tools::FilePrimeName(std::string("../conf/top good_1")));
    ASSERT_EQ("", tools::FilePrimeName(std::string("")));
    ASSERT_EQ("", tools::FilePrimeName(std::string()));
}

TEST(FileType, all)
{
    ASSERT_EQ("top good_1", tools::FileType(std::string("/usr/local/bin/top good_1.t.a.b.top good_1")));
    ASSERT_EQ("top good_1", tools::FileType(std::string("top good_1.t.a.b.top good_1")));
    ASSERT_EQ("top good_1", tools::FileType(std::string("./top good_1.t.a.b.top good_1")));
    ASSERT_EQ("top good_1", tools::FileType(std::string("../conf/top good_1.t.a.b.top good_1")));
    ASSERT_EQ("", tools::FileType(std::string("/usr/local/bin/top good_1")));
    ASSERT_EQ("", tools::FileType(std::string("top good_1")));
    ASSERT_EQ("", tools::FileType(std::string("./top good_1")));
    ASSERT_EQ("", tools::FileType(std::string("../conf/top good_1")));
    ASSERT_EQ("", tools::FileType(std::string("")));
    ASSERT_EQ("", tools::FileType(std::string()));

    ASSERT_STREQ("top good_1", tools::FileType("/usr/local/bin/tsop good_1.t.a.b.top good_1"));
    ASSERT_STREQ("top good_1", tools::FileType("top gosod_1.t.a.b.top good_1"));
    ASSERT_STREQ("top good_1", tools::FileType("./top ood_1.t.a.b.top good_1"));
    ASSERT_STREQ("top good_1", tools::FileType("../conf/top god_1.t.a.b.top good_1"));
    ASSERT_STREQ(NULL, tools::FileType("/usr/local/bin/top good_1"));
    ASSERT_STREQ(NULL, tools::FileType("top good_1"));
    ASSERT_STREQ(NULL, tools::FileType("./top good_1"));
    ASSERT_STREQ(NULL, tools::FileType("../conf/top good_1"));
    ASSERT_STREQ(NULL, tools::FileType(""));
    ASSERT_EQ(NULL, tools::FileType(NULL));
}

TEST(ExtractArg, all)
{
    const char * argstr = "-file=example.txt";
    const char * result = NULL;

    ASSERT_FALSE(tools::ExtractArg(argstr, "-gfile=", &result));
    ASSERT_EQ(NULL, result);

    ASSERT_TRUE(tools::ExtractArg(argstr, "-file=", &result));
    ASSERT_STREQ("example.txt", result);

    ASSERT_FALSE(tools::ExtractArg(argstr, "-file="));

    ASSERT_TRUE(tools::ExtractArg(argstr, "-file=example.txt"));
}

TEST(GetHost, all)
{
    ASSERT_EQ("zhaopin.oa.com", tools::GetHost("http://zhaopin.oa.com/resume/pages/search/SearchInnerList.aspx?out=0&searchstr=%5B%7B%22n%22%3A%22TX%22%2C%22v%22%3A%22%E6%BD%98%E6%9F%B1%E6%96%B0%22%7D%2C%7B%22n%22%3A%22RQ%22%2C%22v%22%3A%22%E6%89%80%E6%9C%89%22%7D%5D"));
    ASSERT_EQ("zhaopin.oa.com", tools::GetHost("http://zhaopin.oa.com/"));
    ASSERT_EQ("zhaopin.oa.com", tools::GetHost("http://zhaopin.oa.com"));
    ASSERT_EQ("zhaopin.oa.com", tools::GetHost("zhaopin.oa.com/"));
    ASSERT_EQ("zhaopin.oa.com", tools::GetHost("zhaopin.oa.com"));
}

TEST(UrlEncode, UrlDecode)
{
    std::string e = "[{\"n\":\"TX\",\"v\":\"\xE6\xBD\x98\xE6\x9F\xB1\xE6\x96\xB0\"},{\"n\":\"RQ\",\"v\":\"\xE6\x89\x80\xE6\x9C\x89\"}]";
    std::string u = "%5B%7B%22n%22%3A%22TX%22%2C%22v%22%3A%22%E6%BD%98%E6%9F%B1%E6%96%B0%22%7D%2C%7B%22n%22%3A%22RQ%22%2C%22v%22%3A%22%E6%89%80%E6%9C%89%22%7D%5D";

    ASSERT_EQ(u, tools::UrlEncode(e));
    ASSERT_EQ(e, tools::UrlDecode(u));
}

const char * const fname[] =
{
    "utf8_1.txt",
    "utf8_2.txt",
    "ansi_1.txt",
    "ansi_2.txt",
    "gb2312_1.txt",
};

#define __ENCODING_TEST(func, enc_from, enc_to) \
    TEST(func, all){   \
        for(size_t i = 0;i < sizeof fname / sizeof fname[0];++i){   \
            std::ifstream inf(fname[i], ios::in | ios::binary); \
            ASSERT_TRUE(inf.is_open())<<"cannot open file '"<<fname[i]<<"'";    \
            std::string buf((1 << 20), 0);  \
            inf.read(&buf[0], buf.size());  \
            assert(inf.eof());  \
            buf.resize(inf.gcount());   \
            if(tools::func(buf)){   \
                CCharsetConvert cv(enc_from, enc_to, CCharsetConvert::kIgnore); \
                ASSERT_TRUE(cv.valid());    \
                string out; \
                ASSERT_TRUE(cv.convert(buf, out))<<"for '"<<fname[i]<<"'";  \
            }   \
        }   \
    }

__ENCODING_TEST(IsTextUtf8, "UTF8", "GBK")

__ENCODING_TEST(IsTextGbk, "GBK", "UTF8")

__ENCODING_TEST(IsTextGb2312, "GB2312", "UTF8")

#undef __ENCODING_TEST
