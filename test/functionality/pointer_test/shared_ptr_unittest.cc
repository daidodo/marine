#include <marine/shared_ptr.hh>

#include "../inc.h"

struct A
{
    static int c;
    A(){++c;}
    A(const A &){++c;}
    ~A(){--c;}
};

int A::c = 0;

typedef CSharedPtr<A>    __Ptr;

TEST(CSharedPtr, ctor_dtor)
{
    ASSERT_EQ(0, A::c);
    {
        __Ptr p;
        ASSERT_EQ(NULL, p.get());
        ASSERT_EQ(0, A::c);
        {
            __Ptr p2(p);
            ASSERT_EQ(NULL, p2.get());
        }
        ASSERT_EQ(0, A::c);
    }{
        A * t = tools::New<A>();
        ASSERT_EQ(1, A::c);

        __Ptr p(t);
        ASSERT_EQ(t, p.get());
        ASSERT_EQ(1, A::c);
        {
            __Ptr p2(p);
            ASSERT_EQ(t, p2.get());
        }
        ASSERT_EQ(1, A::c);
    }
    ASSERT_EQ(0, A::c);
}

TEST(CSharedPtr, assignment)
{
    {
        ASSERT_EQ(0, A::c);
        __Ptr p;
        __Ptr p2;
        ASSERT_EQ(NULL, p.get());
        ASSERT_EQ(NULL, p2.get());
        ASSERT_EQ(0, A::c);

        p = p2;
        ASSERT_EQ(NULL, p.get());
        ASSERT_EQ(NULL, p2.get());
        ASSERT_EQ(0, A::c);
    }{
        A * t = tools::New<A>();
        ASSERT_EQ(1, A::c);
        __Ptr p(t);
        __Ptr p2;
        ASSERT_EQ(t, p.get());
        ASSERT_EQ(NULL, p2.get());
        ASSERT_EQ(1, A::c);

        p = p2;
        ASSERT_EQ(NULL, p.get());
        ASSERT_EQ(NULL, p2.get());
        ASSERT_EQ(0, A::c);
    }{
        A * t = tools::New<A>();
        ASSERT_EQ(1, A::c);
        __Ptr p;
        __Ptr p2(t);
        ASSERT_EQ(NULL, p.get());
        ASSERT_EQ(t, p2.get());
        ASSERT_EQ(1, A::c);

        p = p2;
        ASSERT_EQ(t, p.get());
        ASSERT_EQ(t, p2.get());
        ASSERT_EQ(1, A::c);
    }{
        A * t = tools::New<A>();
        A * t2 = tools::New<A>();
        ASSERT_EQ(2, A::c);
        __Ptr p(t);
        __Ptr p2(t2);
        ASSERT_EQ(t, p.get());
        ASSERT_EQ(t2, p2.get());
        ASSERT_EQ(2, A::c);

        p = p2;
        ASSERT_EQ(t2, p.get());
        ASSERT_EQ(t2, p2.get());
        ASSERT_EQ(1, A::c);
    }{
        ASSERT_EQ(0, A::c);
        __Ptr p;
        ASSERT_EQ(NULL, p.get());
        ASSERT_EQ(0, A::c);

        p = NULL;
        ASSERT_EQ(NULL, p.get());
        ASSERT_EQ(0, A::c);
    }{
        A * t = tools::New<A>();
        ASSERT_EQ(1, A::c);
        __Ptr p(t);
        ASSERT_EQ(t, p.get());
        ASSERT_EQ(1, A::c);

        p = NULL;
        ASSERT_EQ(NULL, p.get());
        ASSERT_EQ(0, A::c);
    }{
        A * t = tools::New<A>();
        ASSERT_EQ(1, A::c);
        __Ptr p;
        ASSERT_EQ(NULL, p.get());
        ASSERT_EQ(1, A::c);

        p = t;
        ASSERT_EQ(t, p.get());
        ASSERT_EQ(1, A::c);
    }{
        A * t = tools::New<A>();
        A * t2 = tools::New<A>();
        ASSERT_EQ(2, A::c);
        __Ptr p(t);
        ASSERT_EQ(t, p.get());
        ASSERT_EQ(2, A::c);

        p = t2;
        ASSERT_EQ(t2, p.get());
        ASSERT_EQ(1, A::c);
    }
    ASSERT_EQ(0, A::c);
}

TEST(CSharedPtr, bool_get)
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

TEST(CSharedPtr, compare)
{
    {
        A * t1 = tools::New<A>();
        const A * ct1 = t1;

        __Ptr p1;
        __Ptr p2;
        ASSERT_TRUE(p1 == p2);
        ASSERT_FALSE(p1 != p2);
        ASSERT_TRUE(p1 == NULL);
        ASSERT_TRUE(NULL == p1);
        ASSERT_FALSE(p1 != NULL);
        ASSERT_FALSE(NULL != p1);
        ASSERT_FALSE(p1 == ct1);
        ASSERT_FALSE(ct1 == p1);
        ASSERT_TRUE(p1 != ct1);
        ASSERT_TRUE(ct1 != p1);

        tools::Delete(t1);
    }{
        A * t1 = tools::New<A>();
        const A * ct1 = t1;

        __Ptr p1(t1);
        __Ptr p2;
        ASSERT_FALSE(p1 == p2);
        ASSERT_TRUE(p1 != p2);
        ASSERT_FALSE(p1 == NULL);
        ASSERT_FALSE(NULL == p1);
        ASSERT_TRUE(p1 != NULL);
        ASSERT_TRUE(NULL != p1);
        ASSERT_TRUE(p1 == ct1);
        ASSERT_TRUE(ct1 == p1);
        ASSERT_FALSE(p1 != ct1);
        ASSERT_FALSE(ct1 != p1);
    }{
        A * t1 = tools::New<A>();
        const A * ct1 = t1;

        __Ptr p2(t1);
        __Ptr p1(p2);
        ASSERT_TRUE(p1 == p2);
        ASSERT_FALSE(p1 != p2);
        ASSERT_FALSE(p1 == NULL);
        ASSERT_FALSE(NULL == p1);
        ASSERT_TRUE(p1 != NULL);
        ASSERT_TRUE(NULL != p1);
        ASSERT_TRUE(p1 == ct1);
        ASSERT_TRUE(ct1 == p1);
        ASSERT_FALSE(p1 != ct1);
        ASSERT_FALSE(ct1 != p1);
    }{
        A * t1 = tools::New<A>();
        A * t2 = tools::New<A>();
        const A * ct1 = t1;

        __Ptr p1(t1);
        __Ptr p2(t2);
        ASSERT_FALSE(p1 == p2);
        ASSERT_TRUE(p1 != p2);
        ASSERT_FALSE(p1 == NULL);
        ASSERT_FALSE(NULL == p1);
        ASSERT_TRUE(p1 != NULL);
        ASSERT_TRUE(NULL != p1);
        ASSERT_TRUE(p1 == ct1);
        ASSERT_TRUE(ct1 == p1);
        ASSERT_FALSE(p1 != ct1);
        ASSERT_FALSE(ct1 != p1);
    }
    ASSERT_EQ(0, A::c);
}

TEST(CSharedPtr, reset)
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

TEST(CSharedPtr, swap)
{
    {
        A * t1 = tools::New<A>();
        A * t2 = tools::New<A>();
        ASSERT_EQ(2, A::c);

        __Ptr p1(t1);
        {
            __Ptr p2(t2);
            ASSERT_EQ(2, A::c);
            ASSERT_EQ(t1, p1.get());
            ASSERT_EQ(t2, p2.get());

            p1.swap(p2);
            ASSERT_EQ(2, A::c);
            ASSERT_EQ(t2, p1.get());
            ASSERT_EQ(t1, p2.get());
        }
        ASSERT_EQ(1, A::c);
    }{
        A * t1 = tools::New<A>();
        ASSERT_EQ(1, A::c);

        __Ptr p1(t1);
        {
            __Ptr p2;
            ASSERT_EQ(1, A::c);
            ASSERT_EQ(t1, p1.get());
            ASSERT_EQ(NULL, p2.get());

            p1.swap(p2);
            ASSERT_EQ(1, A::c);
            ASSERT_EQ(NULL, p1.get());
            ASSERT_EQ(t1, p2.get());
        }
        ASSERT_EQ(0, A::c);
    }{
        A * t2 = tools::New<A>();
        ASSERT_EQ(1, A::c);

        __Ptr p1;
        {
            __Ptr p2(t2);
            ASSERT_EQ(1, A::c);
            ASSERT_EQ(NULL, p1.get());
            ASSERT_EQ(t2, p2.get());

            p1.swap(p2);
            ASSERT_EQ(1, A::c);
            ASSERT_EQ(t2, p1.get());
            ASSERT_EQ(NULL, p2.get());
        }
        ASSERT_EQ(1, A::c);
    }{
        A * t1 = tools::New<A>();
        A * t2 = tools::New<A>();
        ASSERT_EQ(2, A::c);

        __Ptr p1(t1);
        {
            __Ptr p2(t2);
            ASSERT_EQ(2, A::c);
            ASSERT_EQ(t1, p1.get());
            ASSERT_EQ(t2, p2.get());

            swap(p1, p2);
            ASSERT_EQ(2, A::c);
            ASSERT_EQ(t2, p1.get());
            ASSERT_EQ(t1, p2.get());
        }
        ASSERT_EQ(1, A::c);
    }{
        A * t1 = tools::New<A>();
        ASSERT_EQ(1, A::c);

        __Ptr p1(t1);
        {
            __Ptr p2;
            ASSERT_EQ(1, A::c);
            ASSERT_EQ(t1, p1.get());
            ASSERT_EQ(NULL, p2.get());

            swap(p1, p2);
            ASSERT_EQ(1, A::c);
            ASSERT_EQ(NULL, p1.get());
            ASSERT_EQ(t1, p2.get());
        }
        ASSERT_EQ(0, A::c);
    }{
        A * t2 = tools::New<A>();
        ASSERT_EQ(1, A::c);

        __Ptr p1;
        {
            __Ptr p2(t2);
            ASSERT_EQ(1, A::c);
            ASSERT_EQ(NULL, p1.get());
            ASSERT_EQ(t2, p2.get());

            swap(p1, p2);
            ASSERT_EQ(1, A::c);
            ASSERT_EQ(t2, p1.get());
            ASSERT_EQ(NULL, p2.get());
        }
        ASSERT_EQ(1, A::c);
    }{
        __Ptr p1;
        ASSERT_EQ(0, A::c);
        ASSERT_EQ(NULL, p1.get());

        p1.swap(p1);
        ASSERT_EQ(0, A::c);
        ASSERT_EQ(NULL, p1.get());

        swap(p1, p1);
        ASSERT_EQ(0, A::c);
        ASSERT_EQ(NULL, p1.get());
    }{
        A * t1 = tools::New<A>();
        ASSERT_EQ(1, A::c);

        __Ptr p1(t1);
        ASSERT_EQ(1, A::c);
        ASSERT_EQ(t1, p1.get());

        p1.swap(p1);
        ASSERT_EQ(1, A::c);
        ASSERT_EQ(t1, p1.get());

        swap(p1, p1);
        ASSERT_EQ(1, A::c);
        ASSERT_EQ(t1, p1.get());
    }

    ASSERT_EQ(0, A::c);
}
