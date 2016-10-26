#include <marine/fd_data_map.hh>

#include "../inc.h"

typedef CFdDataMap<std::string> __FdDataMap;
typedef __FdDataMap::pointer __Ptr;
typedef long __Fd;

TEST(CFdDataMap, ctor)
{
    __FdDataMap m1;
    ASSERT_EQ(100, (int)m1.capacity());
    ASSERT_EQ(0, (int)m1.size());

    __FdDataMap m2(200);
    ASSERT_EQ(200, (int)m2.capacity());
    ASSERT_EQ(0, (int)m2.size());

    m1.capacity(300);
    ASSERT_EQ(300, (int)m1.capacity());
    ASSERT_EQ(0, (int)m1.size());

}

TEST(CFdDataMap, setData)
{
    __FdDataMap m;
    size_t sz = 0;
    ASSERT_EQ(sz, m.size());
    {   //000
        const __Fd fd = 0;

        ASSERT_EQ(__Ptr(), m.getData(fd));
        m.setData(fd, __Ptr(NULL));
        ASSERT_EQ(sz, m.size());
        ASSERT_EQ(__Ptr(), m.getData(fd));
    }{  //001
        const __Fd fd = 100;
        __Ptr p;

        ASSERT_EQ(__Ptr(), m.getData(fd));
        m.setData(fd, __Ptr(NULL), &p);
        ASSERT_EQ(sz, m.size());
        ASSERT_EQ(__Ptr(), m.getData(fd));
        ASSERT_TRUE(!p);
    }{  //010
        const __Fd fd = 200;
        const char * const STR = "gaeng";
        __Ptr p(tools::New1<std::string>(STR));

        ASSERT_EQ(__Ptr(), m.getData(fd));
        m.setData(fd, p);
        ++sz;
        ASSERT_EQ(sz, m.size());
        ASSERT_EQ(STR, *m.getData(fd));
    }{  //011
        const __Fd fd = 300;
        const char * const STR = "negna[g";
        __Ptr p(tools::New1<std::string>(STR));

        ASSERT_EQ(__Ptr(), m.getData(fd));
        m.setData(fd, p, &p);
        ++sz;
        ASSERT_EQ(sz, m.size());
        ASSERT_EQ(STR, *m.getData(fd));
        ASSERT_TRUE(!p);
    }{  //100
        const __Fd fd = 400;
        const char * const OLD = "n3g;n";

        m.setData(fd, __Ptr(tools::New1<std::string>(OLD)));
        ++sz;
        ASSERT_EQ(sz, m.size());
        ASSERT_EQ(OLD, *m.getData(fd));

        m.setData(fd, __Ptr(NULL));
        --sz;
        ASSERT_EQ(sz, m.size());
        ASSERT_EQ(__Ptr(), m.getData(fd));
    }{  //101
        const __Fd fd = 500;
        const char * const OLD = "genplp";
        __Ptr p;

        m.setData(fd, __Ptr(tools::New1<std::string>(OLD)));
        ++sz;
        ASSERT_EQ(sz, m.size());
        ASSERT_EQ(OLD, *m.getData(fd));

        m.setData(fd, __Ptr(NULL), &p);
        --sz;
        ASSERT_EQ(sz, m.size());
        ASSERT_EQ(__Ptr(), m.getData(fd));
        ASSERT_EQ(OLD, *p);
    }{  //110
        const __Fd fd = 600;
        const char * const OLD = "genplp";
        const char * const STR = "32nn25";
        __Ptr p(tools::New1<std::string>(STR));

        m.setData(fd, __Ptr(tools::New1<std::string>(OLD)));
        ++sz;
        ASSERT_EQ(sz, m.size());
        ASSERT_EQ(OLD, *m.getData(fd));

        m.setData(fd, p);
        ASSERT_EQ(sz, m.size());
        ASSERT_EQ(STR, *m.getData(fd));
    }{  //111
        const __Fd fd = 700;
        const char * const OLD = "genplp";
        const char * const STR = "32nn25";
        __Ptr p(tools::New1<std::string>(STR));

        m.setData(fd, __Ptr(tools::New1<std::string>(OLD)));
        ++sz;
        ASSERT_EQ(sz, m.size());
        ASSERT_EQ(OLD, *m.getData(fd));

        m.setData(fd, p, &p);
        ASSERT_EQ(sz, m.size());
        ASSERT_EQ(STR, *m.getData(fd));
        ASSERT_EQ(OLD, *p);
    }
}

TEST(CFdDataMap, getData)
{
    {
        __FdDataMap m;
        const __Fd fd = 20;
        const char * const STR = "en;gw";
        __Ptr p(tools::New1<std::string>(STR));

        m.setData(fd, p);
        ASSERT_EQ(1, (int)m.size());

        ASSERT_EQ(STR, *m.getData(fd));

        m.getData(-1, NULL);
        m.getData(-1, &p);
        m.getData(20, NULL);
        ASSERT_EQ(STR, *p);

        p = 0;
        ASSERT_TRUE(!p);
        m.getData(fd, &p);
        ASSERT_EQ(STR, *p);
    }{
        __FdDataMap m;
        ASSERT_EQ(0, (int)m.size());

        std::vector<__Fd> fdvec;
        std::string s;
        for(__Fd i = 0;i < 500;++i){
            s.push_back(i & 0xFF);
            m.setData(i, __Ptr(tools::New1<std::string>(s)));
            fdvec.push_back(i);
        }
        ASSERT_EQ(500, (int)m.size());

        std::vector<__Ptr> pvec(fdvec.size());
        m.getData(fdvec.begin(), fdvec.end(), pvec.begin());
        s.clear();
        for(__Fd i = 0;i < 500;++i){
            s.push_back(i & 0xFF);
            ASSERT_EQ(s, *pvec[i]);
        }
        ASSERT_EQ(500, (int)m.size());
    }
}

TEST(CFdDataMap, clearData)
{
    {
        __FdDataMap m;
        const __Fd fd = 20;
        const char * const STR = "en;gw";
        __Ptr p(tools::New1<std::string>(STR));

        ASSERT_EQ(0, (int)m.size());
        m.setData(fd, p);
        ASSERT_EQ(1, (int)m.size());
        ASSERT_EQ(STR, *m.getData(fd));

        m.clearData(fd);
        ASSERT_EQ(0, (int)m.size());
        ASSERT_EQ(__Ptr(), m.getData(fd));
    }{
        __FdDataMap m;
        const __Fd fd = 20;
        const char * const STR = "en;gw";
        __Ptr p(tools::New1<std::string>(STR));

        ASSERT_EQ(0, (int)m.size());
        m.setData(fd, p);
        p = NULL;
        ASSERT_EQ(1, (int)m.size());
        ASSERT_EQ(STR, *m.getData(fd));
        ASSERT_TRUE(!p);

        m.clearData(fd, &p);
        ASSERT_EQ(0, (int)m.size());
        ASSERT_EQ(__Ptr(), m.getData(fd));
        ASSERT_EQ(STR, *p);
    }{
        __FdDataMap m;
        ASSERT_EQ(0, (int)m.size());

        std::vector<__Fd> fdvec;
        std::string s;
        for(__Fd i = 0;i < 500;++i){
            s.push_back(i & 0xFF);
            m.setData(i, __Ptr(tools::New1<std::string>(s)));
            fdvec.push_back(i);
        }
        ASSERT_EQ(500, (int)m.size());

        m.clearData(fdvec.begin(), fdvec.end());
        for(__Fd i = 0;i < 500;++i){
            ASSERT_EQ(__Ptr(), m.getData(i));
        }
        ASSERT_EQ(0, (int)m.size());
    }{
        __FdDataMap m;
        ASSERT_EQ(0, (int)m.size());

        std::vector<__Fd> fdvec;
        std::string s;
        for(__Fd i = 0;i < 500;++i){
            s.push_back(i & 0xFF);
            m.setData(i, __Ptr(tools::New1<std::string>(s)));
            fdvec.push_back(i);
        }
        ASSERT_EQ(500, (int)m.size());

        std::vector<__Ptr> pvec(fdvec.size());
        m.clearData(fdvec.begin(), fdvec.end(), pvec.begin());
        s.clear();
        for(__Fd i = 0;i < 500;++i){
            s.push_back(i & 0xFF);
            ASSERT_EQ(s, *pvec[i]);
        }
        ASSERT_EQ(0, (int)m.size());
    }
}

TEST(CFdDataMap, clear)
{
    __FdDataMap m;
    ASSERT_EQ(0, (int)m.size());

    std::string s;
    for(__Fd i = 0;i < 500;++i){
        s.push_back(i & 0xFF);
        m.setData(i, __Ptr(tools::New1<std::string>(s)));
    }
    ASSERT_EQ(500, (int)m.size());

    m.clear();
    for(__Fd i = 0;i < 500;++i){
        ASSERT_EQ(__Ptr(), m.getData(i));
    }
    ASSERT_EQ(0, (int)m.size());
}
