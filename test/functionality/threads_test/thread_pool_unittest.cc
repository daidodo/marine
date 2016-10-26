#include <marine/threads.hh>

#include "../inc.h"

typedef CLockInt<int>   __Int;

static int g_sigRecv;

static void sigHandler(int sig)
{
    if(SIGUSR1 == sig)
        g_sigRecv = 1;
}

class CProcInc : public CThreadPool
{
public:
    CProcInc()
        : i_(NULL)
        , before_(0)
        , after_(0)
        , sig_(false)
    {}
    void setInt(__Int & i){i_ = &i;}
    void setBefore(int i){before_ = i;}
    void setAfter(int i){after_ = i;}
    void setSig(bool on){sig_ = on;}
protected:
    virtual void run(){
        assert(i_);
        if(sig_)
            ::signal(SIGUSR1, sigHandler);
        if(before_)
            ::sleep(before_);
        ++ *i_;
        if(after_)
            ::sleep(after_);
    }
private:
    __Int * i_;
    int before_;
    int after_;
    bool sig_;
};

TEST(CThreadPool, joinable)
{
    {
        CProcInc t;
        __Int i;
        t.setInt(i);
        ASSERT_EQ(0, i);
        ASSERT_EQ(1, t.startThreads());
        t.joinThreads();
        ASSERT_EQ(1, i);
    }{
        CProcInc t;
        __Int i;
        t.setInt(i);
        ASSERT_EQ(0, i);
        ASSERT_EQ(10, t.startThreads(10));
        t.joinThreads();
        ASSERT_EQ(10, i);
    }{
        CThreadAttr attr;
        CProcInc t;
        __Int i;
        t.setInt(i);
        t.setBefore(2);
        ASSERT_EQ(0, i);
        ASSERT_EQ(10, t.startThreads(10, attr));
        ::sleep(1);

        ASSERT_EQ(size_t(10), t.runningCount());
        ASSERT_FALSE(t.stopped());

        t.joinThreads();
        ASSERT_EQ(10, i);

        ASSERT_EQ(size_t(0), t.runningCount());
        ASSERT_TRUE(t.stopped());

        ASSERT_NE("", t.toString());
    }
}

TEST(CThreadPool, detached)
{
    CThreadAttr attr;
    attr.detach(true);

    CProcInc t;
    __Int i;
    t.setInt(i);
    ASSERT_EQ(0, i);
    ASSERT_EQ(10, t.startThreads(10, attr));
    ::sleep(1);
    ASSERT_EQ(10, i);
}

TEST(CThreadPool, detach)
{
    CProcInc t;
    __Int i;
    t.setInt(i);
    t.setBefore(1);
    ASSERT_EQ(0, i);
    ASSERT_EQ(10, t.startThreads(10));
    t.detachThreads();
    ::sleep(2);
    ASSERT_EQ(10, i);
}

TEST(CThreadPool, stop)
{
    CProcInc t;
    __Int i;
    t.setInt(i);
    t.setAfter(-1);
    ASSERT_EQ(0, i);
    ASSERT_EQ(10, t.startThreads(10));
    ::sleep(1);

    ASSERT_EQ(size_t(10), t.runningCount());
    ASSERT_FALSE(t.stopped());

    t.stopThreads();
    t.joinThreads();

    ASSERT_EQ(size_t(0), t.runningCount());
    ASSERT_TRUE(t.stopped());

    ASSERT_EQ(10, i);
}

TEST(CThreadPool, signal)
{
    CProcInc t;
    __Int i;
    t.setInt(i);
    t.setBefore(2);
    t.setSig(true);
    ASSERT_EQ(0, i);
    g_sigRecv = 0;
    ASSERT_EQ(1, t.startThreads());
    ::sleep(1);
    t.signalThreads(SIGUSR1);
    t.joinThreads();
    ASSERT_EQ(1, i);
    ASSERT_EQ(1, g_sigRecv);
}
