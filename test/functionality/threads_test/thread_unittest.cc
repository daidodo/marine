#include <marine/threads.hh>

#include "../inc.h"

TEST(CThreadAttr, all)
{
    {
        CThreadAttr attr;
        //stack size
        ASSERT_EQ(size_t(8388608), attr.stackSize());
        attr.stackSize(16 << 10);
        ASSERT_EQ(size_t(16 << 10), attr.stackSize());
        //guard size
        ASSERT_EQ(size_t(4096), attr.guardSize());
        attr.guardSize(8 << 10);
        ASSERT_EQ(size_t(8 << 10), attr.guardSize());
        attr.guardSize(0);
        ASSERT_EQ(size_t(0), attr.guardSize());
        attr.guardSize(4096);
        ASSERT_EQ(size_t(4096), attr.guardSize());
        //detach
        ASSERT_FALSE(attr.detach());
        attr.detach(true);
        ASSERT_TRUE(attr.detach());
        attr.detach(false);
        ASSERT_FALSE(attr.detach());
    }{
        CThreadAttr attr;
        //stack
        char * addr = NULL;
        size_t sz = 0;
        ASSERT_EQ(addr, attr.stack(&sz));
        ASSERT_EQ(size_t(0), sz);

        sz = 16 << 10;
        addr = new char[sz];
        attr.stack(addr, sz);
        size_t sz2 = 0;
        ASSERT_EQ(addr, attr.stack(&sz2));
        ASSERT_EQ(sz, sz2);
        delete [] addr;
    }
}

typedef CLockInt<int>   __Int;

static void * procInc(void * arg)
{
    __Int * v = static_cast<__Int *>(arg);
    ++ *v;
    return reinterpret_cast<void *>(pthread_self());
}

TEST(CThread, joinable)
{
    CThread t;
    ASSERT_EQ("{id_=0, ret_=0}", t.toString());

    __Int i;
    ASSERT_EQ(0, i);
    ASSERT_TRUE(t.start(procInc, &i));
    ASSERT_TRUE(t.join());
    ASSERT_EQ(1, i);

    CThreadAttr attr;
    ASSERT_TRUE(t.start(procInc, &i, attr));
    ASSERT_TRUE(t.join());
    ASSERT_EQ(2, i);

    CThread t2;
    ASSERT_TRUE(t != t2);
    t2 = t;
    ASSERT_TRUE(t == t2);

    ASSERT_TRUE(t != pthread_self());
    ASSERT_TRUE(pthread_self() != t);
    pthread_t id = reinterpret_cast<pthread_t>(t.retval());
    ASSERT_TRUE(t == id);
    ASSERT_TRUE(id == t);

    std::ostringstream oss;
    oss<<"{id_="<<id<<", ret_=0x"<<std::hex<<id<<"}";
    ASSERT_EQ(oss.str(), t.toString());
}

TEST(CThread, detached)
{
    CThreadAttr attr;
    attr.detach(true);

    CThread t[10];
    __Int i;
    ASSERT_EQ(0, i);
    for(int j = 0;j < 10;++j)
        ASSERT_TRUE(t[i].start(procInc, &i, attr));
    ::sleep(1);
    ASSERT_EQ(10, i);
}

static void * procSleepInc(void * arg)
{
    ::sleep(1);
    __Int * v = static_cast<__Int *>(arg);
    ++ *v;
    return reinterpret_cast<void *>(pthread_self());
}

TEST(CThread, detach)
{
    CThread t[10];
    __Int i;
    ASSERT_EQ(0, i);
    for(int j = 0;j < 10;++j){
        ASSERT_TRUE(t[i].start(procSleepInc, &i));
        ASSERT_TRUE(t[i].detach());
    }
    ::sleep(2);
    ASSERT_EQ(10, i);
}

static int g_sigRecv;

static void sigHandler(int sig)
{
    if(SIGUSR1 == sig)
        g_sigRecv = 1;
}

static void * procIncSleep(void * arg)
{
    ::signal(SIGUSR1, sigHandler);
    __Int * v = static_cast<__Int *>(arg);
    ++ *v;
    ::sleep(-1);
    return reinterpret_cast<void *>(pthread_self());
}

TEST(CThread, stop)
{
    CThread t;
    __Int i;
    ASSERT_EQ(0, i);
    ASSERT_TRUE(t.start(procIncSleep, &i));
    ASSERT_TRUE(t.stop());
    ASSERT_TRUE(t.join());
    ASSERT_EQ(1, i);
}

TEST(CThread, signal)
{
    CThread t;
    __Int i;
    ASSERT_EQ(0, i);
    g_sigRecv = 0;
    ASSERT_TRUE(t.start(procSleepInc, &i));
    ASSERT_TRUE(t.signal(SIGUSR1));
    ASSERT_TRUE(t.join());
    ASSERT_EQ(1, i);
    ASSERT_EQ(1, g_sigRecv);
}
