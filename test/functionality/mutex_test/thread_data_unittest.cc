#include <marine/mutex.hh>

#include <marine/atomic_sync.hh>
#include <marine/threads.hh>
#include "../inc.h"

struct A
{
    static CAtomicSync<int> cnt_;
    A():v_(0){++cnt_;}
    A(const A & a):v_(a.v_){++cnt_;}
    ~A(){--cnt_;}
    //fields:
    long v_;
};

CAtomicSync<int> A::cnt_;

typedef CThreadData<A> __A;

TEST(CThreadData, ST)
{
    {
        __A a;
        ASSERT_TRUE(a.valid());
        ASSERT_EQ(0, A::cnt_);

        A * p = a.ptr();
        ASSERT_TRUE(a.valid())<<a.toString();
        ASSERT_EQ(1, A::cnt_);
        ASSERT_TRUE(NULL != p);
        ASSERT_EQ(0, p->v_);

        p->v_ = 100;

        A * p2 = a.ptr();
        ASSERT_TRUE(a.valid())<<a.toString();
        ASSERT_EQ(1, A::cnt_);
        ASSERT_TRUE(NULL != p2);
        ASSERT_EQ(100, p2->v_);
        ASSERT_EQ(p, p2);
    }{
        __A a;
        ASSERT_TRUE(a.valid());
        ASSERT_EQ(1, A::cnt_);

        const __A & ca = a;

        const A * p = ca.ptr();
        ASSERT_TRUE(ca.valid())<<ca.toString();
        ASSERT_EQ(2, A::cnt_);
        ASSERT_TRUE(NULL != p);
        ASSERT_EQ(0, p->v_);

        const A * p2 = ca.ptr();
        ASSERT_TRUE(ca.valid())<<ca.toString();
        ASSERT_EQ(2, A::cnt_);
        ASSERT_TRUE(NULL != p2);
        ASSERT_EQ(0, p2->v_);
        ASSERT_EQ(p, p2);
    }{
        __A a;
        ASSERT_TRUE(a.valid());
        ASSERT_EQ(2, A::cnt_);

        A & p = a.ref();
        ASSERT_TRUE(a.valid())<<a.toString();
        ASSERT_EQ(3, A::cnt_);
        ASSERT_EQ(0, p.v_);

        p.v_ = 100;

        A & p2 = a.ref();
        ASSERT_TRUE(a.valid())<<a.toString();
        ASSERT_EQ(3, A::cnt_);
        ASSERT_EQ(100, p2.v_);
        ASSERT_EQ(&p, &p2);
    }{
        __A a;
        ASSERT_TRUE(a.valid());
        ASSERT_EQ(3, A::cnt_);

        const __A & ca = a;

        const A & p = ca.ref();
        ASSERT_TRUE(ca.valid())<<ca.toString();
        ASSERT_EQ(4, A::cnt_);
        ASSERT_EQ(0, p.v_);

        const A & p2 = ca.ref();
        ASSERT_TRUE(ca.valid())<<ca.toString();
        ASSERT_EQ(4, A::cnt_);
        ASSERT_EQ(0, p2.v_);
        ASSERT_EQ(&p, &p2);
    }
}

static __A mt;

static void * Proc(void *)
{
    long r = 0;
    const __A & cmt = mt;
    for(int i = 0;i < 50;++i){
        EXPECT_EQ(r, cmt.ptr()->v_);
        int a = 2 * i + 1;
        r += a;
        mt.ptr()->v_ += a;

        EXPECT_EQ(r, cmt.ref().v_);
        ++a;
        r += a;
        mt.ref().v_ += a;
    }
    sleep(1);
    EXPECT_EQ(14, A::cnt_);
    sleep(1);
    EXPECT_EQ(5050, cmt.ptr()->v_)<<"r="<<r;
    EXPECT_EQ(5050, cmt.ref().v_)<<"r="<<r;
    return NULL;
}


TEST(CThreadData, MT)
{
    ASSERT_EQ(4, A::cnt_);
    std::vector<CThread> th(10);
    for(size_t i = 0;i < th.size();++i)
        th[i].start(Proc);
    for(size_t i = 0;i < th.size();++i)
        th[i].join();
    ASSERT_EQ(4, A::cnt_);
}
