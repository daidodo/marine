#include <marine/sock_addr.hh>

#include "../inc.h"

const std::string hostv6 = "2001:DB8:2de::e13";
const std::string ipv6 = "[2001:db8:2de::e13]";
const std::string ifname = "lo";

static bool hasNet;

bool IsLocalhost(string port, string str)
{
    return ("127.0.0.1:" + port == str || "[::1]:" + port == str);
}

struct CEnv : public ::testing::Environment
{
    void SetUp(){
        hasNet = (0 == system("ping www.qq.com -c 2 -q"));
        cout<<"hasNet = "<<hasNet<<endl;
    }
};

__ADD_GTEST_ENV(CEnv, sock_addr);

TEST(CSockAddr, IPv4String)
{
    {
        uint32_t ip = 0x12345678;
        const std::string str = "18.52.86.120";
        ASSERT_EQ(str, CSockAddr::IPv4String(ip));
        ASSERT_EQ(str, CSockAddr::IPv4String(ip, true));
        ASSERT_EQ(str, CSockAddr::IPv4String(htonl(ip), false));
    }{
        uint32_t ip = 0;
        const std::string str = "0.0.0.0";
        ASSERT_EQ(str, CSockAddr::IPv4String(ip));
        ASSERT_EQ(str, CSockAddr::IPv4String(ip, true));
        ASSERT_EQ(str, CSockAddr::IPv4String(htonl(ip), false));
    }{
        uint32_t ip = 0xFFFFFFFF;
        const std::string str = "255.255.255.255";
        ASSERT_EQ(str, CSockAddr::IPv4String(ip));
        ASSERT_EQ(str, CSockAddr::IPv4String(ip, true));
        ASSERT_EQ(str, CSockAddr::IPv4String(htonl(ip), false));
    }
}

TEST(CSockAddr, IPv6String)
{
    {
        const char s[17] = "1234567890abcdef";
        const std::string str = "3132:3334:3536:3738:3930:6162:6364:6566";
        struct in6_addr addr;
        memcpy(&addr, s, sizeof addr);
        std::string ret;
        ASSERT_TRUE(CSockAddr::IPv6String(addr, &ret));
        ASSERT_EQ(str, ret);
        ASSERT_FALSE(CSockAddr::IPv6String(addr, NULL));
    }{
        const char s[17] = {0};
        const std::string str = "::";
        struct in6_addr addr;
        memcpy(&addr, s, sizeof addr);
        std::string ret;
        ASSERT_TRUE(CSockAddr::IPv6String(addr, &ret));
        ASSERT_EQ(str, ret);
        ASSERT_FALSE(CSockAddr::IPv6String(addr, NULL));
    }{
        char s[17];
        memset(s, 255, 16);
        const std::string str = "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff";
        struct in6_addr addr;
        memcpy(&addr, s, sizeof addr);
        std::string ret;
        ASSERT_TRUE(CSockAddr::IPv6String(addr, &ret));
        ASSERT_EQ(str, ret);
        ASSERT_FALSE(CSockAddr::IPv6String(addr, NULL));
    }
}

TEST(CSockAddr, IPv4FromStr)
{
    {
        const std::string str = "12.34.56.78";
        uint32_t ip = 0xC22384E;
        ASSERT_EQ(ip, CSockAddr::IPv4FromStr(str));
        ASSERT_EQ(ip, CSockAddr::IPv4FromStr(str, true));
        ASSERT_EQ(ntohl(ip), CSockAddr::IPv4FromStr(str, false));
    }{
        const std::string str = "0.0.0.0";
        uint32_t ip = 0;
        ASSERT_EQ(ip, CSockAddr::IPv4FromStr(str));
        ASSERT_EQ(ip, CSockAddr::IPv4FromStr(str, true));
        ASSERT_EQ(ntohl(ip), CSockAddr::IPv4FromStr(str, false));
    }{
        const std::string str = "127.0.0.1";
        uint32_t ip = 0x7F000001;
        ASSERT_EQ(ip, CSockAddr::IPv4FromStr(str));
        ASSERT_EQ(ip, CSockAddr::IPv4FromStr(str, true));
        ASSERT_EQ(ntohl(ip), CSockAddr::IPv4FromStr(str, false));
    }{
        const std::string str = "1";
        uint32_t ip = 0;
        ASSERT_EQ(ip, CSockAddr::IPv4FromStr(str));
        ASSERT_EQ(ip, CSockAddr::IPv4FromStr(str, true));
        ASSERT_EQ(ntohl(ip), CSockAddr::IPv4FromStr(str, false));
    }{
        const std::string str = "hi error";
        uint32_t ip = 0;
        ASSERT_EQ(ip, CSockAddr::IPv4FromStr(str));
        ASSERT_EQ(ip, CSockAddr::IPv4FromStr(str, true));
        ASSERT_EQ(ntohl(ip), CSockAddr::IPv4FromStr(str, false));
    }{
        const std::string str = "3132:3334:3536:3738:3930:6162:6364:6566";
        uint32_t ip = 0;
        ASSERT_EQ(ip, CSockAddr::IPv4FromStr(str));
        ASSERT_EQ(ip, CSockAddr::IPv4FromStr(str, true));
        ASSERT_EQ(ntohl(ip), CSockAddr::IPv4FromStr(str, false));
    }
}

TEST(CSockAddr, IPv6FromStr)
{
    {
        const std::string str = "3132:3334:3536:3738:3930:6162:6364:6566";
        const char s[17] = "1234567890abcdef";
        struct in6_addr addr;

        ASSERT_TRUE(CSockAddr::IPv6FromStr(str, &addr));
        ASSERT_EQ(0, memcmp(&addr, s, sizeof addr));
        ASSERT_FALSE(CSockAddr::IPv6FromStr(str, NULL));
    }{
        const std::string str = "::";
        const char s[17] = {0};
        struct in6_addr addr;

        ASSERT_TRUE(CSockAddr::IPv6FromStr(str, &addr));
        ASSERT_EQ(0, memcmp(&addr, s, sizeof addr));
        ASSERT_FALSE(CSockAddr::IPv6FromStr(str, NULL));
    }{
        const std::string str = "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff";
        char s[17];
        memset(s, 255, 16);
        struct in6_addr addr;

        ASSERT_TRUE(CSockAddr::IPv6FromStr(str, &addr));
        ASSERT_EQ(0, memcmp(&addr, s, sizeof addr));
        ASSERT_FALSE(CSockAddr::IPv6FromStr(str, NULL));
    }{
        const std::string str = "1.2.3.4";
        char s[17];
        memset(s, 255, 16);
        struct in6_addr addr;

        ASSERT_FALSE(CSockAddr::IPv6FromStr(str, &addr));
        ASSERT_FALSE(CSockAddr::IPv6FromStr(str, NULL));
    }
}

TEST(CSockAddr, IPv4FromIf)
{
    {
        const std::string eth = ifname;
        uint32_t ip = 0;
        ASSERT_NE(ip, CSockAddr::IPv4FromIf(eth));
        ASSERT_NE(ip, CSockAddr::IPv4FromIf(eth, true));
        ASSERT_NE(ip, CSockAddr::IPv4FromIf(eth, false));
        cout<<eth<<": "<<CSockAddr::IPv4String(CSockAddr::IPv4FromIf(eth))<<endl;
    }{
        const std::string eth = "eth99999999";
        uint32_t ip = 0;
        ASSERT_EQ(ip, CSockAddr::IPv4FromIf(eth));
        ASSERT_EQ(ip, CSockAddr::IPv4FromIf(eth, true));
        ASSERT_EQ(ip, CSockAddr::IPv4FromIf(eth, false));
    }
}

TEST(CSockAddr, ctor_1)
{
    {   //ctor 1
        const std::string serv = "12345";

        CSockAddr addr1("123.2.3.4", serv);
        ASSERT_TRUE(addr1.valid());
        ASSERT_EQ("123.2.3.4:" + serv, addr1.toString());

        CSockAddr addr1e("123.2.3.421", serv);
        ASSERT_FALSE(addr1e.valid());
        ASSERT_EQ("unknown", addr1e.toString());

        CSockAddr addr2("localhost", serv);
        ASSERT_TRUE(addr2.valid());
        ASSERT_TRUE(IsLocalhost(serv, addr2.toString()))<<"addr2="<<addr2.toString()<<endl;

        CSockAddr addr2e("localhosty", serv);
        ASSERT_FALSE(addr2e.valid());
        ASSERT_EQ("unknown", addr2e.toString());

        if(hasNet){
            CSockAddr addr3("www.qq.com", serv);
            EXPECT_TRUE(addr3.valid())<<"This test needs network";
            EXPECT_NE("unknown", addr3.toString());

            CSockAddr addr3e("www.qq.comaab", serv);
            EXPECT_FALSE(addr3e.valid());
            ASSERT_EQ("unknown", addr3e.toString());
        }

        CSockAddr addr4(ifname, serv);
        ASSERT_TRUE(addr4.valid())<<"No inerface named '"<<ifname<<"'";
        ASSERT_NE("unknown", addr4.toString());

        CSockAddr addr4e("eth99999999", serv);
        ASSERT_FALSE(addr4e.valid());
        ASSERT_EQ("unknown", addr4e.toString());

        CSockAddr addr5("", serv);
        ASSERT_TRUE(addr5.valid());
        ASSERT_TRUE(("127.0.0.1:" + serv == addr5.toString()) || ("[::1]:" + serv == addr5.toString()));

        CSockAddr addr6("0.0.0.0", serv);
        ASSERT_TRUE(addr6.valid());
        ASSERT_EQ("0.0.0.0:" + serv, addr6.toString());

        CSockAddr addr7(std::string(), serv);
        ASSERT_TRUE(addr7.valid());
        ASSERT_TRUE(("127.0.0.1:" + serv == addr7.toString()) || ("[::1]:" + serv == addr7.toString()));

        CSockAddr addr8("2001:DB8:2de::e13", serv);
        ASSERT_TRUE(addr8.valid());
        ASSERT_EQ("[2001:db8:2de::e13]:" + serv, addr8.toString());
    }
    {   //ctor 2
        const std::string serv = "12345x";

        CSockAddr addr1("123.2.3.4", serv);
        ASSERT_FALSE(addr1.valid());
        ASSERT_EQ("unknown", addr1.toString());

        CSockAddr addr1e("123.2.3.421", serv);
        ASSERT_FALSE(addr1e.valid());
        ASSERT_EQ("unknown", addr1e.toString());

        CSockAddr addr2("localhost", serv);
        ASSERT_FALSE(addr2.valid());
        ASSERT_EQ("unknown", addr2.toString());

        CSockAddr addr2e("localhosty", serv);
        ASSERT_FALSE(addr2e.valid());
        ASSERT_EQ("unknown", addr2e.toString());

        CSockAddr addr3("www.qq.com", serv);
        EXPECT_FALSE(addr3.valid());
        ASSERT_EQ("unknown", addr3.toString());

        CSockAddr addr3e("www.qq.comaab", serv);
        EXPECT_FALSE(addr3e.valid());
        ASSERT_EQ("unknown", addr3e.toString());

        CSockAddr addr4(ifname, serv);
        EXPECT_FALSE(addr4.valid());
        ASSERT_EQ("unknown", addr4.toString());

        CSockAddr addr4e("eth99999999", serv);
        ASSERT_FALSE(addr4e.valid());
        ASSERT_EQ("unknown", addr4e.toString());

        CSockAddr addr5("", serv);
        EXPECT_FALSE(addr5.valid());
        ASSERT_EQ("unknown", addr5.toString());

        CSockAddr addr6("0.0.0.0", serv);
        EXPECT_FALSE(addr6.valid());
        ASSERT_EQ("unknown", addr6.toString());

        CSockAddr addr7(std::string(), serv);
        EXPECT_FALSE(addr7.valid());
        ASSERT_EQ("unknown", addr7.toString());

        CSockAddr addr8("2001:DB8:2de::e13", serv);
        ASSERT_FALSE(addr8.valid());
        ASSERT_EQ("unknown", addr8.toString());
    }
    {   //ctor 3
        const std::string serv = "https";
        const std::string port = "443";

        CSockAddr addr1("123.2.3.4", serv);
        ASSERT_TRUE(addr1.valid());
        ASSERT_EQ("123.2.3.4:" + port, addr1.toString());

        CSockAddr addr1e("123.2.3.421", serv);
        ASSERT_FALSE(addr1e.valid());
        ASSERT_EQ("unknown", addr1e.toString());

        CSockAddr addr2("localhost", serv);
        ASSERT_TRUE(addr2.valid());
        ASSERT_TRUE(IsLocalhost(port, addr2.toString()))<<"addr2="<<addr2.toString()<<endl;

        CSockAddr addr2e("localhosty", serv);
        ASSERT_FALSE(addr2e.valid());
        ASSERT_EQ("unknown", addr2e.toString());

        if(hasNet){
            CSockAddr addr3("www.qq.com", serv);
            EXPECT_TRUE(addr3.valid())<<"This test needs network";
            EXPECT_NE("unknown", addr3.toString());

            CSockAddr addr3e("www.qq.comaab", serv);
            EXPECT_FALSE(addr3e.valid());
            ASSERT_EQ("unknown", addr3e.toString());
        }

        CSockAddr addr4(ifname, serv);
        ASSERT_TRUE(addr4.valid())<<"No inerface named '"<<ifname<<"'";
        ASSERT_NE("unknown", addr4.toString());

        CSockAddr addr4e("eth99999999", serv);
        ASSERT_FALSE(addr4e.valid());
        ASSERT_EQ("unknown", addr4e.toString());

        CSockAddr addr5("", serv);
        ASSERT_TRUE(addr5.valid());
        ASSERT_TRUE(IsLocalhost(port, addr5.toString()));

        CSockAddr addr6("0.0.0.0", serv);
        ASSERT_TRUE(addr6.valid());
        ASSERT_EQ("0.0.0.0:" + port, addr6.toString());

        CSockAddr addr7(std::string(), serv);
        ASSERT_TRUE(addr7.valid());
        ASSERT_TRUE(IsLocalhost(port, addr7.toString()));

        CSockAddr addr8("2001:DB8:2de::e13", serv);
        ASSERT_TRUE(addr8.valid());
        ASSERT_EQ("[2001:db8:2de::e13]:" + port, addr8.toString());
    }
    {   //ctor 4
        const std::string serv = "httpsaabdfdfa";

        CSockAddr addr1("123.2.3.4", serv);
        ASSERT_FALSE(addr1.valid());
        ASSERT_EQ("unknown", addr1.toString());

        CSockAddr addr1e("123.2.3.421", serv);
        ASSERT_FALSE(addr1e.valid());
        ASSERT_EQ("unknown", addr1e.toString());

        CSockAddr addr2("localhost", serv);
        ASSERT_FALSE(addr2.valid());
        ASSERT_EQ("unknown", addr2.toString());

        CSockAddr addr2e("localhosty", serv);
        ASSERT_FALSE(addr2e.valid());
        ASSERT_EQ("unknown", addr2e.toString());

        CSockAddr addr3("www.qq.com", serv);
        EXPECT_FALSE(addr3.valid());
        ASSERT_EQ("unknown", addr3.toString());

        CSockAddr addr3e("www.qq.comaab", serv);
        EXPECT_FALSE(addr3e.valid());
        ASSERT_EQ("unknown", addr3e.toString());

        CSockAddr addr4(ifname, serv);
        EXPECT_FALSE(addr4.valid());
        ASSERT_EQ("unknown", addr4.toString());

        CSockAddr addr4e("eth99999999", serv);
        ASSERT_FALSE(addr4e.valid());
        ASSERT_EQ("unknown", addr4e.toString());

        CSockAddr addr5("", serv);
        EXPECT_FALSE(addr5.valid());
        ASSERT_EQ("unknown", addr5.toString());

        CSockAddr addr6("0.0.0.0", serv);
        EXPECT_FALSE(addr6.valid());
        ASSERT_EQ("unknown", addr6.toString());

        CSockAddr addr7(std::string(), serv);
        EXPECT_FALSE(addr7.valid());
        ASSERT_EQ("unknown", addr7.toString());

        CSockAddr addr8("2001:DB8:2de::e13", serv);
        ASSERT_FALSE(addr8.valid());
        ASSERT_EQ("unknown", addr8.toString());
    }
    {   //ctor 5
        const std::string serv = "0";
        const std::string port = serv;

        CSockAddr addr1("123.2.3.4", serv);
        ASSERT_TRUE(addr1.valid());
        ASSERT_EQ("123.2.3.4:" + port, addr1.toString());

        CSockAddr addr1e("123.2.3.421", serv);
        ASSERT_FALSE(addr1e.valid());
        ASSERT_EQ("unknown", addr1e.toString());

        CSockAddr addr2("localhost", serv);
        ASSERT_TRUE(addr2.valid());
        ASSERT_TRUE(IsLocalhost(port, addr2.toString()));

        CSockAddr addr2e("localhosty", serv);
        ASSERT_FALSE(addr2e.valid());
        ASSERT_EQ("unknown", addr2e.toString());

        if(hasNet){
            CSockAddr addr3("www.qq.com", serv);
            EXPECT_TRUE(addr3.valid())<<"This test needs network";
            EXPECT_NE("unknown", addr3.toString());

            CSockAddr addr3e("www.qq.comaab", serv);
            EXPECT_FALSE(addr3e.valid());
            ASSERT_EQ("unknown", addr3e.toString());
        }

        CSockAddr addr4(ifname, serv);
        ASSERT_TRUE(addr4.valid())<<"No inerface named '"<<ifname<<"'";
        ASSERT_NE("unknown", addr4.toString());

        CSockAddr addr4e("eth99999999", serv);
        ASSERT_FALSE(addr4e.valid());
        ASSERT_EQ("unknown", addr4e.toString());

        CSockAddr addr5("", serv);
        ASSERT_TRUE(addr5.valid());
        ASSERT_TRUE(IsLocalhost(port, addr5.toString()));

        CSockAddr addr6("0.0.0.0", serv);
        ASSERT_TRUE(addr6.valid());
        ASSERT_EQ("0.0.0.0:" + port, addr6.toString());

        CSockAddr addr7(std::string(), serv);
        ASSERT_TRUE(addr7.valid());
        ASSERT_TRUE(IsLocalhost(port, addr7.toString()));

        CSockAddr addr8("2001:DB8:2de::e13", serv);
        ASSERT_TRUE(addr8.valid());
        ASSERT_EQ("[2001:db8:2de::e13]:" + serv, addr8.toString());
    }
    {   //ctor 6
        const std::string serv;
        const std::string port = "0";

        CSockAddr addr1("123.2.3.4", serv);
        ASSERT_TRUE(addr1.valid());
        ASSERT_EQ("123.2.3.4:" + port, addr1.toString());

        CSockAddr addr1e("123.2.3.421", serv);
        ASSERT_FALSE(addr1e.valid());
        ASSERT_EQ("unknown", addr1e.toString());

        CSockAddr addr2("localhost", serv);
        ASSERT_TRUE(addr2.valid());
        ASSERT_TRUE(IsLocalhost(port, addr2.toString()));

        CSockAddr addr2e("localhosty", serv);
        ASSERT_FALSE(addr2e.valid());
        ASSERT_EQ("unknown", addr2e.toString());

        if(hasNet){
            CSockAddr addr3("www.qq.com", serv);
            EXPECT_TRUE(addr3.valid())<<"This test needs network";
            EXPECT_NE("unknown", addr3.toString());

            CSockAddr addr3e("www.qq.comaab", serv);
            EXPECT_FALSE(addr3e.valid());
            ASSERT_EQ("unknown", addr3e.toString());
        }

        CSockAddr addr4(ifname, serv);
        ASSERT_TRUE(addr4.valid())<<"No inerface named '"<<ifname<<"'";
        ASSERT_NE("unknown", addr4.toString());

        CSockAddr addr4e("eth99999999", serv);
        ASSERT_FALSE(addr4e.valid());
        ASSERT_EQ("unknown", addr4e.toString());

        CSockAddr addr5("", serv);
        ASSERT_FALSE(addr5.valid());
        ASSERT_EQ("unknown", addr5.toString());

        CSockAddr addr6("0.0.0.0", serv);
        ASSERT_TRUE(addr6.valid());
        ASSERT_EQ("0.0.0.0:" + port, addr6.toString());

        CSockAddr addr7(std::string(), serv);
        ASSERT_FALSE(addr7.valid());
        ASSERT_EQ("unknown", addr7.toString());

        CSockAddr addr8("2001:DB8:2de:0:0:0:0:e13", serv);
        ASSERT_TRUE(addr8.valid());
        ASSERT_EQ("[2001:db8:2de::e13]:" + port, addr8.toString());
    }
}
TEST(CSockAddr, ctor_2)
{
    {   //1
        const std::string host = "123.4.5.6";
        const std::string ip = host;
        {
            CSockAddr addr(host);
            ASSERT_TRUE(addr.valid());
            ASSERT_EQ(ip + ":0", addr.toString());
        }{
            CSockAddr addr(host, 12345);
            ASSERT_TRUE(addr.valid());
            ASSERT_EQ(ip + ":12345", addr.toString());
        }{
            CSockAddr addr(host, 12345, true);
            ASSERT_TRUE(addr.valid());
            ASSERT_EQ(ip + ":12345", addr.toString());
        }{
            CSockAddr addr(host, htons(12345), false);
            ASSERT_TRUE(addr.valid());
            ASSERT_EQ(ip + ":12345", addr.toString());
        }{
            CSockAddr addr(host, 0);
            ASSERT_TRUE(addr.valid());
            ASSERT_EQ(ip + ":0", addr.toString());
        }{
            CSockAddr addr(host, 0, true);
            ASSERT_TRUE(addr.valid());
            ASSERT_EQ(ip + ":0", addr.toString());
        }{
            CSockAddr addr(host, 0, false);
            ASSERT_TRUE(addr.valid());
            ASSERT_EQ(ip + ":0", addr.toString());
        }
    }
    {   //1e
        const std::string host = "123.4.5.621";
        {
            CSockAddr addr(host);
            ASSERT_FALSE(addr.valid());
            ASSERT_EQ("unknown", addr.toString());
        }{
            CSockAddr addr(host, 12345);
            ASSERT_FALSE(addr.valid());
            ASSERT_EQ("unknown", addr.toString());
        }{
            CSockAddr addr(host, 12345, true);
            ASSERT_FALSE(addr.valid());
            ASSERT_EQ("unknown", addr.toString());
        }{
            CSockAddr addr(host, htons(12345), false);
            ASSERT_FALSE(addr.valid());
            ASSERT_EQ("unknown", addr.toString());
        }{
            CSockAddr addr(host, 0);
            ASSERT_FALSE(addr.valid());
            ASSERT_EQ("unknown", addr.toString());
        }{
            CSockAddr addr(host, 0, true);
            ASSERT_FALSE(addr.valid());
            ASSERT_EQ("unknown", addr.toString());
        }{
            CSockAddr addr(host, 0, false);
            ASSERT_FALSE(addr.valid());
            ASSERT_EQ("unknown", addr.toString());
        }
    }
    {   //2
        const std::string host = "localhost";
        {
            CSockAddr addr(host);
            ASSERT_TRUE(addr.valid());
            ASSERT_NE("unknown", addr.toString());
        }{
            CSockAddr addr(host, 12345);
            ASSERT_TRUE(addr.valid());
            ASSERT_NE("unknown", addr.toString());
        }{
            CSockAddr addr(host, 12345, true);
            ASSERT_TRUE(addr.valid());
            ASSERT_NE("unknown", addr.toString());
        }{
            CSockAddr addr(host, htons(12345), false);
            ASSERT_TRUE(addr.valid());
            ASSERT_NE("unknown", addr.toString());
        }{
            CSockAddr addr(host, 0);
            ASSERT_TRUE(addr.valid());
            ASSERT_NE("unknown", addr.toString());
        }{
            CSockAddr addr(host, 0, true);
            ASSERT_TRUE(addr.valid());
            ASSERT_NE("unknown", addr.toString());
        }{
            CSockAddr addr(host, 0, false);
            ASSERT_TRUE(addr.valid());
            ASSERT_NE("unknown", addr.toString());
        }
    }
    {   //2e
        const std::string host = "localhosty";
        {
            CSockAddr addr(host);
            ASSERT_FALSE(addr.valid());
            ASSERT_EQ("unknown", addr.toString());
        }{
            CSockAddr addr(host, 12345);
            ASSERT_FALSE(addr.valid());
            ASSERT_EQ("unknown", addr.toString());
        }{
            CSockAddr addr(host, 12345, true);
            ASSERT_FALSE(addr.valid());
            ASSERT_EQ("unknown", addr.toString());
        }{
            CSockAddr addr(host, htons(12345), false);
            ASSERT_FALSE(addr.valid());
            ASSERT_EQ("unknown", addr.toString());
        }{
            CSockAddr addr(host, 0);
            ASSERT_FALSE(addr.valid());
            ASSERT_EQ("unknown", addr.toString());
        }{
            CSockAddr addr(host, 0, true);
            ASSERT_FALSE(addr.valid());
            ASSERT_EQ("unknown", addr.toString());
        }{
            CSockAddr addr(host, 0, false);
            ASSERT_FALSE(addr.valid());
            ASSERT_EQ("unknown", addr.toString());
        }
    }
    if(hasNet){   //3
        const std::string host = "www.qq.com";
        {
            CSockAddr addr(host);
            EXPECT_TRUE(addr.valid())<<"This test needs network";
            EXPECT_NE("unknown", addr.toString());
        }{
            CSockAddr addr(host, 12345);
            EXPECT_TRUE(addr.valid())<<"This test needs network";
            EXPECT_NE("unknown", addr.toString());
        }{
            CSockAddr addr(host, 12345, true);
            EXPECT_TRUE(addr.valid())<<"This test needs network";
            EXPECT_NE("unknown", addr.toString());
        }{
            CSockAddr addr(host, htons(12345), false);
            EXPECT_TRUE(addr.valid())<<"This test needs network";
            EXPECT_NE("unknown", addr.toString());
        }{
            CSockAddr addr(host, 0);
            EXPECT_TRUE(addr.valid())<<"This test needs network";
            EXPECT_NE("unknown", addr.toString());
        }{
            CSockAddr addr(host, 0, true);
            EXPECT_TRUE(addr.valid())<<"This test needs network";
            EXPECT_NE("unknown", addr.toString());
        }{
            CSockAddr addr(host, 0, false);
            EXPECT_TRUE(addr.valid())<<"This test needs network";
            EXPECT_NE("unknown", addr.toString());
        }
    }
    if(hasNet){   //3e
        const std::string host = "www.qq.comgadnaega";
        {
            CSockAddr addr(host);
            ASSERT_FALSE(addr.valid());
            ASSERT_EQ("unknown", addr.toString());
        }{
            CSockAddr addr(host, 12345);
            ASSERT_FALSE(addr.valid());
            ASSERT_EQ("unknown", addr.toString());
        }{
            CSockAddr addr(host, 12345, true);
            ASSERT_FALSE(addr.valid());
            ASSERT_EQ("unknown", addr.toString());
        }{
            CSockAddr addr(host, htons(12345), false);
            ASSERT_FALSE(addr.valid());
            ASSERT_EQ("unknown", addr.toString());
        }{
            CSockAddr addr(host, 0);
            ASSERT_FALSE(addr.valid());
            ASSERT_EQ("unknown", addr.toString());
        }{
            CSockAddr addr(host, 0, true);
            ASSERT_FALSE(addr.valid());
            ASSERT_EQ("unknown", addr.toString());
        }{
            CSockAddr addr(host, 0, false);
            ASSERT_FALSE(addr.valid());
            ASSERT_EQ("unknown", addr.toString());
        }
    }
    {   //4
        const std::string host = ifname;
        {
            CSockAddr addr(host);
            ASSERT_TRUE(addr.valid());
            ASSERT_NE("unknown", addr.toString());
        }{
            CSockAddr addr(host, 12345);
            ASSERT_TRUE(addr.valid());
            ASSERT_NE("unknown", addr.toString());
        }{
            CSockAddr addr(host, 12345, true);
            ASSERT_TRUE(addr.valid());
            ASSERT_NE("unknown", addr.toString());
        }{
            CSockAddr addr(host, htons(12345), false);
            ASSERT_TRUE(addr.valid());
            ASSERT_NE("unknown", addr.toString());
        }{
            CSockAddr addr(host, 0);
            ASSERT_TRUE(addr.valid());
            ASSERT_NE("unknown", addr.toString());
        }{
            CSockAddr addr(host, 0, true);
            ASSERT_TRUE(addr.valid());
            ASSERT_NE("unknown", addr.toString());
        }{
            CSockAddr addr(host, 0, false);
            ASSERT_TRUE(addr.valid());
            ASSERT_NE("unknown", addr.toString());
        }
    }
    {   //4e
        const std::string host = "eth999999999";
        {
            CSockAddr addr(host);
            ASSERT_FALSE(addr.valid());
            ASSERT_EQ("unknown", addr.toString());
        }{
            CSockAddr addr(host, 12345);
            ASSERT_FALSE(addr.valid());
            ASSERT_EQ("unknown", addr.toString());
        }{
            CSockAddr addr(host, 12345, true);
            ASSERT_FALSE(addr.valid());
            ASSERT_EQ("unknown", addr.toString());
        }{
            CSockAddr addr(host, htons(12345), false);
            ASSERT_FALSE(addr.valid());
            ASSERT_EQ("unknown", addr.toString());
        }{
            CSockAddr addr(host, 0);
            ASSERT_FALSE(addr.valid());
            ASSERT_EQ("unknown", addr.toString());
        }{
            CSockAddr addr(host, 0, true);
            ASSERT_FALSE(addr.valid());
            ASSERT_EQ("unknown", addr.toString());
        }{
            CSockAddr addr(host, 0, false);
            ASSERT_FALSE(addr.valid());
            ASSERT_EQ("unknown", addr.toString());
        }
    }
    {   //5
        const std::string host = "0.0.0.0";
        const std::string ip = host;
        {
            CSockAddr addr(host);
            ASSERT_TRUE(addr.valid());
            ASSERT_EQ(ip + ":0", addr.toString());
        }{
            CSockAddr addr(host, 12345);
            ASSERT_TRUE(addr.valid());
            ASSERT_EQ(ip + ":12345", addr.toString());
        }{
            CSockAddr addr(host, 12345, true);
            ASSERT_TRUE(addr.valid());
            ASSERT_EQ(ip + ":12345", addr.toString());
        }{
            CSockAddr addr(host, htons(12345), false);
            ASSERT_TRUE(addr.valid());
            ASSERT_EQ(ip + ":12345", addr.toString());
        }{
            CSockAddr addr(host, 0);
            ASSERT_TRUE(addr.valid());
            ASSERT_EQ(ip + ":0", addr.toString());
        }{
            CSockAddr addr(host, 0, true);
            ASSERT_TRUE(addr.valid());
            ASSERT_EQ(ip + ":0", addr.toString());
        }{
            CSockAddr addr(host, 0, false);
            ASSERT_TRUE(addr.valid());
            ASSERT_EQ(ip + ":0", addr.toString());
        }
    }
    {   //6.1
        const std::string host = "";
        const std::string ip = "127.0.0.1";
        const std::string ip6 = "[::1]";
        {
            CSockAddr addr(host);
            ASSERT_TRUE(addr.valid());
            ASSERT_TRUE((ip + ":0" == addr.toString()) || (ip6 + ":0" == addr.toString()));
        }{
            CSockAddr addr(host, 12345);
            ASSERT_TRUE(addr.valid());
            ASSERT_TRUE((ip + ":12345" == addr.toString()) || (ip6 + ":12345" == addr.toString()));
        }{
            CSockAddr addr(host, 12345, true);
            ASSERT_TRUE(addr.valid());
            ASSERT_TRUE((ip + ":12345" == addr.toString()) || (ip6 + ":12345" == addr.toString()));
        }{
            CSockAddr addr(host, htons(12345), false);
            ASSERT_TRUE(addr.valid());
            ASSERT_TRUE((ip + ":12345" == addr.toString()) || (ip6 + ":12345" == addr.toString()));
        }{
            CSockAddr addr(host, 0);
            ASSERT_TRUE(addr.valid());
            ASSERT_TRUE((ip + ":0" == addr.toString()) || (ip6 + ":0" == addr.toString()));
        }{
            CSockAddr addr(host, 0, true);
            ASSERT_TRUE(addr.valid());
            ASSERT_TRUE((ip + ":0" == addr.toString()) || (ip6 + ":0" == addr.toString()));
        }{
            CSockAddr addr(host, 0, false);
            ASSERT_TRUE(addr.valid());
            ASSERT_TRUE((ip + ":0" == addr.toString()) || (ip6 + ":0" == addr.toString()));
        }
    }
    {   //6.2
        const std::string host;
        const std::string ip = "127.0.0.1";
        const std::string ip6 = "[::1]";
        {
            CSockAddr addr(host);
            ASSERT_TRUE(addr.valid());
            ASSERT_TRUE((ip + ":0" == addr.toString()) || (ip6 + ":0" == addr.toString()));
        }{
            CSockAddr addr(host, 12345);
            ASSERT_TRUE(addr.valid());
            ASSERT_TRUE((ip + ":12345" == addr.toString()) || (ip6 + ":12345" == addr.toString()));
        }{
            CSockAddr addr(host, 12345, true);
            ASSERT_TRUE(addr.valid());
            ASSERT_TRUE((ip + ":12345" == addr.toString()) || (ip6 + ":12345" == addr.toString()));
        }{
            CSockAddr addr(host, htons(12345), false);
            ASSERT_TRUE(addr.valid());
            ASSERT_TRUE((ip + ":12345" == addr.toString()) || (ip6 + ":12345" == addr.toString()));
        }{
            CSockAddr addr(host, 0);
            ASSERT_TRUE(addr.valid());
            ASSERT_TRUE((ip + ":0" == addr.toString()) || (ip6 + ":0" == addr.toString()));
        }{
            CSockAddr addr(host, 0, true);
            ASSERT_TRUE(addr.valid());
            ASSERT_TRUE((ip + ":0" == addr.toString()) || (ip6 + ":0" == addr.toString()));
        }{
            CSockAddr addr(host, 0, false);
            ASSERT_TRUE(addr.valid());
            ASSERT_TRUE((ip + ":0" == addr.toString()) || (ip6 + ":0" == addr.toString()));
        }
    }{  //7
        const std::string host = "2001:DB8:2de::e13";
        const std::string ip = "[2001:db8:2de::e13]";
        {
            CSockAddr addr(host);
            ASSERT_TRUE(addr.valid());
            ASSERT_EQ(ip + ":0", addr.toString());
        }{
            CSockAddr addr(host, 12345);
            ASSERT_TRUE(addr.valid());
            ASSERT_EQ(ip + ":12345", addr.toString());
        }{
            CSockAddr addr(host, 12345, true);
            ASSERT_TRUE(addr.valid());
            ASSERT_EQ(ip + ":12345", addr.toString());
        }{
            CSockAddr addr(host, htons(12345), false);
            ASSERT_TRUE(addr.valid());
            ASSERT_EQ(ip + ":12345", addr.toString());
        }{
            CSockAddr addr(host, 0);
            ASSERT_TRUE(addr.valid());
            ASSERT_EQ(ip + ":0", addr.toString());
        }{
            CSockAddr addr(host, 0, true);
            ASSERT_TRUE(addr.valid());
            ASSERT_EQ(ip + ":0", addr.toString());
        }{
            CSockAddr addr(host, 0, false);
            ASSERT_TRUE(addr.valid());
            ASSERT_EQ(ip + ":0", addr.toString());
        }
    }
}

TEST(CSockAddr, setIp_str_1)
{
    {   //1
        CSockAddr addr;
        ASSERT_FALSE(addr.valid());
        ASSERT_EQ("unknown", addr.toString());

        ASSERT_TRUE(addr.setIp("123.4.5.6"));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ("123.4.5.6:0", addr.toString());
    }{  //1e
        CSockAddr addr;
        ASSERT_FALSE(addr.valid());
        ASSERT_EQ("unknown", addr.toString());

        ASSERT_FALSE(addr.setIp("123.4.5.621"));
        ASSERT_FALSE(addr.valid());
        ASSERT_EQ("unknown", addr.toString());
    }{  //2
        CSockAddr addr;
        ASSERT_FALSE(addr.valid());
        ASSERT_EQ("unknown", addr.toString());

        ASSERT_TRUE(addr.setIp("localhost"));
        ASSERT_TRUE(addr.valid());
        ASSERT_NE("unknown", addr.toString());
    }{  //2e
        CSockAddr addr;
        ASSERT_FALSE(addr.valid());
        ASSERT_EQ("unknown", addr.toString());

        ASSERT_FALSE(addr.setIp("localhosty"));
        ASSERT_FALSE(addr.valid());
        ASSERT_EQ("unknown", addr.toString());
    }if(hasNet){  //3
        CSockAddr addr;
        ASSERT_FALSE(addr.valid());
        ASSERT_EQ("unknown", addr.toString());

        EXPECT_TRUE(addr.setIp("www.qq.com"))<<"This test needs network";
        EXPECT_TRUE(addr.valid());
        EXPECT_NE("unknown", addr.toString());
    }if(hasNet){  //3e
        CSockAddr addr;
        ASSERT_FALSE(addr.valid());
        ASSERT_EQ("unknown", addr.toString());

        ASSERT_FALSE(addr.setIp("www.qq.comadganegnd"));
        ASSERT_FALSE(addr.valid());
        ASSERT_EQ("unknown", addr.toString());
    }{  //4
        CSockAddr addr;
        ASSERT_FALSE(addr.valid());
        ASSERT_EQ("unknown", addr.toString());

        ASSERT_TRUE(addr.setIp(ifname));
        ASSERT_TRUE(addr.valid());
        ASSERT_NE("unknown", addr.toString());
    }{  //4e
        CSockAddr addr;
        ASSERT_FALSE(addr.valid());
        ASSERT_EQ("unknown", addr.toString());

        ASSERT_FALSE(addr.setIp("eth9999999999"));
        ASSERT_FALSE(addr.valid());
        ASSERT_EQ("unknown", addr.toString());
    }{  //5
        CSockAddr addr;
        ASSERT_FALSE(addr.valid());
        ASSERT_EQ("unknown", addr.toString());

        ASSERT_TRUE(addr.setIp("0.0.0.0"));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ("0.0.0.0:0", addr.toString());
    }{  //6.1
        CSockAddr addr;
        ASSERT_FALSE(addr.valid());
        ASSERT_EQ("unknown", addr.toString());

        ASSERT_FALSE(addr.setIp(""));
        ASSERT_FALSE(addr.valid());
        ASSERT_EQ("unknown", addr.toString());
    }{  //6.2
        CSockAddr addr;
        ASSERT_FALSE(addr.valid());
        ASSERT_EQ("unknown", addr.toString());

        ASSERT_FALSE(addr.setIp(std::string()));
        ASSERT_FALSE(addr.valid());
        ASSERT_EQ("unknown", addr.toString());
    }{  //7
        CSockAddr addr;
        ASSERT_FALSE(addr.valid());
        ASSERT_EQ("unknown", addr.toString());

        ASSERT_TRUE(addr.setIp(hostv6));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(ipv6 + ":0", addr.toString());
    }
}

TEST(CSockAddr, setIp_str_2)
{
    const std::string host = "12.23.34.45";
    const std::string ip = host;
    const std::string serv;
    const std::string port = "0";
    const std::string old = ip + ":" + port;
    {   //1
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_TRUE(addr.setIp("123.4.5.6"));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ("123.4.5.6:" + port, addr.toString());
    }{  //1e
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_FALSE(addr.setIp("123.4.5.621"));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());
    }{  //2
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_TRUE(addr.setIp("localhost"));
        ASSERT_TRUE(addr.valid());
        ASSERT_NE("unknown", addr.toString());
        ASSERT_NE(old, addr.toString());
    }{  //2e
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_FALSE(addr.setIp("localhosty"));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());
    }if(hasNet){  //3
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        EXPECT_TRUE(addr.setIp("www.qq.com"))<<"This test needs network";
        EXPECT_TRUE(addr.valid());
        EXPECT_NE("unknown", addr.toString());
        ASSERT_NE(old, addr.toString());
    }if(hasNet){  //3e
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_FALSE(addr.setIp("www.qq.comadganegnd"));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());
    }{  //4
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_TRUE(addr.setIp(ifname));
        ASSERT_TRUE(addr.valid());
        ASSERT_NE("unknown", addr.toString());
        ASSERT_NE(old, addr.toString());
    }{  //4e
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_FALSE(addr.setIp("eth9999999999"));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());
    }{  //5
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_TRUE(addr.setIp("0.0.0.0"));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ("0.0.0.0:" + port, addr.toString());
    }{  //6.1
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_FALSE(addr.setIp(""));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());
    }{  //6.2
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_FALSE(addr.setIp(std::string()));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());
    }{  //7
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_TRUE(addr.setIp(hostv6));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(ipv6 + ":" + port, addr.toString());
    }
}

TEST(CSockAddr, setIp_str_3)
{
    const std::string host;
    const std::string ip = "127.0.0.1";
    const std::string ip6 = "[::1]";
    const std::string serv = "1000";
    const std::string port = serv;
    const std::string old = ip + ":" + port;
    const std::string old6 = ip6 + ":" + port;
    {   //1
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_TRUE(old == addr.toString() || old6 == addr.toString());

        ASSERT_TRUE(addr.setIp("123.4.5.6"));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ("123.4.5.6:" + port, addr.toString());
    }{  //1e
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_TRUE(old == addr.toString() || old6 == addr.toString());

        ASSERT_FALSE(addr.setIp("123.4.5.621"));
        ASSERT_TRUE(addr.valid());
        ASSERT_TRUE(old == addr.toString() || old6 == addr.toString());
    }{  //2
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_TRUE(old == addr.toString() || old6 == addr.toString());

        ASSERT_TRUE(addr.setIp("localhost"));
        ASSERT_TRUE(addr.valid());
        ASSERT_NE("unknown", addr.toString());
        ASSERT_TRUE(IsLocalhost(port, addr.toString()));
    }{  //2e
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_TRUE(old == addr.toString() || old6 == addr.toString());

        ASSERT_FALSE(addr.setIp("localhosty"));
        ASSERT_TRUE(addr.valid());
        ASSERT_TRUE(old == addr.toString() || old6 == addr.toString());
    }if(hasNet){  //3
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_TRUE(old == addr.toString() || old6 == addr.toString());

        EXPECT_TRUE(addr.setIp("www.qq.com"))<<"This test needs network";
        EXPECT_TRUE(addr.valid());
        EXPECT_NE("unknown", addr.toString());
        ASSERT_NE(old, addr.toString());
    }if(hasNet){  //3e
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_TRUE(old == addr.toString() || old6 == addr.toString());

        ASSERT_FALSE(addr.setIp("www.qq.comadganegnd"));
        ASSERT_TRUE(addr.valid());
        ASSERT_TRUE(old == addr.toString() || old6 == addr.toString());
    }{  //4
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_TRUE(old == addr.toString() || old6 == addr.toString());

        ASSERT_TRUE(addr.setIp(ifname));
        ASSERT_TRUE(addr.valid());
        ASSERT_NE("unknown", addr.toString());
        //ASSERT_NE(old, addr.toString());
    }{  //4e
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_TRUE(old == addr.toString() || old6 == addr.toString());

        ASSERT_FALSE(addr.setIp("eth9999999999"));
        ASSERT_TRUE(addr.valid());
        ASSERT_TRUE(old == addr.toString() || old6 == addr.toString());
    }{  //5
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_TRUE(old == addr.toString() || old6 == addr.toString());

        ASSERT_TRUE(addr.setIp("0.0.0.0"));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ("0.0.0.0:" + port, addr.toString());
    }{  //6.1
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_TRUE(old == addr.toString() || old6 == addr.toString());

        ASSERT_FALSE(addr.setIp(""));
        ASSERT_TRUE(addr.valid());
        ASSERT_TRUE(old == addr.toString() || old6 == addr.toString());
    }{  //6.2
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_TRUE(old == addr.toString() || old6 == addr.toString());

        ASSERT_FALSE(addr.setIp(std::string()));
        ASSERT_TRUE(addr.valid());
        ASSERT_TRUE(old == addr.toString() || old6 == addr.toString());
    }{  //7
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_TRUE(old == addr.toString() || old6 == addr.toString());

        ASSERT_TRUE(addr.setIp(hostv6));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(ipv6 + ":" + port, addr.toString());
    }
}

TEST(CSockAddr, setIp_str_4)
{
    const std::string host = "1.2.3.45";
    const std::string ip = host;
    const std::string serv = "https";
    const std::string port = "443";
    const std::string old = ip + ":" + port;
    {   //1
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_TRUE(addr.setIp("123.4.5.6"));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ("123.4.5.6:" + port, addr.toString());
    }{  //1e
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_FALSE(addr.setIp("123.4.5.621"));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());
    }{  //2
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_TRUE(addr.setIp("localhost"));
        ASSERT_TRUE(addr.valid());
        ASSERT_NE("unknown", addr.toString());
        ASSERT_NE(old, addr.toString());
    }{  //2e
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_FALSE(addr.setIp("localhosty"));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());
    }if(hasNet){  //3
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        EXPECT_TRUE(addr.setIp("www.qq.com"))<<"This test needs network";
        EXPECT_TRUE(addr.valid());
        EXPECT_NE("unknown", addr.toString());
        ASSERT_NE(old, addr.toString());
    }if(hasNet){  //3e
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_FALSE(addr.setIp("www.qq.comadganegnd"));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());
    }{  //4
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_TRUE(addr.setIp(ifname));
        ASSERT_TRUE(addr.valid());
        ASSERT_NE("unknown", addr.toString());
        ASSERT_NE(old, addr.toString());
    }{  //4e
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_FALSE(addr.setIp("eth9999999999"));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());
    }{  //5
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_TRUE(addr.setIp("0.0.0.0"));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ("0.0.0.0:" + port, addr.toString());
    }{  //6.1
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_FALSE(addr.setIp(""));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());
    }{  //6.2
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_FALSE(addr.setIp(std::string()));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());
    }{  //7
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_TRUE(addr.setIp(hostv6));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(ipv6 + ":" + port, addr.toString());
    }
}

TEST(CSockAddr, setIp_str_5)
{
    const std::string host = "2001:0db8:85a3:08d3:1319:8a2e:0370:7344";
    const std::string ip = "[2001:db8:85a3:8d3:1319:8a2e:370:7344]";
    const std::string serv = "https";
    const std::string port = "443";
    const std::string old = ip + ":" + port;
    {   //1
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_TRUE(addr.setIp("123.4.5.6"));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ("123.4.5.6:" + port, addr.toString());
    }{  //1e
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_FALSE(addr.setIp("123.4.5.621"));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());
    }{  //2
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_TRUE(addr.setIp("localhost"));
        ASSERT_TRUE(addr.valid());
        ASSERT_NE("unknown", addr.toString());
        ASSERT_NE(old, addr.toString());
    }{  //2e
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_FALSE(addr.setIp("localhosty"));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());
    }if(hasNet){  //3
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        EXPECT_TRUE(addr.setIp("www.qq.com"))<<"This test needs network";
        EXPECT_TRUE(addr.valid());
        EXPECT_NE("unknown", addr.toString());
        ASSERT_NE(old, addr.toString());
    }if(hasNet){  //3e
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_FALSE(addr.setIp("www.qq.comadganegnd"));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());
    }{  //4
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_TRUE(addr.setIp(ifname));
        ASSERT_TRUE(addr.valid());
        ASSERT_NE("unknown", addr.toString());
        ASSERT_NE(old, addr.toString());
    }{  //4e
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_FALSE(addr.setIp("eth9999999999"));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());
    }{  //5
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_TRUE(addr.setIp("0.0.0.0"));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ("0.0.0.0:" + port, addr.toString());
    }{  //6.1
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_FALSE(addr.setIp(""));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());
    }{  //6.2
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_FALSE(addr.setIp(std::string()));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());
    }{  //7
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_TRUE(addr.setIp(hostv6));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(ipv6 + ":" + port, addr.toString());
    }
}

TEST(CSockAddr, setIp_ipv4_1)
{
    const std::string old = "unknown";
    const uint32_t ip = 0x12345678;
    const std::string now = CSockAddr::IPv4String(ip) + ":0";
    {
        CSockAddr addr;
        ASSERT_FALSE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_TRUE(addr.setIp(ip));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(now, addr.toString());
    }{
        CSockAddr addr;
        ASSERT_FALSE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_TRUE(addr.setIp(ip, true));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(now, addr.toString());
    }{
        CSockAddr addr;
        ASSERT_FALSE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_TRUE(addr.setIp(htonl(ip), false));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(now, addr.toString());
    }
}

TEST(CSockAddr, setIp_ipv4_2)
{
    const std::string host = "1.2.3.4";
    const std::string serv;
    const std::string old = host + ":0";
    const uint32_t ip = 0x12345678;
    const std::string now = CSockAddr::IPv4String(ip) + ":0";
    {
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_TRUE(addr.setIp(ip));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(now, addr.toString());
    }{
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_TRUE(addr.setIp(ip, true));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(now, addr.toString());
    }{
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_TRUE(addr.setIp(htonl(ip), false));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(now, addr.toString());
    }
}

TEST(CSockAddr, setIp_ipv4_3)
{
    const std::string host;
    const std::string serv = "http";
    const std::string old = "127.0.0.1:80";
    const std::string old6 = "[::1]:80";
    const uint32_t ip = 0x12345678;
    const std::string now = CSockAddr::IPv4String(ip) + ":80";
    {
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_TRUE(old == addr.toString() || old6 == addr.toString());

        ASSERT_TRUE(addr.setIp(ip));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(now, addr.toString());
    }{
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_TRUE(old == addr.toString() || old6 == addr.toString());

        ASSERT_TRUE(addr.setIp(ip, true));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(now, addr.toString());
    }{
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_TRUE(old == addr.toString() || old6 == addr.toString());

        ASSERT_TRUE(addr.setIp(htonl(ip), false));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(now, addr.toString());
    }
}

TEST(CSockAddr, setIp_ipv4_4)
{
    const std::string host = "1.2.3.4";
    const std::string serv = "http";
    const std::string old = "1.2.3.4:80";
    const uint32_t ip = 0x12345678;
    const std::string now = CSockAddr::IPv4String(ip) + ":80";
    {
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_TRUE(addr.setIp(ip));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(now, addr.toString());
    }{
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_TRUE(addr.setIp(ip, true));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(now, addr.toString());
    }{
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_TRUE(addr.setIp(htonl(ip), false));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(now, addr.toString());
    }
}

TEST(CSockAddr, setIp_ipv4_5)
{
    const std::string host = hostv6;
    const std::string serv;
    const std::string old = ipv6 + ":0";
    const uint32_t ip = 0x12345678;
    const std::string now = CSockAddr::IPv4String(ip) + ":0";
    {
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_TRUE(addr.setIp(ip));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(now, addr.toString());
    }{
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_TRUE(addr.setIp(ip, true));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(now, addr.toString());
    }{
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_TRUE(addr.setIp(htonl(ip), false));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(now, addr.toString());
    }
}

TEST(CSockAddr, setIp_ipv4_6)
{
    const std::string host = hostv6;
    const std::string serv = "http";
    const std::string old = ipv6 + ":80";
    const uint32_t ip = 0x12345678;
    const std::string now = CSockAddr::IPv4String(ip) + ":80";
    {
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_TRUE(addr.setIp(ip));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(now, addr.toString());
    }{
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_TRUE(addr.setIp(ip, true));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(now, addr.toString());
    }{
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_TRUE(addr.setIp(htonl(ip), false));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(now, addr.toString());
    }
}

TEST(CSockAddr, setPort_str)
{
    const std::string serv = "http";
    const std::string port = "80";
    {
        const std::string old = "unknown";
        const std::string now = "127.0.0.1:" + port;
        const std::string now6 = "[::1]:" + port;
        CSockAddr addr;
        ASSERT_FALSE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_TRUE(addr.setPort(serv));
        ASSERT_TRUE(addr.valid());
        ASSERT_TRUE(now == addr.toString() || now6 == addr.toString());
    }{
        const std::string host = "1.2.3.4";
        const std::string old = host + ":0";
        const std::string now = host + ":" + port;
        CSockAddr addr(host);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_TRUE(addr.setPort(serv));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(now, addr.toString());
    }{
        const std::string host;
        const std::string old_serv = "12";
        const std::string old = "127.0.0.1:" + old_serv;
        const std::string old6 = "[::1]:" + old_serv;
        const std::string now = "127.0.0.1:" + port;
        const std::string now6 = "[::1]:" + port;
        CSockAddr addr(host, old_serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_TRUE(old == addr.toString() || old6 == addr.toString());

        ASSERT_TRUE(addr.setPort(serv));
        ASSERT_TRUE(addr.valid());
        ASSERT_TRUE(now == addr.toString() || now6 == addr.toString());
    }{
        const std::string host = "1.2.3.4";
        const std::string old_serv = "12";
        const std::string old = host + ":" + old_serv;
        const std::string now = host + ":" + port;
        CSockAddr addr(host, old_serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_TRUE(addr.setPort(serv));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(now, addr.toString());
    }{
        const std::string host = hostv6;
        const std::string old = ipv6 + ":0";
        const std::string now = ipv6 + ":" + port;
        CSockAddr addr(host);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_TRUE(addr.setPort(serv));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(now, addr.toString());
    }{
        const std::string host = hostv6;
        const std::string old_serv = "12";
        const std::string old = ipv6 + ":" + old_serv;
        const std::string now = ipv6 + ":" + port;
        CSockAddr addr(host, old_serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_TRUE(addr.setPort(serv));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(now, addr.toString());
    }
}

TEST(CSockAddr, setPort_int_1)
{
    const std::string old = "unknown";
    const uint16_t port = 1234;
    const std::string now = "127.0.0.1:1234";
    const std::string now6 = "[::1]:1234";
    {
        CSockAddr addr;
        ASSERT_FALSE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_TRUE(addr.setPort(port));
        ASSERT_TRUE(addr.valid());
        ASSERT_TRUE(now == addr.toString() || now6 == addr.toString());
    }{
        CSockAddr addr;
        ASSERT_FALSE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_TRUE(addr.setPort(port, true));
        ASSERT_TRUE(addr.valid());
        ASSERT_TRUE(now == addr.toString() || now6 == addr.toString());
    }{
        CSockAddr addr;
        ASSERT_FALSE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_TRUE(addr.setPort(htons(port), false));
        ASSERT_TRUE(addr.valid());
        ASSERT_TRUE(now == addr.toString() || now6 == addr.toString());
    }
}

TEST(CSockAddr, setPort_int_2)
{
    const std::string host = "1.2.3.4";
    const std::string serv = "3333";
    const std::string old = host + ":" + serv;
    const uint16_t port = 1234;
    const std::string now = host + ":1234";
    {
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_TRUE(addr.setPort(port));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(now, addr.toString());
    }{
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_TRUE(addr.setPort(port, true));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(now, addr.toString());
    }{
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_TRUE(addr.setPort(htons(port), false));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(now, addr.toString());
    }
}

TEST(CSockAddr, setPort_int_3)
{
    const std::string host = hostv6;
    const std::string serv = "3333";
    const std::string old = ipv6 + ":" + serv;
    const uint16_t port = 1234;
    const std::string now = ipv6 + ":1234";
    {
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_TRUE(addr.setPort(port));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(now, addr.toString());
    }{
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_TRUE(addr.setPort(port, true));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(now, addr.toString());
    }{
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_TRUE(addr.setPort(htons(port), false));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(now, addr.toString());
    }
}

TEST(CSockAddr, setPort_int_4)
{
    const std::string host = "1.2.3.4";
    const std::string serv;
    const std::string old = host + ":0";
    const uint16_t port = 1234;
    const std::string now = host + ":1234";
    {
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_TRUE(addr.setPort(port));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(now, addr.toString());
    }{
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_TRUE(addr.setPort(port, true));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(now, addr.toString());
    }{
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_TRUE(addr.setPort(htons(port), false));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(now, addr.toString());
    }
}

TEST(CSockAddr, setPort_int_5)
{
    const std::string host = hostv6;
    const std::string serv;
    const std::string old = ipv6 + ":0";
    const uint16_t port = 1234;
    const std::string now = ipv6 + ":1234";
    {
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_TRUE(addr.setPort(port));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(now, addr.toString());
    }{
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_TRUE(addr.setPort(port, true));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(now, addr.toString());
    }{
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(old, addr.toString());

        ASSERT_TRUE(addr.setPort(htons(port), false));
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(now, addr.toString());
    }
}

TEST(CSockAddr, setPort_int_6)
{
    const std::string host;
    const std::string serv = "3333";
    const std::string old = "127.0.0.1:3333";
    const std::string old6 = "[::1]:3333";
    const uint16_t port = 1234;
    const std::string now = "127.0.0.1:1234";
    const std::string now6 = "[::1]:1234";
    {
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_TRUE(old == addr.toString() || old6 == addr.toString());

        ASSERT_TRUE(addr.setPort(port));
        ASSERT_TRUE(addr.valid());
        ASSERT_TRUE(now == addr.toString() || now6 == addr.toString());
    }{
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_TRUE(old == addr.toString() || old6 == addr.toString());

        ASSERT_TRUE(addr.setPort(port, true));
        ASSERT_TRUE(addr.valid());
        ASSERT_TRUE(now == addr.toString() || now6 == addr.toString());
    }{
        CSockAddr addr(host, serv);
        ASSERT_TRUE(addr.valid());
        ASSERT_TRUE(old == addr.toString() || old6 == addr.toString());

        ASSERT_TRUE(addr.setPort(htons(port), false));
        ASSERT_TRUE(addr.valid());
        ASSERT_TRUE(now == addr.toString() || now6 == addr.toString());
    }
}

TEST(CSockAddr, getIpv4)
{
    {
        CSockAddr addr;
        ASSERT_FALSE(addr.valid());
        ASSERT_EQ("unknown", addr.toString());

        uint32_t ip = 0;
        ASSERT_EQ(ip, addr.getIpv4(false));
        ASSERT_EQ(htonl(ip), addr.getIpv4());
        ASSERT_EQ(htonl(ip), addr.getIpv4(true));
    }{
        CSockAddr addr("12.34.56.78");
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ("12.34.56.78:0", addr.toString());

        uint32_t ip = 0x4E38220C;
        ASSERT_EQ(ip, addr.getIpv4(false));
        ASSERT_EQ(htonl(ip), addr.getIpv4());
        ASSERT_EQ(htonl(ip), addr.getIpv4(true));
    }{
        CSockAddr addr(hostv6);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(ipv6 + ":0", addr.toString());

        uint32_t ip = 0;
        ASSERT_EQ(ip, addr.getIpv4(false));
        ASSERT_EQ(htonl(ip), addr.getIpv4());
        ASSERT_EQ(htonl(ip), addr.getIpv4(true));
    }{
        CSockAddr addr("12.34.56.78", "http");
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ("12.34.56.78:80", addr.toString());

        uint32_t ip = 0x4E38220C;
        ASSERT_EQ(ip, addr.getIpv4(false));
        ASSERT_EQ(htonl(ip), addr.getIpv4());
        ASSERT_EQ(htonl(ip), addr.getIpv4(true));
    }{
        CSockAddr addr(hostv6, "http");
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(ipv6 + ":80", addr.toString());

        uint32_t ip = 0;
        ASSERT_EQ(ip, addr.getIpv4(false));
        ASSERT_EQ(htonl(ip), addr.getIpv4());
        ASSERT_EQ(htonl(ip), addr.getIpv4(true));
    }{
        CSockAddr addr("", "http");
        ASSERT_TRUE(addr.valid());
        ASSERT_TRUE("127.0.0.1:80" == addr.toString() || "[::1]:80" == addr.toString());

        uint32_t ip = 0x100007F;
        ASSERT_TRUE(ip == addr.getIpv4(false) || 0 == addr.getIpv4(false));
        ASSERT_TRUE(htonl(ip) == addr.getIpv4() || 0 == addr.getIpv4());
        ASSERT_TRUE(htonl(ip) == addr.getIpv4(true) || 0 == addr.getIpv4(true));
    }
}

TEST(CSockAddr, getPort)
{
    {
        CSockAddr addr;
        ASSERT_FALSE(addr.valid());
        ASSERT_EQ("unknown", addr.toString());

        uint16_t port = 0;
        ASSERT_EQ(port, addr.getPort(false));
        ASSERT_EQ(htons(port), addr.getPort());
        ASSERT_EQ(htons(port), addr.getPort(true));
    }{
        CSockAddr addr("12.34.56.78");
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ("12.34.56.78:0", addr.toString());

        uint16_t port = 0;
        ASSERT_EQ(port, addr.getPort(false));
        ASSERT_EQ(htons(port), addr.getPort());
        ASSERT_EQ(htons(port), addr.getPort(true));
    }{
        CSockAddr addr(hostv6);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(ipv6 + ":0", addr.toString());

        uint16_t port = 0;
        ASSERT_EQ(port, addr.getPort(false));
        ASSERT_EQ(htons(port), addr.getPort());
        ASSERT_EQ(htons(port), addr.getPort(true));
    }{
        CSockAddr addr("12.34.56.78", "http");
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ("12.34.56.78:80", addr.toString());

        uint16_t port = 80;
        ASSERT_EQ(htons(port), addr.getPort(false));
        ASSERT_EQ(port, addr.getPort());
        ASSERT_EQ(port, addr.getPort(true));
    }{
        CSockAddr addr(hostv6, "http");
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(ipv6 + ":80", addr.toString());

        uint16_t port = 80;
        ASSERT_EQ(htons(port), addr.getPort(false));
        ASSERT_EQ(port, addr.getPort());
        ASSERT_EQ(port, addr.getPort(true));
    }{
        CSockAddr addr("", "http");
        ASSERT_TRUE(addr.valid());
        ASSERT_TRUE("127.0.0.1:80" == addr.toString() || "[::1]:80" == addr.toString());

        uint16_t port = 80;
        ASSERT_EQ(htons(port), addr.getPort(false));
        ASSERT_EQ(port, addr.getPort());
        ASSERT_EQ(port, addr.getPort(true));
    }
}

TEST(CSockAddr, clear)
{
    {
        CSockAddr addr;
        ASSERT_FALSE(addr.valid());
        ASSERT_EQ("unknown", addr.toString());

        addr.clear();
        ASSERT_FALSE(addr.valid());
        ASSERT_EQ("unknown", addr.toString());
    }{
        CSockAddr addr("12.34.56.78");
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ("12.34.56.78:0", addr.toString());

        addr.clear();
        ASSERT_FALSE(addr.valid());
        ASSERT_EQ("unknown", addr.toString());
    }{
        CSockAddr addr(hostv6);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(ipv6 + ":0", addr.toString());

        addr.clear();
        ASSERT_FALSE(addr.valid());
        ASSERT_EQ("unknown", addr.toString());
    }{
        CSockAddr addr("12.34.56.78", "http");
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ("12.34.56.78:80", addr.toString());

        addr.clear();
        ASSERT_FALSE(addr.valid());
        ASSERT_EQ("unknown", addr.toString());
    }{
        CSockAddr addr(hostv6, "http");
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ(ipv6 + ":80", addr.toString());

        addr.clear();
        ASSERT_FALSE(addr.valid());
        ASSERT_EQ("unknown", addr.toString());
    }{
        CSockAddr addr("", "http");
        ASSERT_TRUE(addr.valid());
        ASSERT_TRUE("127.0.0.1:80" == addr.toString() || "[::1]:80" == addr.toString());

        addr.clear();
        ASSERT_FALSE(addr.valid());
        ASSERT_EQ("unknown", addr.toString());
    }
}

TEST(CSockAddr, SA4)
{
    const std::string ip = "12.34.56.78";
    const std::string port = "80";

    CSockAddr tmp(ip, port);
    EXPECT_TRUE(tmp.valid());
    EXPECT_EQ(ip + ":" + port, tmp.toString());
    EXPECT_EQ(AF_INET, tmp.family());
    const struct sockaddr * const sa = tmp.sockaddr();
    const socklen_t len = tmp.socklen();
    EXPECT_EQ(sizeof(struct sockaddr_in), len);
    {
        CSockAddr addr(*(const struct sockaddr_storage *)sa, len);
        EXPECT_TRUE(addr.valid());
        EXPECT_EQ(ip + ":" + port, addr.toString());
    }{
        CSockAddr addr(*(const struct sockaddr *)sa, len);
        EXPECT_TRUE(addr.valid());
        EXPECT_EQ(ip + ":" + port, addr.toString());
    }{
        CSockAddr addr(*(const struct sockaddr_in *)sa);
        EXPECT_TRUE(addr.valid());
        EXPECT_EQ(ip + ":" + port, addr.toString());
    }{
        CSockAddr addr;
        EXPECT_FALSE(addr.valid());
        EXPECT_EQ("unknown", addr.toString());

        addr.setAddr(*(const struct sockaddr_storage *)sa, len);
        EXPECT_TRUE(addr.valid());
        EXPECT_EQ(ip + ":" + port, addr.toString());
    }{
        CSockAddr addr;
        EXPECT_FALSE(addr.valid());
        EXPECT_EQ("unknown", addr.toString());

        addr.setAddr(*(const struct sockaddr *)sa, len);
        EXPECT_TRUE(addr.valid());
        EXPECT_EQ(ip + ":" + port, addr.toString());
    }{
        CSockAddr addr;
        EXPECT_FALSE(addr.valid());
        EXPECT_EQ("unknown", addr.toString());

        addr.setAddr(*(const struct sockaddr_in *)sa);
        EXPECT_TRUE(addr.valid());
        EXPECT_EQ(ip + ":" + port, addr.toString());
    }
}

TEST(CSockAddr, SA6)
{
    const std::string ip = ipv6;
    const std::string port = "80";

    CSockAddr tmp(hostv6, port);
    EXPECT_TRUE(tmp.valid());
    EXPECT_EQ(ip + ":" + port, tmp.toString());
    EXPECT_EQ(AF_INET6, tmp.family());
    const struct sockaddr * const sa = tmp.sockaddr();
    const socklen_t len = tmp.socklen();
    EXPECT_EQ(sizeof(struct sockaddr_in6), len);
    {
        CSockAddr addr(*(const struct sockaddr_storage *)sa, len);
        EXPECT_TRUE(addr.valid());
        EXPECT_EQ(ip + ":" + port, addr.toString());
    }{
        CSockAddr addr(*(const struct sockaddr *)sa, len);
        EXPECT_TRUE(addr.valid());
        EXPECT_EQ(ip + ":" + port, addr.toString());
    }{
        CSockAddr addr(*(const struct sockaddr_in6 *)sa);
        EXPECT_TRUE(addr.valid());
        EXPECT_EQ(ip + ":" + port, addr.toString());
    }{
        CSockAddr addr;
        EXPECT_FALSE(addr.valid());
        EXPECT_EQ("unknown", addr.toString());

        addr.setAddr(*(const struct sockaddr_storage *)sa, len);
        EXPECT_TRUE(addr.valid());
        EXPECT_EQ(ip + ":" + port, addr.toString());
    }{
        CSockAddr addr;
        EXPECT_FALSE(addr.valid());
        EXPECT_EQ("unknown", addr.toString());

        addr.setAddr(*(const struct sockaddr *)sa, len);
        EXPECT_TRUE(addr.valid());
        EXPECT_EQ(ip + ":" + port, addr.toString());
    }{
        CSockAddr addr;
        EXPECT_FALSE(addr.valid());
        EXPECT_EQ("unknown", addr.toString());

        addr.setAddr(*(const struct sockaddr_in6 *)sa);
        EXPECT_TRUE(addr.valid());
        EXPECT_EQ(ip + ":" + port, addr.toString());
    }
}
