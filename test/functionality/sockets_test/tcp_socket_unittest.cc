#include <marine/sockets.hh>

#include "../inc.h"

TEST(TcpSocket, all)
{
    CSockAddr addr("127.0.0.1",8000);
    ASSERT_TRUE(addr.valid());
    ASSERT_EQ("127.0.0.1:8000", addr.toString());

    CSockAddr addr2("127.0.0.1",9000);
    ASSERT_TRUE(addr2.valid());
    ASSERT_EQ("127.0.0.1:9000", addr2.toString());

    const std::string buf1 = "pan3na;dspa3 a)N# [";

    int n = fork();
    ASSERT_TRUE(n >= 0);
    if(n > 0){ //parent
        //1. listen
        CListenSocket v;
        ASSERT_TRUE(v.listenAddr(addr))<<CSocket::ErrMsg();
        ASSERT_TRUE(v.valid());
        ASSERT_NE(-1, v.fd());

        //3. accept
        CTcpConnSocket s;
        ASSERT_FALSE(s.valid());
        ASSERT_EQ(-1, s.fd());

        ASSERT_TRUE(v.acceptSocket(s));
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());

        //4. send 1
        ASSERT_EQ(buf1.size(), (size_t)s.sendData(&buf1[0], buf1.size()));

        //7. recv 2
        std::string tmp;
        tmp.clear();
        tmp.resize(buf1.size());
        ASSERT_EQ(buf1.size(), (size_t)s.recvData(&tmp[0], buf1.size(), false));
        ASSERT_EQ(buf1, tmp);

        //8. send 3
        ASSERT_EQ(buf1.size(), (size_t)s.sendData(&buf1[0], buf1.size()));

        //11. recv 4
        tmp.clear();
        ASSERT_EQ(buf1.size(), (size_t)s.recvData(tmp, buf1.size(), true));
        ASSERT_EQ(buf1, tmp);

        //12. send 5
        ASSERT_EQ(buf1.size(), (size_t)s.sendData(buf1));

        //15. recv 6
        tmp.clear();
        ASSERT_EQ(buf1.size(), (size_t)s.recvData(tmp, buf1.size()));
        ASSERT_EQ(buf1, tmp);
    }else{     //child
        //2. connect
        CTcpConnSocket s;
        sleep(1);
        ASSERT_TRUE(s.bindAddr(addr2))<<CSocket::ErrMsg();
        ASSERT_TRUE(s.connectAddr(addr));
        ASSERT_TRUE(s.valid());
        ASSERT_NE(-1, s.fd());

        //5. recv 1
        std::string tmp;
        tmp.clear();
        tmp.resize(buf1.size());
        ASSERT_EQ(buf1.size(), (size_t)s.recvData(&tmp[0], buf1.size(), true));
        ASSERT_EQ(buf1, tmp);

        //6. send 2
        ASSERT_EQ(buf1.size(), (size_t)s.sendData(&buf1[0], buf1.size()));

        //9. recv 3
        tmp.clear();
        tmp.resize(buf1.size());
        ASSERT_EQ(buf1.size(), (size_t)s.recvData(&tmp[0], buf1.size()));
        ASSERT_EQ(buf1, tmp);

        //10. send 4
        ASSERT_EQ(buf1.size(), (size_t)s.sendData(buf1));

        //13. recv 5
        tmp.clear();
        ASSERT_EQ(buf1.size(), (size_t)s.recvData(tmp, buf1.size(), false));
        ASSERT_EQ(buf1, tmp);

        //14. send 6
        ASSERT_EQ(buf1.size(), (size_t)s.sendData(buf1));
    }
}

