#include <marine/tools/memory.hh>

#include "../inc.h"

struct Base
{
    static int count;
    Base(){++count;}
    virtual ~Base(){--count;}
    Base(const Base &){++count;}
};

int Base::count = 0;

struct Test : public Base
{
    static int count;
    Test(){++count;}
    explicit Test(const std::string & s):s_(s){++count;}
    Test(const std::string & s, double p):s_(s){++count;}
    virtual ~Test(){--count;}
    Test(const Test &){++count;}
    std::string s_;
    bool operator ==(const std::string & s) const{
        return s_ == s;
    }
};

int Test::count = 0;

bool operator ==(const std::string & s, const Test & t)
{
    return t == s;
}

template<class T>
struct CAlloc : public std::allocator<T>
{
    typedef std::allocator<T> __MyBase;
    static int count;
    T * allocate(size_t sz){
        count += sz;
        return __MyBase::allocate(sz);
    }
    void deallocate(T * p, size_t sz) throw(){
        count -= sz;
        __MyBase::deallocate(p, sz);
    }
};

template<class T>
int CAlloc<T>::count = 0;

typedef Base        B;
typedef Test        T;
typedef CAlloc<T>   A;

TEST(Allocate, all)
{
    A a;
    T * p = NULL;
    ASSERT_TRUE(NULL == p);
    ASSERT_EQ(0, A::count);

    p = tools::Allocate<T>(a, 10);
    ASSERT_TRUE(NULL != p);
    ASSERT_EQ(0, T::count);
    ASSERT_EQ(10, A::count);
    tools::Deallocate(p, 10, a);
    ASSERT_TRUE(NULL == p);
    ASSERT_EQ(0, A::count);

    p = tools::Allocate<T>(a);
    ASSERT_TRUE(NULL != p);
    ASSERT_EQ(0, T::count);
    ASSERT_EQ(1, A::count);
    tools::Deallocate(p, 1, a);
    ASSERT_TRUE(NULL == p);
    ASSERT_EQ(0, A::count);

    p = tools::Allocate<T>(20);
    ASSERT_TRUE(NULL != p);
    ASSERT_EQ(0, T::count);
    tools::Deallocate(p, 20, std::allocator<T>());
    ASSERT_TRUE(NULL == p);
    ASSERT_EQ(0, A::count);

    p = tools::Allocate<T>();
    ASSERT_TRUE(NULL != p);
    ASSERT_EQ(0, T::count);
    tools::Deallocate(p, 1, std::allocator<T>());
    ASSERT_TRUE(NULL == p);
    ASSERT_EQ(0, A::count);
}

TEST(Deallocate, all)
{
    T * p = NULL;
    A a;
    ASSERT_TRUE(NULL == p);
    ASSERT_EQ(0, A::count);
    ASSERT_EQ(0, T::count);

    p = tools::Allocate<T>(a, 10);
    ASSERT_TRUE(NULL != p);
    ASSERT_EQ(0, T::count);
    ASSERT_EQ(10, A::count);
    tools::Deallocate(p, 10, a);
    ASSERT_TRUE(NULL == p);
    ASSERT_EQ(0, A::count);
    ASSERT_EQ(0, T::count);

    p = tools::Allocate<T>(a, 1);
    ASSERT_TRUE(NULL != p);
    ASSERT_EQ(0, T::count);
    ASSERT_EQ(1, A::count);
    tools::Deallocate(p, a);
    ASSERT_TRUE(NULL == p);
    ASSERT_EQ(0, A::count);
    ASSERT_EQ(0, T::count);

    p = tools::Allocate<T>(10);
    ASSERT_TRUE(NULL != p);
    ASSERT_EQ(0, T::count);
    ASSERT_EQ(0, A::count);
    tools::Deallocate(p, 10);
    ASSERT_TRUE(NULL == p);
    ASSERT_EQ(0, A::count);
    ASSERT_EQ(0, T::count);

    p = tools::Allocate<T>(1);
    ASSERT_TRUE(NULL != p);
    ASSERT_EQ(0, T::count);
    ASSERT_EQ(0, A::count);
    tools::Deallocate(p);
    ASSERT_TRUE(NULL == p);
    ASSERT_EQ(0, A::count);
    ASSERT_EQ(0, T::count);
}

TEST(Construct, all)
{
    T * p = NULL;
    A a;
    ASSERT_TRUE(NULL == p);
    ASSERT_EQ(0, A::count);
    ASSERT_EQ(0, T::count);

    p = tools::Allocate<T>(a, 30);
    ASSERT_EQ(30, A::count);

    for(int i = 0;i < 10;++i){
        ASSERT_EQ(i, T::count);
        tools::Construct(p + i);
    }

    for(int i = 10;i < 20;++i){
        ASSERT_EQ(i, T::count);
        tools::Construct(p + i, "abc");
        ASSERT_EQ("abc", p[i]);
    }
    ASSERT_EQ(20, T::count);

    for(int i = 20;i < 30;++i){
        ASSERT_EQ(i, T::count);
        tools::Construct(p + i, "abc", 3.0);
        ASSERT_EQ("abc", p[i]);
    }
    ASSERT_EQ(30, T::count);

    for(int i = 0;i < 30;++i)
        tools::Destroy(p + i);
    ASSERT_EQ(0, T::count);
    tools::Deallocate(p, 30, a);
    ASSERT_TRUE(NULL == p);
    ASSERT_EQ(0, A::count);
}

TEST(Destroy, all)
{
    T * p = NULL;
    ASSERT_TRUE(NULL == p);
    ASSERT_EQ(0, A::count);
    ASSERT_EQ(0, B::count);
    ASSERT_EQ(0, T::count);

    p = new T[20];
    ASSERT_TRUE(NULL != p);

    for(int i = 0;i < 10;++i){
        ASSERT_EQ(20 - i, B::count);
        ASSERT_EQ(20 - i, T::count);
        tools::Destroy(p + i);
    }
    for(int i = 10;i < 20;++i){
        ASSERT_EQ(20 - i, B::count);
        ASSERT_EQ(20 - i, T::count)<<"i="<<i;
        B * b = p + i;   //support virtual dtor
        tools::Destroy(b);
    }
    ASSERT_EQ(0, B::count);
    ASSERT_EQ(0, T::count);
}

TEST(New, all)
{
    T * p = NULL;
    A a;
    ASSERT_TRUE(NULL == p);
    ASSERT_EQ(0, A::count);
    ASSERT_EQ(0, T::count);

    p = tools::New<T>(a, 10);
    ASSERT_TRUE(NULL != p);
    ASSERT_EQ(10, A::count);
    ASSERT_EQ(10, T::count);
    tools::Delete(p, 10, a);
    ASSERT_TRUE(NULL == p);
    ASSERT_EQ(0, A::count);
    ASSERT_EQ(0, T::count);

    p = tools::New<T>(a);
    ASSERT_TRUE(NULL != p);
    ASSERT_EQ(1, A::count);
    ASSERT_EQ(1, T::count);
    tools::Delete(p, 1, a);
    ASSERT_TRUE(NULL == p);
    ASSERT_EQ(0, A::count);
    ASSERT_EQ(0, T::count);

    p = tools::New<T>(10);
    ASSERT_TRUE(NULL != p);
    ASSERT_EQ(0, A::count);
    ASSERT_EQ(10, T::count);
    tools::Delete(p, 10);
    ASSERT_TRUE(NULL == p);
    ASSERT_EQ(0, A::count);
    ASSERT_EQ(0, T::count);

    p = tools::New<T>();
    ASSERT_TRUE(NULL != p);
    ASSERT_EQ(0, A::count);
    ASSERT_EQ(1, T::count);
    tools::Delete(p, 1);
    ASSERT_TRUE(NULL == p);
    ASSERT_EQ(0, A::count);
    ASSERT_EQ(0, T::count);
}

TEST(New1, all)
{
    T * p = NULL;
    A a;
    ASSERT_TRUE(NULL == p);
    ASSERT_EQ(0, A::count);
    ASSERT_EQ(0, T::count);

    p = tools::New1<T>("abc", a, 10);
    ASSERT_TRUE(NULL != p);
    ASSERT_EQ(10, A::count);
    ASSERT_EQ(10, T::count);
    for(int i = 0;i < 10;++i){
        ASSERT_EQ("abc", p[i]);
    }
    tools::Delete(p, 10, a);
    ASSERT_TRUE(NULL == p);
    ASSERT_EQ(0, A::count);
    ASSERT_EQ(0, T::count);

    p = tools::New1<T>("abcd", a);
    ASSERT_TRUE(NULL != p);
    ASSERT_EQ(1, A::count);
    ASSERT_EQ(1, T::count);
    ASSERT_EQ("abcd", *p);
    tools::Delete(p, 1, a);
    ASSERT_TRUE(NULL == p);
    ASSERT_EQ(0, A::count);
    ASSERT_EQ(0, T::count);

    p = tools::New1<T>("abcde", 10);
    ASSERT_TRUE(NULL != p);
    ASSERT_EQ(0, A::count);
    ASSERT_EQ(10, T::count);
    for(int i = 0;i < 10;++i){
        ASSERT_EQ("abcde", p[i]);
    }
    tools::Delete(p, 10);
    ASSERT_TRUE(NULL == p);
    ASSERT_EQ(0, A::count);
    ASSERT_EQ(0, T::count);

    p = tools::New1<T>("abcdef");
    ASSERT_TRUE(NULL != p);
    ASSERT_EQ(0, A::count);
    ASSERT_EQ(1, T::count);
    ASSERT_EQ("abcdef", *p);
    tools::Delete(p, 1);
    ASSERT_TRUE(NULL == p);
    ASSERT_EQ(0, A::count);
    ASSERT_EQ(0, T::count);
}

TEST(New2, all)
{
    T * p = NULL;
    A a;
    ASSERT_TRUE(NULL == p);
    ASSERT_EQ(0, A::count);
    ASSERT_EQ(0, T::count);

    p = tools::New2<T>("abc", .1, a, 10);
    ASSERT_TRUE(NULL != p);
    ASSERT_EQ(10, A::count);
    ASSERT_EQ(10, T::count);
    for(int i = 0;i < 10;++i){
        ASSERT_EQ("abc", p[i]);
    }
    tools::Delete(p, 10, a);
    ASSERT_TRUE(NULL == p);
    ASSERT_EQ(0, A::count);
    ASSERT_EQ(0, T::count);

    p = tools::New2<T>("abcd", .2, a);
    ASSERT_TRUE(NULL != p);
    ASSERT_EQ(1, A::count);
    ASSERT_EQ(1, T::count);
    ASSERT_EQ("abcd", *p);
    tools::Delete(p, 1, a);
    ASSERT_TRUE(NULL == p);
    ASSERT_EQ(0, A::count);
    ASSERT_EQ(0, T::count);

    p = tools::New2<T>("abcde", .30, 10);
    ASSERT_TRUE(NULL != p);
    ASSERT_EQ(0, A::count);
    ASSERT_EQ(10, T::count);
    for(int i = 0;i < 10;++i){
        ASSERT_EQ("abcde", p[i]);
    }
    tools::Delete(p, 10);
    ASSERT_TRUE(NULL == p);
    ASSERT_EQ(0, A::count);
    ASSERT_EQ(0, T::count);

    p = tools::New2<T>("abcdef", .4);
    ASSERT_TRUE(NULL != p);
    ASSERT_EQ(0, A::count);
    ASSERT_EQ(1, T::count);
    ASSERT_EQ("abcdef", *p);
    tools::Delete(p, 1);
    ASSERT_TRUE(NULL == p);
    ASSERT_EQ(0, A::count);
    ASSERT_EQ(0, T::count);
}

TEST(Delete, all)
{
    T * p = NULL;
    A a;
    ASSERT_TRUE(NULL == p);
    ASSERT_EQ(0, A::count);
    ASSERT_EQ(0, B::count);
    ASSERT_EQ(0, T::count);

    p = tools::New<T>(a, 10);
    ASSERT_TRUE(NULL != p);
    ASSERT_EQ(10, A::count);
    ASSERT_EQ(10, B::count);
    ASSERT_EQ(10, T::count);
    tools::Delete(p, 10, a);
    ASSERT_TRUE(NULL == p);
    ASSERT_EQ(0, A::count);
    ASSERT_EQ(0, B::count);
    ASSERT_EQ(0, T::count);

    p = tools::New<T>(a, 1);
    ASSERT_TRUE(NULL != p);
    ASSERT_EQ(1, A::count);
    ASSERT_EQ(1, B::count);
    ASSERT_EQ(1, T::count);
    tools::Delete(p, a);
    ASSERT_TRUE(NULL == p);
    ASSERT_EQ(0, A::count);
    ASSERT_EQ(0, B::count);
    ASSERT_EQ(0, T::count);

    /*TODO
    p = tools::New<T>(a, 1);
    ASSERT_TRUE(NULL != p);
    ASSERT_EQ(1, A::count);
    ASSERT_EQ(1, B::count);
    ASSERT_EQ(1, T::count);
    B * b = p;
    tools::Delete(b, a);
    ASSERT_TRUE(NULL == p);
    ASSERT_EQ(0, A::count);
    ASSERT_EQ(0, B::count);
    ASSERT_EQ(0, T::count);
    */

    p = tools::New<T>(10);
    ASSERT_TRUE(NULL != p);
    ASSERT_EQ(0, A::count);
    ASSERT_EQ(10, B::count);
    ASSERT_EQ(10, T::count);
    tools::Delete(p, 10);
    ASSERT_TRUE(NULL == p);
    ASSERT_EQ(0, A::count);
    ASSERT_EQ(0, B::count);
    ASSERT_EQ(0, T::count);

    p = tools::New<T>(1);
    ASSERT_TRUE(NULL != p);
    ASSERT_EQ(0, A::count);
    ASSERT_EQ(1, B::count);
    ASSERT_EQ(1, T::count);
    tools::Delete(p);
    ASSERT_TRUE(NULL == p);
    ASSERT_EQ(0, A::count);
    ASSERT_EQ(0, B::count);
    ASSERT_EQ(0, T::count);
}
