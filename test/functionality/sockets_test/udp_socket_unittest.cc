#include <marine/sockets.hh>

#include "../inc.h"
#include "comm.h"

TEST(CUdpSocket, ctor)
{
    {
        CUdpSocket s;
        ASSERT_FALSE(s.valid());
        ASSERT_EQ(-1, s.fd());
        ASSERT_TRUE(CUdpSocket::kFdType == s.fdType());
        ASSERT_STREQ("CUdpSocket", s.fdTypeName());
    }{
        CUdpSocket s(AF_INET);
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());
        ASSERT_TRUE(CUdpSocket::kFdType == s.fdType());
        ASSERT_STREQ("CUdpSocket", s.fdTypeName());
    }{
        CUdpSocket s(AF_INET6);
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());
        ASSERT_TRUE(CUdpSocket::kFdType == s.fdType());
        ASSERT_STREQ("CUdpSocket", s.fdTypeName());
    }{
        CUdpSocket s(AF_UNSPEC);
        ASSERT_FALSE(s.valid())<<CSocket::ErrMsg();   //Address family not supported by protocol
        ASSERT_EQ(-1, s.fd());
        ASSERT_TRUE(CUdpSocket::kFdType == s.fdType());
        ASSERT_STREQ("CUdpSocket", s.fdTypeName());
    }{
        CUdpSocket s(-1);
        ASSERT_FALSE(s.valid());
        ASSERT_EQ(-1, s.fd());
        ASSERT_TRUE(CUdpSocket::kFdType == s.fdType());
        ASSERT_STREQ("CUdpSocket", s.fdTypeName());
    }
}

TEST(CUdpSocket, getSock)
{
    {
        CUdpSocket s;
        ASSERT_FALSE(s.valid());
        ASSERT_EQ(-1, s.fd());

        ASSERT_TRUE(s.getSock(AF_INET));
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());

        ASSERT_FALSE(s.getSock(AF_INET));
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());
    }{
        CUdpSocket s;
        ASSERT_FALSE(s.valid());
        ASSERT_EQ(-1, s.fd());

        ASSERT_TRUE(s.getSock(AF_INET6));
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());
    }{
        CUdpSocket s;
        ASSERT_FALSE(s.valid());
        ASSERT_EQ(-1, s.fd());

        ASSERT_FALSE(s.getSock(AF_UNSPEC));
        ASSERT_FALSE(s.valid())<<CSocket::ErrMsg();   //Address family not supported by protocol
        ASSERT_EQ(-1, s.fd());
    }{
        CUdpSocket s;
        ASSERT_FALSE(s.valid());
        ASSERT_EQ(-1, s.fd());

        ASSERT_FALSE(s.getSock(-1));
        ASSERT_FALSE(s.valid());
        ASSERT_EQ(-1, s.fd());
    }
}

TEST(CUdpSocket, hostAddr)
{
    {
        CUdpSocket s;
        ASSERT_FALSE(s.valid());
        ASSERT_EQ(-1, s.fd());

        CSockAddr result;
        ASSERT_FALSE(result.valid());
        ASSERT_EQ("unknown", result.toString());

        result = s.hostAddr();
        ASSERT_FALSE(result.valid());
        ASSERT_EQ("unknown", result.toString());
    }{
        CUdpSocket s(AF_INET);
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

        CSockAddr addr2("127.0.0.1", 9000);
        ASSERT_TRUE(addr2.valid());
        ASSERT_EQ("127.0.0.1:9000", addr2.toString());
        {
            CUdpSocket s;
            ASSERT_TRUE(s.bindAddr(addr));
            ASSERT_TRUE(s.valid());
            ASSERT_NE(-1, s.fd());

            CSockAddr result;
            ASSERT_FALSE(result.valid());
            ASSERT_EQ("unknown", result.toString());

            result = s.hostAddr();
            ASSERT_TRUE(result.valid());
            ASSERT_EQ(addr.toString(), result.toString());
        }{
            CUdpSocket s;
            ASSERT_TRUE(s.connectAddr(addr2));
            ASSERT_TRUE(s.valid());
            ASSERT_NE(-1, s.fd());

            CSockAddr result;
            ASSERT_FALSE(result.valid());
            ASSERT_EQ("unknown", result.toString());

            result = s.hostAddr();
            ASSERT_TRUE(result.valid());    //port is random
        }{
            CUdpSocket s;
            ASSERT_TRUE(s.bindAddr(addr));
            ASSERT_TRUE(s.connectAddr(addr2));
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
}

TEST(CUdpSocket, peerAddr)
{
    {
        CUdpSocket s;
        ASSERT_FALSE(s.valid());
        ASSERT_EQ(-1, s.fd());

        CSockAddr result;
        ASSERT_FALSE(result.valid());
        ASSERT_EQ("unknown", result.toString());

        result = s.peerAddr();
        ASSERT_FALSE(result.valid());
        ASSERT_EQ("unknown", result.toString());
    }{
        CUdpSocket s(AF_INET);
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

        CSockAddr addr2("127.0.0.1", 9000);
        ASSERT_TRUE(addr2.valid());
        ASSERT_EQ("127.0.0.1:9000", addr2.toString());
        {
            CUdpSocket s;
            ASSERT_TRUE(s.bindAddr(addr));
            ASSERT_TRUE(s.valid());
            ASSERT_NE(-1, s.fd());

            CSockAddr result;
            ASSERT_FALSE(result.valid());
            ASSERT_EQ("unknown", result.toString());

            result = s.peerAddr();
            ASSERT_FALSE(result.valid());
            ASSERT_EQ("unknown", result.toString());
        }{
            CUdpSocket s;
            ASSERT_TRUE(s.connectAddr(addr2));
            ASSERT_TRUE(s.valid());
            ASSERT_NE(-1, s.fd());

            CSockAddr result;
            ASSERT_FALSE(result.valid());
            ASSERT_EQ("unknown", result.toString());

            result = s.peerAddr();
            ASSERT_TRUE(result.valid());
            ASSERT_EQ(addr2.toString(), result.toString());
        }{
            CUdpSocket s;
            ASSERT_TRUE(s.bindAddr(addr));
            ASSERT_TRUE(s.connectAddr(addr2));
            ASSERT_TRUE(s.valid());
            ASSERT_NE(-1, s.fd());

            CSockAddr result;
            ASSERT_FALSE(result.valid());
            ASSERT_EQ("unknown", result.toString());

            result = s.peerAddr();
            ASSERT_TRUE(result.valid());
            ASSERT_EQ(addr2.toString(), result.toString());
        }
    }
}

TEST(CUdpSocket, linger)
{
    {
        CUdpSocket s;
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
        CUdpSocket s(AF_INET);
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
        CUdpSocket s(AF_INET6);
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

TEST(CUdpSocket, reuseAddr)
{
    {
        CUdpSocket s;
        ASSERT_FALSE(s.valid());
        ASSERT_EQ(-1, s.fd());

        ASSERT_FALSE(s.reuseAddr(true));
        ASSERT_FALSE(s.valid());
        ASSERT_EQ(-1, s.fd());

        ASSERT_FALSE(s.reuseAddr(false));
        ASSERT_FALSE(s.valid());
        ASSERT_EQ(-1, s.fd());
    }{
        CUdpSocket s(AF_INET);
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());

        ASSERT_TRUE(s.reuseAddr(true));
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());

        ASSERT_TRUE(s.reuseAddr(false));
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());
    }{
        CUdpSocket s(AF_INET6);
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

TEST(CUdpSocket, recvTimeout)
{
    {
        CUdpSocket s;
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
        CUdpSocket s(AF_INET);
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
        CUdpSocket s(AF_INET6);
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

TEST(CUdpSocket, sendTimeout)
{
    {
        CUdpSocket s;
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
        CUdpSocket s(AF_INET);
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
        CUdpSocket s(AF_INET6);
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

TEST(CUdpSocket, recvBufSize)
{
    {
        CUdpSocket s;
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
        CUdpSocket s(AF_INET);
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
        CUdpSocket s(AF_INET6);
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

TEST(CUdpSocket, sendBufSize)
{
    {
        CUdpSocket s;
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
        CUdpSocket s(AF_INET);
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
        CUdpSocket s(AF_INET6);
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

TEST(CUdpSocket, toString)
{
    {
        CUdpSocket s;
        ASSERT_FALSE(s.valid());
        ASSERT_EQ(-1, s.fd());
        ASSERT_EQ("{IFileDesc={fd_=-1(CUdpSocket)}, host=unknown, peer=unknown}", s.toString());
    }{
        CUdpSocket s(AF_INET);
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());
        std::ostringstream oss;
        oss<<"{IFileDesc={fd_="<<s.fd()<<"(CUdpSocket)}, host=0.0.0.0:0, peer=unknown}";
        ASSERT_EQ(oss.str(), s.toString());
    }{
        CSockAddr addr("127.0.0.1", 8000);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ("127.0.0.1:8000", addr.toString());

        CSockAddr addr2("127.0.0.1", 9000);
        ASSERT_TRUE(addr2.valid());
        ASSERT_EQ("127.0.0.1:9000", addr2.toString());
        {
            CUdpSocket s;
            ASSERT_TRUE(s.bindAddr(addr));
            ASSERT_TRUE(s.valid());
            ASSERT_NE(-1, s.fd());
            std::ostringstream oss;
            oss<<"{IFileDesc={fd_="<<s.fd()<<"(CUdpSocket)}, host="<<addr.toString()<<", peer=unknown}";
            ASSERT_EQ(oss.str(), s.toString());
        }{
            CUdpSocket s;
            ASSERT_TRUE(s.connectAddr(addr));
            ASSERT_TRUE(s.valid());
            ASSERT_NE(-1, s.fd());
            CSockAddr host = s.hostAddr();
            std::ostringstream oss;
            oss<<"{IFileDesc={fd_="<<s.fd()<<"(CUdpSocket)}, host="<<host.toString()<<", peer="<<addr.toString()<<"}";
            ASSERT_EQ(oss.str(), s.toString());
        }{
            CUdpSocket s;
            ASSERT_TRUE(s.bindAddr(addr));
            ASSERT_TRUE(s.connectAddr(addr2));
            ASSERT_TRUE(s.valid());
            ASSERT_NE(-1, s.fd());
            std::ostringstream oss;
            oss<<"{IFileDesc={fd_="<<s.fd()<<"(CUdpSocket)}, host="<<addr.toString()<<", peer="<<addr2.toString()<<"}";
            ASSERT_EQ(oss.str(), s.toString());
        }
    }
}

TEST(CUdpSocket, bindAddr)
{
    {
        CSockAddr addr("127.0.0.1", 8000);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ("127.0.0.1:8000", addr.toString());
        {
            CUdpSocket s;
            ASSERT_FALSE(s.valid());
            ASSERT_EQ(-1, s.fd());

            ASSERT_TRUE(s.bindAddr(addr))<<CSocket::ErrMsg();
            ASSERT_TRUE(s.valid());
            ASSERT_NE(-1, s.fd());

            CSockAddr result;
            ASSERT_FALSE(result.valid());
            ASSERT_EQ("unknown", result.toString());

            result = s.hostAddr();
            ASSERT_TRUE(result.valid());
            ASSERT_EQ(addr.toString(), result.toString());
        }{
            CUdpSocket s(AF_INET);
            ASSERT_TRUE(s.valid());
            ASSERT_NE(-1, s.fd());

            ASSERT_TRUE(s.bindAddr(addr))<<CSocket::ErrMsg();
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
        CSockAddr addr("::ffff:172.25.44.15", 8000);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ("[::ffff:172.25.44.15]:8000", addr.toString());
        const std::string addrStr = "0.0.0.0:8000"; //should be addr.toString();
        {
            CUdpSocket s;
            ASSERT_FALSE(s.valid());
            ASSERT_EQ(-1, s.fd());

            if(s.bindAddr(addr)){
                ASSERT_TRUE(s.valid());
                ASSERT_NE(-1, s.fd());

                CSockAddr result;
                ASSERT_FALSE(result.valid());
                ASSERT_EQ("unknown", result.toString());

                result = s.hostAddr();
                ASSERT_TRUE(result.valid());
                ASSERT_EQ(addrStr, result.toString());
            }else{
                cerr<<CSocket::ErrMsg()<<endl;

                CSockAddr result;
                ASSERT_FALSE(result.valid());
                ASSERT_EQ("unknown", result.toString());

                result = s.hostAddr();
                ASSERT_TRUE(result.valid());
                ASSERT_EQ("0.0.0.0:0", result.toString());
            }
        }{
            CUdpSocket s(AF_INET);
            ASSERT_TRUE(s.valid());
            ASSERT_NE(-1, s.fd());

            if(s.bindAddr(addr)){
                ASSERT_TRUE(s.valid());
                ASSERT_NE(-1, s.fd());

                CSockAddr result;
                ASSERT_FALSE(result.valid());
                ASSERT_EQ("unknown", result.toString());

                result = s.hostAddr();
                ASSERT_TRUE(result.valid());
                ASSERT_EQ(addrStr, result.toString());
            }else{
                cerr<<CSocket::ErrMsg()<<endl;

                CSockAddr result;
                ASSERT_FALSE(result.valid());
                ASSERT_EQ("unknown", result.toString());

                result = s.hostAddr();
                ASSERT_TRUE(result.valid());
                ASSERT_EQ("0.0.0.0:0", result.toString());
            }
        }
    }{
        CSockAddr addr;
        ASSERT_FALSE(addr.valid());
        ASSERT_EQ("unknown", addr.toString());
        {
            CUdpSocket s;
            ASSERT_FALSE(s.valid());
            ASSERT_EQ(-1, s.fd());

            ASSERT_FALSE(s.bindAddr(addr));
            ASSERT_FALSE(s.valid());
            ASSERT_EQ(-1, s.fd());
        }{
            CUdpSocket s(AF_INET);
            ASSERT_TRUE(s.valid());
            ASSERT_NE(-1, s.fd());

            ASSERT_FALSE(s.bindAddr(addr));
            ASSERT_TRUE(s.valid());
            ASSERT_NE(-1, s.fd());
        }
    }
}

TEST(CUdpSocket, connectAddr)
{
    {
        CSockAddr addr("127.0.0.1", 8000);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ("127.0.0.1:8000", addr.toString());
        {
            CUdpSocket s;
            ASSERT_FALSE(s.valid());
            ASSERT_EQ(-1, s.fd());

            ASSERT_TRUE(s.connectAddr(addr))<<CSocket::ErrMsg();
            ASSERT_TRUE(s.valid());
            ASSERT_NE(-1, s.fd());

            CSockAddr result;
            ASSERT_FALSE(result.valid());
            ASSERT_EQ("unknown", result.toString());

            result = s.peerAddr();
            ASSERT_TRUE(result.valid());
            ASSERT_EQ(addr.toString(), result.toString());
        }{
            CUdpSocket s(AF_INET);
            ASSERT_TRUE(s.valid());
            ASSERT_NE(-1, s.fd());

            ASSERT_TRUE(s.connectAddr(addr))<<CSocket::ErrMsg();
            ASSERT_TRUE(s.valid());
            ASSERT_NE(-1, s.fd());
            {
                CSockAddr result;
                ASSERT_FALSE(result.valid());
                ASSERT_EQ("unknown", result.toString());

                result = s.peerAddr();
                ASSERT_TRUE(result.valid());
                ASSERT_EQ(addr.toString(), result.toString());
            }
            //re-connect
            {
                CSockAddr addr2("127.0.0.1", 9000);
                ASSERT_TRUE(addr2.valid());
                ASSERT_EQ("127.0.0.1:9000", addr2.toString());

                ASSERT_TRUE(s.connectAddr(addr2))<<CSocket::ErrMsg();
                ASSERT_TRUE(s.valid());
                ASSERT_NE(-1, s.fd());

                CSockAddr result;
                ASSERT_FALSE(result.valid());
                ASSERT_EQ("unknown", result.toString());

                result = s.peerAddr();
                ASSERT_TRUE(result.valid());
                ASSERT_EQ(addr2.toString(), result.toString());
            }{
                CSockAddr addr2("::ffff:127.0.0.1", 10000);
                ASSERT_TRUE(addr2.valid());
                ASSERT_EQ("[::ffff:127.0.0.1]:10000", addr2.toString());

                ASSERT_FALSE(s.connectAddr(addr2))<<CSocket::ErrMsg();  //Address family not supported by protocol
                ASSERT_TRUE(s.valid());
                ASSERT_NE(-1, s.fd());
            }
        }
    }{
        CSockAddr addr("::ffff:127.0.0.1", 8000);
        ASSERT_TRUE(addr.valid());
        ASSERT_EQ("[::ffff:127.0.0.1]:8000", addr.toString());
        {
            CUdpSocket s;
            ASSERT_FALSE(s.valid());
            ASSERT_EQ(-1, s.fd());

            ASSERT_FALSE(s.connectAddr(addr))<<CSocket::ErrMsg();   //Address family not supported by protocol
            EXPECT_TRUE(s.valid());
            EXPECT_NE(-1, s.fd());
        }{
            CUdpSocket s(AF_INET);
            ASSERT_TRUE(s.valid());
            ASSERT_NE(-1, s.fd());

            ASSERT_FALSE(s.connectAddr(addr))<<CSocket::ErrMsg();   //Address family not supported by protocol
            EXPECT_TRUE(s.valid());
            EXPECT_NE(-1, s.fd());

            //re-connect
            {
                CSockAddr addr2("::ffff:127.0.0.1", 9000);
                ASSERT_TRUE(addr2.valid());
                ASSERT_EQ("[::ffff:127.0.0.1]:9000", addr2.toString());

                ASSERT_FALSE(s.connectAddr(addr2))<<CSocket::ErrMsg();  //Address family not supported by protocol
                EXPECT_TRUE(s.valid());
                EXPECT_NE(-1, s.fd());
            }{
                CSockAddr addr2("127.0.0.1", 10000);
                ASSERT_TRUE(addr2.valid());
                ASSERT_EQ("127.0.0.1:10000", addr2.toString());

                ASSERT_TRUE(s.connectAddr(addr2))<<CSocket::ErrMsg();
                ASSERT_TRUE(s.valid());
                ASSERT_NE(-1, s.fd());

                CSockAddr result;
                ASSERT_FALSE(result.valid());
                ASSERT_EQ("unknown", result.toString());

                result = s.peerAddr();
                ASSERT_TRUE(result.valid());
                ASSERT_EQ(addr2.toString(), result.toString());
            }
        }
    }{
        CSockAddr addr;
        ASSERT_FALSE(addr.valid());
        ASSERT_EQ("unknown", addr.toString());
        {
            CUdpSocket s;
            ASSERT_FALSE(s.valid());
            ASSERT_EQ(-1, s.fd());

            ASSERT_FALSE(s.connectAddr(addr));
            ASSERT_FALSE(s.valid());
            ASSERT_EQ(-1, s.fd());
        }{
            CUdpSocket s(AF_INET);
            ASSERT_TRUE(s.valid());
            ASSERT_NE(-1, s.fd());

            ASSERT_FALSE(s.connectAddr(addr));
            ASSERT_TRUE(s.valid());
            ASSERT_NE(-1, s.fd());
        }
    }
}

TEST(CUdpSocket, send_recv)
{
    const CSockAddr addr1("127.0.0.1", 8000);
    const CSockAddr addr2("127.0.0.1", 9000);
    const CSockAddr addr3("127.0.0.1", 10000);
    {
        ASSERT_TRUE(addr1.valid());
        ASSERT_EQ("127.0.0.1:8000", addr1.toString());
        ASSERT_TRUE(addr2.valid());
        ASSERT_EQ("127.0.0.1:9000", addr2.toString());
        ASSERT_TRUE(addr3.valid());
        ASSERT_EQ("127.0.0.1:10000", addr3.toString());
    }
    CUdpSocket s1;
    ASSERT_TRUE(s1.bindAddr(addr1));
    ASSERT_TRUE(s1.connectAddr(addr2));
    ASSERT_TRUE(s1.valid());
    ASSERT_NE(-1, s1.fd());

    CUdpSocket s2;
    ASSERT_TRUE(s2.bindAddr(addr2));
    ASSERT_TRUE(s2.connectAddr(addr1));
    ASSERT_TRUE(s2.valid());
    ASSERT_NE(-1, s2.fd());
    {
        std::string buf = "abcdefg2ndagon3:LD#N";
        std::string buf2;

        ASSERT_EQ(buf.size(), (size_t)s1.sendData(&buf[0], buf.size()));
        buf2.resize(buf.size());
        ASSERT_EQ(buf.size(), (size_t)s2.recvData(&buf2[0], buf.size(), true));
        ASSERT_EQ(buf, buf2);

        buf = "2na-n3-qnbafbn;ak:NKEG_E#";
        ASSERT_EQ(buf.size(), (size_t)s1.sendData(&buf[0], buf.size()));
        buf2.resize(buf.size());
        ASSERT_EQ(buf.size(), (size_t)s2.recvData(&buf2[0], buf.size(), false));
        ASSERT_EQ(buf, buf2);

        buf = "34ny;qi-2N_##V (_N_(#";
        ASSERT_EQ(buf.size(), (size_t)s1.sendData(&buf[0], buf.size()));
        buf2.resize(buf.size());
        ASSERT_EQ(buf.size(), (size_t)s2.recvData(&buf2[0], buf.size()));
        ASSERT_EQ(buf, buf2);
    }{
        std::string buf = "abcdefg2ndagon3:LD#N";
        std::string buf2;

        ASSERT_EQ(buf.size(), (size_t)s1.sendData(buf));
        buf2.clear();
        ASSERT_EQ(buf.size(), (size_t)s2.recvData(buf2, buf.size(), true));
        ASSERT_EQ(buf, buf2);

        buf = "2na-n3-qnbafbn;ak:NKEG_E#";
        ASSERT_EQ(buf.size(), (size_t)s1.sendData(buf));
        buf2.clear();
        ASSERT_EQ(buf.size(), (size_t)s2.recvData(buf2, buf.size(), false));
        ASSERT_EQ(buf, buf2);

        buf = "34ny;qi-2N_##V (_N_(#";
        ASSERT_EQ(buf.size(), (size_t)s1.sendData(buf));
        buf2.clear();
        ASSERT_EQ(buf.size(), (size_t)s2.recvData(buf2, buf.size()));
        ASSERT_EQ(buf, buf2);
    }

    CUdpSocket s3;
    ASSERT_TRUE(s3.bindAddr(addr3));
    ASSERT_TRUE(s3.valid());
    ASSERT_NE(-1, s3.fd());
    {
        std::string buf = "abcdefg2ndagon3:LD#N";
        std::string buf2;

        ASSERT_EQ(buf.size(), (size_t)s1.sendData(addr3, &buf[0], buf.size()));
        buf2.resize(buf.size());
        {
            CSockAddr addr;
            ASSERT_FALSE(addr.valid());
            ASSERT_EQ("unknown", addr.toString());

            ASSERT_EQ(buf.size(), (size_t)s3.recvData(addr, &buf2[0], buf.size(), true));
            ASSERT_EQ(buf, buf2);
            ASSERT_TRUE(addr.valid());
            ASSERT_EQ(addr1.toString(), addr.toString());
        }

        buf = "2na-n3-qnbafbn;ak:NKEG_E#";
        ASSERT_EQ(buf.size(), (size_t)s1.sendData(addr3, &buf[0], buf.size()));
        buf2.resize(buf.size());
        {
            CSockAddr addr;
            ASSERT_FALSE(addr.valid());
            ASSERT_EQ("unknown", addr.toString());

            ASSERT_EQ(buf.size(), (size_t)s3.recvData(addr, &buf2[0], buf.size(), false));
            ASSERT_EQ(buf, buf2);
            ASSERT_TRUE(addr.valid());
            ASSERT_EQ(addr1.toString(), addr.toString());
        }

        buf = "34ny;qi-2N_##V (_N_(#";
        ASSERT_EQ(buf.size(), (size_t)s1.sendData(addr3, &buf[0], buf.size()));
        buf2.resize(buf.size());
        {
            CSockAddr addr;
            ASSERT_FALSE(addr.valid());
            ASSERT_EQ("unknown", addr.toString());

            ASSERT_EQ(buf.size(), (size_t)s3.recvData(addr, &buf2[0], buf.size()));
            ASSERT_EQ(buf, buf2);
            ASSERT_TRUE(addr.valid());
            ASSERT_EQ(addr1.toString(), addr.toString());
        }
    }{
        std::string buf = "abcdefg2ndagon3:LD#N";
        std::string buf2;

        ASSERT_EQ(buf.size(), (size_t)s1.sendData(addr3, buf));
        buf2.clear();
        {
            CSockAddr addr;
            ASSERT_FALSE(addr.valid());
            ASSERT_EQ("unknown", addr.toString());

            ASSERT_EQ(buf.size(), (size_t)s3.recvData(addr, buf2, buf.size(), true));
            ASSERT_EQ(buf, buf2);
            ASSERT_TRUE(addr.valid());
            ASSERT_EQ(addr1.toString(), addr.toString());
        }

        buf = "2na-n3-qnbafbn;ak:NKEG_E#";
        ASSERT_EQ(buf.size(), (size_t)s1.sendData(addr3, buf));
        buf2.clear();
        {
            CSockAddr addr;
            ASSERT_FALSE(addr.valid());
            ASSERT_EQ("unknown", addr.toString());

            ASSERT_EQ(buf.size(), (size_t)s3.recvData(addr, buf2, buf.size(), false));
            ASSERT_EQ(buf, buf2);
            ASSERT_TRUE(addr.valid());
            ASSERT_EQ(addr1.toString(), addr.toString());
        }

        buf = "34ny;qi-2N_##V (_N_(#";
        ASSERT_EQ(buf.size(), (size_t)s1.sendData(addr3, buf));
        buf2.clear();
        {
            CSockAddr addr;
            ASSERT_FALSE(addr.valid());
            ASSERT_EQ("unknown", addr.toString());

            ASSERT_EQ(buf.size(), (size_t)s3.recvData(addr, buf2, buf.size()));
            ASSERT_EQ(buf, buf2);
            ASSERT_TRUE(addr.valid());
            ASSERT_EQ(addr1.toString(), addr.toString());
        }
    }
}
