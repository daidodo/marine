#include <marine/scoped_ptr.hh>

#include "../inc.h"

struct A
{
    static int c;
    A(){++c;}
    A(const A &){++c;}
    ~A(){--c;}
};

int A::c = 0;

typedef CScopedPtr<A>    __Ptr;

TEST(CScopedPtr, ctor)
{
    ASSERT_EQ(0, A::c);
    {
        __Ptr p;
        ASSERT_EQ(NULL, p.get());
        ASSERT_EQ(0, A::c);
    }{
        A * t = tools::New<A>();
        ASSERT_EQ(1, A::c);
        __Ptr p(t);
        ASSERT_EQ(t, p.get());
        ASSERT_EQ(1, A::c);
    }
    ASSERT_EQ(0, A::c);
}

TEST(CScopedPtr, dtor)
{
    ASSERT_EQ(0, A::c);
    A * t = tools::New<A>();
    ASSERT_EQ(1, A::c);
    {
        __Ptr p(t);
        ASSERT_EQ(1, A::c);
        ASSERT_EQ(t, p.get());
    }
    ASSERT_EQ(0, A::c);
}

TEST(CScopedPtr, bool_get)
{
    {
        __Ptr p;
        ASSERT_TRUE(!p);
        ASSERT_FALSE(p);
        ASSERT_EQ(NULL, p.get());
        ASSERT_EQ(NULL, &*p);
        ASSERT_EQ(NULL, p.operator ->());
    }{
        A * t = tools::New<A>();
        __Ptr p(t);
        ASSERT_FALSE(!p);
        ASSERT_TRUE(p);
        ASSERT_EQ(t, p.get());
        ASSERT_EQ(t, &*p);
        ASSERT_EQ(t, p.operator ->());
    }
}

TEST(CScopedPtr, reset)
{
    ASSERT_EQ(0, A::c);
    A * t = tools::New<A>();
    ASSERT_EQ(1, A::c);
    {
        __Ptr p;
        ASSERT_EQ(1, A::c);
        ASSERT_EQ(NULL, p.get());

        p.reset(NULL);
        ASSERT_EQ(1, A::c);
        ASSERT_EQ(NULL, p.get());

        p.reset(t);
        ASSERT_EQ(1, A::c);
        ASSERT_EQ(t, p.get());

        p.reset(t);
        ASSERT_EQ(1, A::c);
        ASSERT_EQ(t, p.get());

        p.reset(NULL);
        ASSERT_EQ(0, A::c);
        ASSERT_EQ(NULL, p.get());
    }
    ASSERT_EQ(0, A::c);
}

TEST(CScopedPtr, release)
{
    ASSERT_EQ(0, A::c);
    A * t = tools::New<A>();
    ASSERT_EQ(1, A::c);
    {
        __Ptr p(t);
        ASSERT_EQ(1, A::c);
        ASSERT_EQ(t, p.get());

        p.release();
        ASSERT_EQ(1, A::c);
        ASSERT_EQ(NULL, p.get());
    }
    ASSERT_EQ(1, A::c);
    tools::Delete(t);
    ASSERT_EQ(0, A::c);
}

TEST(CScopedPtr, swap)
{
    ASSERT_EQ(0, A::c);
    A * t1 = tools::New<A>();
    A * t2 = tools::New<A>();
    ASSERT_EQ(2, A::c);
    {
        __Ptr p1(t1);
        __Ptr p2(t2);
        ASSERT_EQ(2, A::c);
        ASSERT_EQ(t1, p1.get());
        ASSERT_EQ(t2, p2.get());

        p1.swap(p2);
        ASSERT_EQ(2, A::c);
        ASSERT_EQ(t2, p1.get());
        ASSERT_EQ(t1, p2.get());

        p1.release();
        p2.release();
    }
    ASSERT_EQ(2, A::c);
    {
        __Ptr p1(t1);
        __Ptr p2;
        ASSERT_EQ(2, A::c);
        ASSERT_EQ(t1, p1.get());
        ASSERT_EQ(NULL, p2.get());

        p1.swap(p2);
        ASSERT_EQ(2, A::c);
        ASSERT_EQ(NULL, p1.get());
        ASSERT_EQ(t1, p2.get());

        p1.release();
        p2.release();
    }
    ASSERT_EQ(2, A::c);
    {
        __Ptr p1;
        __Ptr p2(t2);
        ASSERT_EQ(2, A::c);
        ASSERT_EQ(NULL, p1.get());
        ASSERT_EQ(t2, p2.get());

        p1.swap(p2);
        ASSERT_EQ(2, A::c);
        ASSERT_EQ(t2, p1.get());
        ASSERT_EQ(NULL, p2.get());

        p1.release();
        p2.release();
    }
    ASSERT_EQ(2, A::c);
    {
        __Ptr p1(t1);
        __Ptr p2(t2);
        ASSERT_EQ(2, A::c);
        ASSERT_EQ(t1, p1.get());
        ASSERT_EQ(t2, p2.get());

        swap(p1, p2);
        ASSERT_EQ(2, A::c);
        ASSERT_EQ(t2, p1.get());
        ASSERT_EQ(t1, p2.get());

        p1.release();
        p2.release();
    }
    ASSERT_EQ(2, A::c);
    {
        __Ptr p1(t1);
        __Ptr p2;
        ASSERT_EQ(2, A::c);
        ASSERT_EQ(t1, p1.get());
        ASSERT_EQ(NULL, p2.get());

        swap(p1, p2);
        ASSERT_EQ(2, A::c);
        ASSERT_EQ(NULL, p1.get());
        ASSERT_EQ(t1, p2.get());

        p1.release();
        p2.release();
    }
    ASSERT_EQ(2, A::c);
    {
        __Ptr p1;
        __Ptr p2(t2);
        ASSERT_EQ(2, A::c);
        ASSERT_EQ(NULL, p1.get());
        ASSERT_EQ(t2, p2.get());

        swap(p1, p2);
        ASSERT_EQ(2, A::c);
        ASSERT_EQ(t2, p1.get());
        ASSERT_EQ(NULL, p2.get());

        p1.release();
        p2.release();
    }
    ASSERT_EQ(2, A::c);
    {
        __Ptr p1;
        ASSERT_EQ(2, A::c);
        ASSERT_EQ(NULL, p1.get());

        p1.swap(p1);
        ASSERT_EQ(2, A::c);
        ASSERT_EQ(NULL, p1.get());

        swap(p1, p1);
        ASSERT_EQ(2, A::c);
        ASSERT_EQ(NULL, p1.get());
    }{
        __Ptr p1(t1);
        ASSERT_EQ(2, A::c);
        ASSERT_EQ(t1, p1.get());

        p1.swap(p1);
        ASSERT_EQ(2, A::c);
        ASSERT_EQ(t1, p1.get());

        swap(p1, p1);
        ASSERT_EQ(2, A::c);
        ASSERT_EQ(t1, p1.get());

        p1.release();
    }
    ASSERT_EQ(2, A::c);
    tools::Delete(t1);
    tools::Delete(t2);
    ASSERT_EQ(0, A::c);
}

