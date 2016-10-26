#include <marine/configuration.hh>

#include "../inc.h"

static inline std::string keyName(const char * key, int i)
{
    std::ostringstream oss;
    oss<<key<<i;
    return oss.str();
}

TEST(CConfiguration, kFormatEqual)
{
    CConfiguration config;
    ASSERT_FALSE(config.load("no_exist.conf"));
    ASSERT_TRUE(config.load("test_equal.conf", CConfiguration::kFormatEqual));
    ASSERT_EQ(tools::AbsFilename("test_equal.conf"), config.getConfName());

    int valCount = config.getInt("val.count");
    ASSERT_TRUE(valCount > 0);
    int emptyCount = config.getInt("empty.count");
    ASSERT_TRUE(emptyCount > 0);
    //test val
    for(int i = 1;i < valCount;++i){
        const std::string key = keyName("val", i);
        const std::string VAL = keyName("VAL", i);
        const std::string val = config.getString(key);
        ASSERT_FALSE(VAL != val)<<"CConfiguration::GetString('"<<key<<"') returns '"<<val<<"' is not '"<<VAL<<"'\n";
    }
    //test empty
    for(int i = 1;i < emptyCount;++i){
        const std::string key = keyName("empty", i);
        const std::string VAL = "";
        const std::string val = config.getString(key);
        ASSERT_FALSE(VAL != val)<<"CConfiguration::GetString('"<<key<<"') returns '"<<val<<"' is not '"<<VAL<<"'\n";
    }
    int empty_1 = config.getInt("");
    ASSERT_FALSE(1 != empty_1)<<"CConfiguration::GetInt('') returns "<<empty_1<<" is not 1\n";
    config.clear();
    ASSERT_EQ(0, config.getInt("val.count"));
}

TEST(CConfiguration, kFormatSpace)
{
    CConfiguration config;
    ASSERT_TRUE(config.load("test_space.conf", CConfiguration::kFormatSpace));
    int valCount = config.getInt("val.count");
    ASSERT_TRUE(valCount > 0);
    int emptyCount = config.getInt("empty.count");
    ASSERT_TRUE(emptyCount > 0);
    //test val
    for(int i = 1;i < valCount;++i){
        const std::string key = keyName("val", i);
        const std::string VAL = keyName("VAL", i);
        const std::string val = config.getString(key);
        ASSERT_FALSE(VAL != val)<<"CConfiguration::GetString('"<<key<<"') returns '"<<val<<"' is not '"<<VAL<<"'\n";
    }
    //test empty
    for(int i = 1;i < emptyCount;++i){
        const std::string key = keyName("empty", i);
        const std::string VAL = "";
        const std::string val = config.getString(key);
        ASSERT_FALSE(VAL != val)<<"CConfiguration::GetString('"<<key<<"') returns '"<<val<<"' is not '"<<VAL<<"'\n";
    }
    int empty_1 = config.getInt("1");
    ASSERT_FALSE(0 != empty_1)<<"CConfiguration::GetInt('1') returns "<<empty_1<<" is not ''\n";
}

TEST(CConfiguration, kFormatColon)
{
    CConfiguration config;
    ASSERT_FALSE(config.load("no_exist.conf"));
    ASSERT_TRUE(config.load("test_colon.conf", CConfiguration::kFormatColon));
    ASSERT_EQ(tools::AbsFilename("test_colon.conf"), config.getConfName());

    int valCount = config.getInt("val.count");
    ASSERT_TRUE(valCount > 0);
    int emptyCount = config.getInt("empty.count");
    ASSERT_TRUE(emptyCount > 0);
    //test val
    for(int i = 1;i < valCount;++i){
        const std::string key = keyName("val", i);
        const std::string VAL = keyName("VAL", i);
        const std::string val = config.getString(key);
        ASSERT_FALSE(VAL != val)<<"CConfiguration::GetString('"<<key<<"') returns '"<<val<<"' is not '"<<VAL<<"'\n";
    }
    //test empty
    for(int i = 1;i < emptyCount;++i){
        const std::string key = keyName("empty", i);
        const std::string VAL = "";
        const std::string val = config.getString(key);
        ASSERT_FALSE(VAL != val)<<"CConfiguration::GetString('"<<key<<"') returns '"<<val<<"' is not '"<<VAL<<"'\n";
    }
    int empty_1 = config.getInt("");
    ASSERT_FALSE(1 != empty_1)<<"CConfiguration::GetInt('') returns "<<empty_1<<" is not 1\n";
    config.clear();
    ASSERT_EQ(0, config.getInt("val.count"));
}

TEST(CConfiguration, string)
{
    CConfiguration config;
    ASSERT_TRUE(config.load("test_space.conf", CConfiguration::kFormatSpace));

    ASSERT_EQ("", config.getString("no_exists"));
    ASSERT_EQ("its default", config.getString("no_exists", "its default"));
    ASSERT_EQ("VAL1", config.getString("val1", "its default"));

    ASSERT_EQ("", config.getFilepath("no_exists"));
    ASSERT_EQ(tools::AbsFilename("default.txt"), config.getFilepath("no_exists", "default.txt"));
    ASSERT_EQ(tools::AbsFilename("VAL1"), config.getFilepath("val1", "default.txt"));
}

TEST(CConfiguration, int)
{
    CConfiguration config;
    ASSERT_TRUE(config.load("test_space.conf", CConfiguration::kFormatSpace));

    ASSERT_EQ(0, config.getInt("no_exists"));
    ASSERT_EQ(10, config.getInt("no_exists", 10));
    ASSERT_EQ(7, config.getInt("intval1", 10));
    ASSERT_EQ(7 << 10, config.getInt("intval2", 10));
    ASSERT_EQ(7 << 10, config.getInt("intval3", 10));
    ASSERT_EQ(7 << 20, config.getInt("intval4", 10));
    ASSERT_EQ(7 << 20, config.getInt("intval5", 10));
    ASSERT_EQ(7 << 30, config.getInt("intval6", 10));
    ASSERT_EQ(7 << 30, config.getInt("intval7", 10));

    ASSERT_EQ(9, config.getInt("intval1", 10, 9));
    ASSERT_EQ(6, config.getInt("intval1", 5, 3, 6));
}

TEST(CConfiguration, short)
{
    typedef short __Int;
    CConfiguration config;
    ASSERT_TRUE(config.load("test_space.conf", CConfiguration::kFormatSpace));

    ASSERT_EQ(__Int(0), config.getInt<__Int>("no_exists"));
    ASSERT_EQ(__Int(10), config.getInt<__Int>("no_exists", 10));
    ASSERT_EQ(__Int(7), config.getInt<__Int>("intval1", 10));
    ASSERT_EQ(__Int(__Int(7) << 10), config.getInt<__Int>("intval2", 10));
    ASSERT_EQ(__Int(__Int(7) << 10), config.getInt<__Int>("intval3", 10));
    ASSERT_EQ(__Int(__Int(7) << 20), config.getInt<__Int>("intval4", 10));
    ASSERT_EQ(__Int(__Int(7) << 20), config.getInt<__Int>("intval5", 10));
    ASSERT_EQ(__Int(__Int(7) << 30), config.getInt<__Int>("intval6", 10));
    ASSERT_EQ(__Int(__Int(7) << 30), config.getInt<__Int>("intval7", 10));
    /*
    ASSERT_EQ(__Int(__Int(7) << 40), config.getInt<__Int>("intval8", 10));
    ASSERT_EQ(__Int(__Int(7) << 40), config.getInt<__Int>("intval9", 10));
    ASSERT_EQ(__Int(__Int(7) << 50), config.getInt<__Int>("intval10", 10));
    ASSERT_EQ(__Int(__Int(7) << 50), config.getInt<__Int>("intval11", 10));
    ASSERT_EQ(__Int(__Int(7) << 60), config.getInt<__Int>("intval12", 10));
    ASSERT_EQ(__Int(__Int(7) << 60), config.getInt<__Int>("intval13", 10));
    */

    ASSERT_EQ(__Int(9), config.getInt<__Int>("intval1", 10, 9));
    ASSERT_EQ(__Int(6), config.getInt<__Int>("intval1", 5, 3, 6));
}

TEST(CConfiguration, uint8_t)
{
    typedef uint8_t __Int;
    CConfiguration config;
    ASSERT_TRUE(config.load("test_space.conf", CConfiguration::kFormatSpace));

    ASSERT_EQ(__Int(0), config.getInt<__Int>("no_exists"));
    ASSERT_EQ(__Int(10), config.getInt<__Int>("no_exists", 10));
    ASSERT_EQ(__Int(7), config.getInt<__Int>("intval1", 10));
    ASSERT_EQ(__Int(__Int(7) << 10), config.getInt<__Int>("intval2", 10));
    ASSERT_EQ(__Int(__Int(7) << 10), config.getInt<__Int>("intval3", 10));
    ASSERT_EQ(__Int(__Int(7) << 20), config.getInt<__Int>("intval4", 10));
    ASSERT_EQ(__Int(__Int(7) << 20), config.getInt<__Int>("intval5", 10));
    ASSERT_EQ(__Int(__Int(7) << 30), config.getInt<__Int>("intval6", 10));
    ASSERT_EQ(__Int(__Int(7) << 30), config.getInt<__Int>("intval7", 10));
    /*
    ASSERT_EQ(__Int(__Int(7) << 40), config.getInt<__Int>("intval8", 10));
    ASSERT_EQ(__Int(__Int(7) << 40), config.getInt<__Int>("intval9", 10));
    ASSERT_EQ(__Int(__Int(7) << 50), config.getInt<__Int>("intval10", 10));
    ASSERT_EQ(__Int(__Int(7) << 50), config.getInt<__Int>("intval11", 10));
    ASSERT_EQ(__Int(__Int(7) << 60), config.getInt<__Int>("intval12", 10));
    ASSERT_EQ(__Int(__Int(7) << 60), config.getInt<__Int>("intval13", 10));
    */

    ASSERT_EQ(__Int(9), config.getInt<__Int>("intval1", 10, 9));
    ASSERT_EQ(__Int(6), config.getInt<__Int>("intval1", 5, 3, 6));
}

TEST(CConfiguration, uint64_t)
{
    typedef uint64_t __Int;
    CConfiguration config;
    ASSERT_TRUE(config.load("test_space.conf", CConfiguration::kFormatSpace));

    ASSERT_EQ(__Int(0), config.getInt<__Int>("no_exists"));
    ASSERT_EQ(__Int(10), config.getInt<__Int>("no_exists", 10));
    ASSERT_EQ(__Int(7), config.getInt<__Int>("intval1", 10));
    ASSERT_EQ(__Int(__Int(7) << 10), config.getInt<__Int>("intval2", 10));
    ASSERT_EQ(__Int(__Int(7) << 10), config.getInt<__Int>("intval3", 10));
    ASSERT_EQ(__Int(__Int(7) << 20), config.getInt<__Int>("intval4", 10));
    ASSERT_EQ(__Int(__Int(7) << 20), config.getInt<__Int>("intval5", 10));
    ASSERT_EQ(__Int(__Int(7) << 30), config.getInt<__Int>("intval6", 10));
    ASSERT_EQ(__Int(__Int(7) << 30), config.getInt<__Int>("intval7", 10));
    ASSERT_EQ(__Int(__Int(7) << 40), config.getInt<__Int>("intval8", 10));
    ASSERT_EQ(__Int(__Int(7) << 40), config.getInt<__Int>("intval9", 10));
    ASSERT_EQ(__Int(__Int(7) << 50), config.getInt<__Int>("intval10", 10));
    ASSERT_EQ(__Int(__Int(7) << 50), config.getInt<__Int>("intval11", 10));
    ASSERT_EQ(__Int(__Int(7) << 60), config.getInt<__Int>("intval12", 10));
    ASSERT_EQ(__Int(__Int(7) << 60), config.getInt<__Int>("intval13", 10));

    ASSERT_EQ(__Int(9), config.getInt<__Int>("intval1", 10, 9));
    ASSERT_EQ(__Int(6), config.getInt<__Int>("intval1", 5, 3, 6));
}
