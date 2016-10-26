#include <marine/sockets.hh>

#include "../inc.h"
#include "comm.h"

TEST(CListenSocket, ctor)
{
    {
        CListenSocket s;
        ASSERT_FALSE(s.valid());
        ASSERT_EQ(-1, s.fd());
        ASSERT_TRUE(CListenSocket::kFdType == s.fdType());
        ASSERT_STREQ("CListenSocket", s.fdTypeName());
    }{
        CListenSocket s(AF_INET);
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());
        ASSERT_TRUE(CListenSocket::kFdType == s.fdType());
        ASSERT_STREQ("CListenSocket", s.fdTypeName());
    }{
        CListenSocket s(AF_INET6);
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());
        ASSERT_TRUE(CListenSocket::kFdType == s.fdType());
        ASSERT_STREQ("CListenSocket", s.fdTypeName());
    }{
        CListenSocket s(AF_UNSPEC);
        ASSERT_FALSE(s.valid())<<CSocket::ErrMsg();   //Address family not supported by protocol
        ASSERT_EQ(-1, s.fd());
        ASSERT_TRUE(CListenSocket::kFdType == s.fdType());
        ASSERT_STREQ("CListenSocket", s.fdTypeName());
    }{
        CListenSocket s(-1);
        ASSERT_FALSE(s.valid());
        ASSERT_EQ(-1, s.fd());
        ASSERT_TRUE(CListenSocket::kFdType == s.fdType());
        ASSERT_STREQ("CListenSocket", s.fdTypeName());
    }
}

TEST(CListenSocket, getSock)
{
    {
        CListenSocket s;
        ASSERT_FALSE(s.valid());
        ASSERT_EQ(-1, s.fd());

        ASSERT_TRUE(s.getSock(AF_INET));
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());

        ASSERT_FALSE(s.getSock(AF_INET));
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());
    }{
        CListenSocket s;
        ASSERT_FALSE(s.valid());
        ASSERT_EQ(-1, s.fd());

        ASSERT_TRUE(s.getSock(AF_INET6));
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());
    }{
        CListenSocket s;
        ASSERT_FALSE(s.valid());
        ASSERT_EQ(-1, s.fd());

        ASSERT_FALSE(s.getSock(AF_UNSPEC));
        ASSERT_FALSE(s.valid())<<CSocket::ErrMsg();   //Address family not supported by protocol
        ASSERT_EQ(-1, s.fd());
    }{
        CListenSocket s;
        ASSERT_FALSE(s.valid());
        ASSERT_EQ(-1, s.fd());

        ASSERT_FALSE(s.getSock(-1));
        ASSERT_FALSE(s.valid());
        ASSERT_EQ(-1, s.fd());
    }
}

TEST(CListenSocket, hostAddr)
{
    {
        CListenSocket s;
        ASSERT_FALSE(s.valid());
        ASSERT_EQ(-1, s.fd());

        CSockAddr result;
        ASSERT_FALSE(result.valid());
        ASSERT_EQ("unknown", result.toString());

        result = s.hostAddr();
        ASSERT_FALSE(result.valid());
        ASSERT_EQ("unknown", result.toString());
    }{
        CListenSocket s(AF_INET);
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());

        CSockAddr result;
        ASSERT_FALSE(result.valid());
        ASSERT_EQ("unknown", result.toString());

        result = s.hostAddr();
        ASSERT_TRUE(result.valid());
        ASSERT_EQ("0.0.0.0:0", result.toString());
    }{
        CSockAddr addr("127.0.0.1", 8000);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ("127.0.0.1:8000", addr.toString());

        CListenSocket s;
        ASSERT_TRUE(s.listenAddr(addr));
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());

        CSockAddr result;
        ASSERT_FALSE(result.valid());
        ASSERT_EQ("unknown", result.toString());

        result = s.hostAddr();
        ASSERT_TRUE(result.valid());
        ASSERT_EQ(addr.toString(), result.toString());
    }
}

TEST(CListenSocket, peerAddr)
{
    {
        CListenSocket s;
        ASSERT_FALSE(s.valid());
        ASSERT_EQ(-1, s.fd());

        CSockAddr result;
        ASSERT_FALSE(result.valid());
        ASSERT_EQ("unknown", result.toString());

        result = s.peerAddr();
        ASSERT_FALSE(result.valid());
        ASSERT_EQ("unknown", result.toString());
    }{
        CListenSocket s(AF_INET);
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());

        CSockAddr result;
        ASSERT_FALSE(result.valid());
        ASSERT_EQ("unknown", result.toString());

        result = s.peerAddr();
        ASSERT_FALSE(result.valid());
        ASSERT_EQ("unknown", result.toString());
    }{
        CSockAddr addr("127.0.0.1", 8000);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ("127.0.0.1:8000", addr.toString());

        CListenSocket s;
        ASSERT_TRUE(s.listenAddr(addr));
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());

        CSockAddr result;
        ASSERT_FALSE(result.valid());
        ASSERT_EQ("unknown", result.toString());

        result = s.peerAddr();
        ASSERT_FALSE(result.valid());
        ASSERT_EQ("unknown", result.toString());
    }
}

TEST(CListenSocket, linger)
{
    {
        CListenSocket s;
        ASSERT_FALSE(s.valid());
        ASSERT_EQ(-1, s.fd());

        ASSERT_FALSE(s.linger(true, 0));
        ASSERT_FALSE(s.valid());
        ASSERT_EQ(-1, s.fd());

        ASSERT_FALSE(s.linger(true, 100));
        ASSERT_FALSE(s.valid());
        ASSERT_EQ(-1, s.fd());

        ASSERT_FALSE(s.linger(true));
        ASSERT_FALSE(s.valid());
        ASSERT_EQ(-1, s.fd());

        ASSERT_FALSE(s.linger(false, 100));
        ASSERT_FALSE(s.valid());
        ASSERT_EQ(-1, s.fd());

        ASSERT_FALSE(s.linger(false));
        ASSERT_FALSE(s.valid());
        ASSERT_EQ(-1, s.fd());
    }{
        CListenSocket s(AF_INET);
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());

        ASSERT_TRUE(s.linger(true, 0));
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());

        ASSERT_TRUE(s.linger(true, 100));
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());

        ASSERT_TRUE(s.linger(true));
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());

        ASSERT_TRUE(s.linger(false, 100));
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());

        ASSERT_TRUE(s.linger(false));
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());
    }{
        CListenSocket s(AF_INET6);
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());

        ASSERT_TRUE(s.linger(true, 0));
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());

        ASSERT_TRUE(s.linger(true, 100));
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());

        ASSERT_TRUE(s.linger(true));
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());

        ASSERT_TRUE(s.linger(false, 100));
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());

        ASSERT_TRUE(s.linger(false));
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());
    }
}

TEST(CListenSocket, reuseAddr)
{
    {
        CListenSocket s;
        ASSERT_FALSE(s.valid());
        ASSERT_EQ(-1, s.fd());

        ASSERT_FALSE(s.reuseAddr(true));
        ASSERT_FALSE(s.valid());
        ASSERT_EQ(-1, s.fd());

        ASSERT_FALSE(s.reuseAddr(false));
        ASSERT_FALSE(s.valid());
        ASSERT_EQ(-1, s.fd());
    }{
        CListenSocket s(AF_INET);
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());

        ASSERT_TRUE(s.reuseAddr(true));
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());

        ASSERT_TRUE(s.reuseAddr(false));
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());
    }{
        CListenSocket s(AF_INET6);
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());

        ASSERT_TRUE(s.reuseAddr(true));
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());

        ASSERT_TRUE(s.reuseAddr(false));
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());
    }
}

TEST(CListenSocket, recvTimeout)
{
    {
        CListenSocket s;
        ASSERT_FALSE(s.valid());
        ASSERT_EQ(-1, s.fd());

        ASSERT_GT(0, s.recvTimeout());
        ASSERT_FALSE(s.valid());
        ASSERT_EQ(-1, s.fd());

        ASSERT_FALSE(s.recvTimeout(3100));
        ASSERT_FALSE(s.valid());
        ASSERT_EQ(-1, s.fd());

        ASSERT_GT(0, s.recvTimeout());
        ASSERT_FALSE(s.valid());
        ASSERT_EQ(-1, s.fd());
    }{
        CListenSocket s(AF_INET);
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());

        ASSERT_EQ(0, s.recvTimeout());
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());

        ASSERT_TRUE(s.recvTimeout(3100));
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());

        ASSERT_EQ(3100, s.recvTimeout());
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());
    }{
        CListenSocket s(AF_INET6);
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());

        ASSERT_EQ(0, s.recvTimeout());
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());

        ASSERT_TRUE(s.recvTimeout(3100));
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());

        ASSERT_EQ(3100, s.recvTimeout());
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());
    }
}

TEST(CListenSocket, sendTimeout)
{
    {
        CListenSocket s;
        ASSERT_FALSE(s.valid());
        ASSERT_EQ(-1, s.fd());

        ASSERT_GT(0, s.sendTimeout());
        ASSERT_FALSE(s.valid());
        ASSERT_EQ(-1, s.fd());

        ASSERT_FALSE(s.sendTimeout(3100));
        ASSERT_FALSE(s.valid());
        ASSERT_EQ(-1, s.fd());

        ASSERT_GT(0, s.sendTimeout());
        ASSERT_FALSE(s.valid());
        ASSERT_EQ(-1, s.fd());
    }{
        CListenSocket s(AF_INET);
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());

        ASSERT_EQ(0, s.sendTimeout());
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());

        ASSERT_TRUE(s.sendTimeout(3100));
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());

        ASSERT_EQ(3100, s.sendTimeout());
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());
    }{
        CListenSocket s(AF_INET6);
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());

        ASSERT_EQ(0, s.sendTimeout());
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());

        ASSERT_TRUE(s.sendTimeout(3100));
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());

        ASSERT_EQ(3100, s.sendTimeout());
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());
    }
}

TEST(CListenSocket, recvBufSize)
{
    {
        CListenSocket s;
        ASSERT_FALSE(s.valid());
        ASSERT_EQ(-1, s.fd());

        ASSERT_GT(0, s.recvBufSize());
        ASSERT_FALSE(s.valid());
        ASSERT_EQ(-1, s.fd());

        ASSERT_FALSE(s.recvBufSize(3100));
        ASSERT_FALSE(s.valid());
        ASSERT_EQ(-1, s.fd());

        ASSERT_GT(0, s.recvBufSize());
        ASSERT_FALSE(s.valid());
        ASSERT_EQ(-1, s.fd());
    }{
        CListenSocket s(AF_INET);
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());

        ASSERT_LT(0, s.recvBufSize());
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());

        int sz = 8192;
        ASSERT_TRUE(s.recvBufSize(sz));
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());

        ASSERT_EQ(2 * sz, s.recvBufSize());
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());

        sz = 0;
        ASSERT_TRUE(s.recvBufSize(sz));
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());

        EXPECT_TRUE(validBufSize(s.recvBufSize()));
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());
    }{
        CListenSocket s(AF_INET6);
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());

        ASSERT_LT(0, s.recvBufSize());
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());

        int sz = 8192;
        ASSERT_TRUE(s.recvBufSize(sz));
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());

        ASSERT_EQ(2 * sz, s.recvBufSize());
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());

        sz = 0;
        ASSERT_TRUE(s.recvBufSize(sz));
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());

        EXPECT_TRUE(validBufSize(s.recvBufSize()));
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());
    }
}

TEST(CListenSocket, sendBufSize)
{
    {
        CListenSocket s;
        ASSERT_FALSE(s.valid());
        ASSERT_EQ(-1, s.fd());

        ASSERT_GT(0, s.sendBufSize());
        ASSERT_FALSE(s.valid());
        ASSERT_EQ(-1, s.fd());

        ASSERT_FALSE(s.sendBufSize(3100));
        ASSERT_FALSE(s.valid());
        ASSERT_EQ(-1, s.fd());

        ASSERT_GT(0, s.sendBufSize());
        ASSERT_FALSE(s.valid());
        ASSERT_EQ(-1, s.fd());
    }{
        CListenSocket s(AF_INET);
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());

        ASSERT_LT(0, s.sendBufSize());
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());

        int sz = 8192;
        ASSERT_TRUE(s.sendBufSize(sz));
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());

        ASSERT_EQ(2 * sz, s.sendBufSize());
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());

        sz = 0;
        ASSERT_TRUE(s.sendBufSize(sz));
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());

        EXPECT_TRUE(validSendSize(s.sendBufSize()));
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());
    }{
        CListenSocket s(AF_INET6);
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());

        ASSERT_LT(0, s.sendBufSize());
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());

        int sz = 8192;
        ASSERT_TRUE(s.sendBufSize(sz));
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());

        ASSERT_EQ(2 * sz, s.sendBufSize());
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());

        sz = 0;
        ASSERT_TRUE(s.sendBufSize(sz));
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());

        EXPECT_TRUE(validSendSize(s.sendBufSize()));
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());
    }
}

TEST(CListenSocket, toString)
{
    {
        CListenSocket s;
        ASSERT_FALSE(s.valid());
        ASSERT_EQ(-1, s.fd());
        ASSERT_EQ("{IFileDesc={fd_=-1(CListenSocket)}, host=unknown, peer=unknown}", s.toString());
    }{
        CListenSocket s(AF_INET);
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());
        std::ostringstream oss;
        oss<<"{IFileDesc={fd_="<<s.fd()<<"(CListenSocket)}, host=0.0.0.0:0, peer=unknown}";
        ASSERT_EQ(oss.str(), s.toString());
    }{
        CSockAddr addr("127.0.0.1", 8000);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ("127.0.0.1:8000", addr.toString());

        CListenSocket s;
        ASSERT_TRUE(s.listenAddr(addr));
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());
        std::ostringstream oss;
        oss<<"{IFileDesc={fd_="<<s.fd()<<"(CListenSocket)}, host="<<addr.toString()<<", peer=unknown}";
        ASSERT_EQ(oss.str(), s.toString());
    }
}

TEST(CListenSocket, listenAddr)
{
    {
        CSockAddr addr("127.0.0.1", 8000);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ("127.0.0.1:8000", addr.toString());
        {
            CListenSocket s;
            ASSERT_FALSE(s.valid());
            ASSERT_EQ(-1, s.fd());

            ASSERT_TRUE(s.listenAddr(addr, 100));
            ASSERT_TRUE(s.valid());
            ASSERT_NE(-1, s.fd());

            CSockAddr result;
            ASSERT_FALSE(result.valid());
            ASSERT_EQ("unknown", result.toString());

            result = s.hostAddr();
            ASSERT_TRUE(result.valid());
            ASSERT_EQ(addr.toString(), result.toString());
        }{
            CListenSocket s;
            ASSERT_FALSE(s.valid());
            ASSERT_EQ(-1, s.fd());

            ASSERT_TRUE(s.listenAddr(addr));
            ASSERT_TRUE(s.valid());
            ASSERT_NE(-1, s.fd());

            CSockAddr result;
            ASSERT_FALSE(result.valid());
            ASSERT_EQ("unknown", result.toString());

            result = s.hostAddr();
            ASSERT_TRUE(result.valid());
            ASSERT_EQ(addr.toString(), result.toString());
        }{
            CListenSocket s;
            ASSERT_FALSE(s.valid());
            ASSERT_EQ(-1, s.fd());

            ASSERT_TRUE(s.listenAddr(addr, 100, true));
            ASSERT_TRUE(s.valid());
            ASSERT_NE(-1, s.fd());

            CSockAddr result;
            ASSERT_FALSE(result.valid());
            ASSERT_EQ("unknown", result.toString());

            result = s.hostAddr();
            ASSERT_TRUE(result.valid());
            ASSERT_EQ(addr.toString(), result.toString());
        }{
            CListenSocket s;
            ASSERT_FALSE(s.valid());
            ASSERT_EQ(-1, s.fd());

            ASSERT_TRUE(s.listenAddr(addr, 100, false));
            ASSERT_TRUE(s.valid());
            ASSERT_NE(-1, s.fd());

            CSockAddr result;
            ASSERT_FALSE(result.valid());
            ASSERT_EQ("unknown", result.toString());

            result = s.hostAddr();
            ASSERT_TRUE(result.valid());
            ASSERT_EQ(addr.toString(), result.toString());
        }
    }{
        CSockAddr addr("::ffff:127.0.0.1", 8000);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ("[::ffff:127.0.0.1]:8000", addr.toString());
        const char * addrStr = "0.0.0.0:8000";  //should be addr.toString()
        {
            CListenSocket s;
            ASSERT_FALSE(s.valid());
            ASSERT_EQ(-1, s.fd());

            if(s.listenAddr(addr, 100)){
                ASSERT_TRUE(s.valid());
                ASSERT_NE(-1, s.fd());

                CSockAddr result;
                ASSERT_FALSE(result.valid());
                ASSERT_EQ("unknown", result.toString());

                result = s.hostAddr();
                ASSERT_TRUE(result.valid());
                EXPECT_EQ(addrStr, result.toString());
            }else{
                cerr<<CSocket::ErrMsg()<<endl;

                CSockAddr result;
                ASSERT_FALSE(result.valid());
                ASSERT_EQ("unknown", result.toString());

                result = s.hostAddr();
                ASSERT_TRUE(result.valid());
                EXPECT_EQ("0.0.0.0:0", result.toString());
            }
        }{
            CListenSocket s;
            ASSERT_FALSE(s.valid());
            ASSERT_EQ(-1, s.fd());

            if(s.listenAddr(addr)){
                ASSERT_TRUE(s.valid());
                ASSERT_NE(-1, s.fd());

                CSockAddr result;
                ASSERT_FALSE(result.valid());
                ASSERT_EQ("unknown", result.toString());

                result = s.hostAddr();
                ASSERT_TRUE(result.valid());
                EXPECT_EQ(addrStr, result.toString());
            }else{
                cerr<<CSocket::ErrMsg()<<endl;

                CSockAddr result;
                ASSERT_FALSE(result.valid());
                ASSERT_EQ("unknown", result.toString());

                result = s.hostAddr();
                ASSERT_TRUE(result.valid());
                EXPECT_EQ("0.0.0.0:0", result.toString());
            }
        }{
            CListenSocket s;
            ASSERT_FALSE(s.valid());
            ASSERT_EQ(-1, s.fd());

            if(s.listenAddr(addr, 100, true)){
                ASSERT_TRUE(s.valid());
                ASSERT_NE(-1, s.fd());

                CSockAddr result;
                ASSERT_FALSE(result.valid());
                ASSERT_EQ("unknown", result.toString());

                result = s.hostAddr();
                ASSERT_TRUE(result.valid());
                EXPECT_EQ(addrStr, result.toString());
            }else{
                cerr<<CSocket::ErrMsg()<<endl;

                CSockAddr result;
                ASSERT_FALSE(result.valid());
                ASSERT_EQ("unknown", result.toString());

                result = s.hostAddr();
                ASSERT_TRUE(result.valid());
                EXPECT_EQ("0.0.0.0:0", result.toString());
            }
        }{
            CListenSocket s;
            ASSERT_FALSE(s.valid());
            ASSERT_EQ(-1, s.fd());

            if(s.listenAddr(addr, 100, false)){
                ASSERT_TRUE(s.valid());
                ASSERT_NE(-1, s.fd());

                CSockAddr result;
                ASSERT_FALSE(result.valid());
                ASSERT_EQ("unknown", result.toString());

                result = s.hostAddr();
                ASSERT_TRUE(result.valid());
                EXPECT_EQ(addrStr, result.toString());
            }else{
                cerr<<CSocket::ErrMsg()<<endl;

                CSockAddr result;
                ASSERT_FALSE(result.valid());
                ASSERT_EQ("unknown", result.toString());

                result = s.hostAddr();
                ASSERT_TRUE(result.valid());
                EXPECT_EQ("0.0.0.0:0", result.toString());
            }
        }
    }{
        CSockAddr addr;
        ASSERT_FALSE(addr.valid());
        ASSERT_EQ("unknown", addr.toString());
        {
            CListenSocket s;
            ASSERT_FALSE(s.valid());
            ASSERT_EQ(-1, s.fd());

            ASSERT_FALSE(s.listenAddr(addr, 100));
            ASSERT_FALSE(s.valid());
            ASSERT_EQ(-1, s.fd());
        }{
            CListenSocket s;
            ASSERT_FALSE(s.valid());
            ASSERT_EQ(-1, s.fd());

            ASSERT_FALSE(s.listenAddr(addr));
            ASSERT_FALSE(s.valid());
            ASSERT_EQ(-1, s.fd());
        }{
            CListenSocket s;
            ASSERT_FALSE(s.valid());
            ASSERT_EQ(-1, s.fd());

            ASSERT_FALSE(s.listenAddr(addr, 100, true));
            ASSERT_FALSE(s.valid());
            ASSERT_EQ(-1, s.fd());
        }{
            CListenSocket s;
            ASSERT_FALSE(s.valid());
            ASSERT_EQ(-1, s.fd());

            ASSERT_FALSE(s.listenAddr(addr, 100, false));
            ASSERT_FALSE(s.valid());
            ASSERT_EQ(-1, s.fd());
        }
    }
}
