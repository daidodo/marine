#include <marine/lock_queue.hh>

#include "../inc.h"

typedef CLockQueue<int>         __Que;
typedef __Que::container_type   __Con;

#define CAPA    10000

TEST(CLockQueue, init)
{
    __Con c;
    for(int i = 0;i < 100;++i)
        c.push_back(i);
    {
        __Que q;
        ASSERT_EQ(size_t(CAPA), q.capacity());
        ASSERT_EQ(size_t(0), q.size());
        ASSERT_EQ(size_t(0), q.topSize());
    }{
        __Que q(1000);
        ASSERT_EQ(size_t(1000), q.capacity());
        ASSERT_EQ(size_t(0), q.size());
        ASSERT_EQ(size_t(0), q.topSize());
    }{
        __Que q(c);
        ASSERT_EQ(size_t(CAPA), q.capacity());
        ASSERT_EQ(c.size(), q.size());
        ASSERT_EQ(c.size(), q.topSize());
    }{
        __Que q(c, 1000);
        ASSERT_EQ(size_t(1000), q.capacity());
        ASSERT_EQ(c.size(), q.size());
        ASSERT_EQ(c.size(), q.topSize());
    }
}

TEST(CLockQueue, capacity)
{
    __Que q;
    ASSERT_EQ(size_t(CAPA), q.capacity());

    q.capacity(1000);
    ASSERT_EQ(size_t(1000), q.capacity());
}

TEST(CLockQueue, empty_size)
{
    __Que q;
    ASSERT_EQ(size_t(0), q.size());
    ASSERT_TRUE(q.empty());

    for(int i = 0;i < 100;++i){
        ASSERT_TRUE(q.push(i));
        ASSERT_EQ(size_t(i + 1), q.size());
        ASSERT_FALSE(q.empty());
        ASSERT_EQ(size_t(i + 1), q.topSize());
    }
    for(int i = 0;i < 100;++i){
        ASSERT_EQ(size_t(100 - i), q.size());
        ASSERT_FALSE(q.empty());
        ASSERT_EQ(size_t(100), q.topSize());
        int x;
        ASSERT_TRUE(q.pop(x));
    }
    ASSERT_EQ(size_t(0), q.size());
    ASSERT_TRUE(q.empty());
    ASSERT_EQ(size_t(100), q.topSize());

    ASSERT_EQ(size_t(100), q.resetTopSize());
    ASSERT_EQ(size_t(0), q.topSize());
}

TEST(CLockQueue, push)
{
    __Que q;
    ASSERT_EQ(size_t(0), q.size());
    ASSERT_EQ(size_t(CAPA), q.capacity());

    for(int i = 0;i < CAPA;++i)
        ASSERT_TRUE(q.push(i));
    ASSERT_EQ(size_t(CAPA), q.size());

    ASSERT_FALSE(q.push(10001, 0));
    time_t s = tools::Time(NULL);
    for(int i = 0;i < 10;++i)
        ASSERT_FALSE(q.push(10001, 400));
    time_t e = tools::Time(NULL);
    ASSERT_EQ(4, e - s);
    ASSERT_EQ(size_t(CAPA), q.size());

    for(int i = 0;i < CAPA;++i){
        int x;
        ASSERT_TRUE(q.pop(x));
        ASSERT_EQ(int(i), x);
    }
    ASSERT_EQ(size_t(0), q.size());
}

TEST(CLockQueue, pushFront)
{
    __Que q;
    ASSERT_EQ(size_t(0), q.size());
    ASSERT_EQ(size_t(CAPA), q.capacity());

    for(int i = 0;i < CAPA;++i)
        ASSERT_TRUE(q.pushFront(i));
    ASSERT_EQ(size_t(CAPA), q.size());

    ASSERT_FALSE(q.pushFront(10001, 0));
    time_t s = tools::Time(NULL);
    for(int i = 0;i < 10;++i)
        ASSERT_FALSE(q.pushFront(10001, 400));
    time_t e = tools::Time(NULL);
    ASSERT_EQ(4, e - s);
    ASSERT_EQ(size_t(CAPA), q.size());

    for(int i = 0;i < CAPA;++i){
        int x;
        ASSERT_TRUE(q.pop(x));
        ASSERT_EQ(int(CAPA - i - 1), x);
    }
    ASSERT_EQ(size_t(0), q.size());
}

TEST(CLockQueue, pushAll)
{
    __Que q;
    ASSERT_EQ(size_t(0), q.size());
    ASSERT_EQ(size_t(CAPA), q.capacity());

    __Con c;
    ASSERT_EQ(size_t(0), c.size());

    ASSERT_TRUE(q.pushAll(c));  //empty
    ASSERT_EQ(size_t(0), q.size());
    ASSERT_EQ(size_t(0), c.size());

    c.resize(CAPA + 1, 1);
    ASSERT_EQ(size_t(CAPA + 1), c.size());

    ASSERT_FALSE(q.pushAll(c, 0));  // > capa
    {
        time_t s = tools::Time(NULL);
        for(int i = 0;i < 10;++i)
            ASSERT_FALSE(q.pushAll(c, 400));    // > capa
        time_t e = tools::Time(NULL);
        ASSERT_EQ(0, e - s);    //这里有个优化
    }
    ASSERT_EQ(size_t(0), q.size());
    ASSERT_EQ(size_t(CAPA + 1), c.size());

    c.clear();
    for(int i = 0;i < CAPA / 2;++i)
        c.push_back(i);
    ASSERT_EQ(size_t(CAPA / 2), c.size());

    ASSERT_TRUE(q.pushAll(c));  // <= capa
    ASSERT_EQ(size_t(CAPA / 2), q.size());
    ASSERT_EQ(size_t(0), c.size());

    c.clear();
    for(int i = CAPA / 2;i < CAPA;++i)
        c.push_back(i);
    ASSERT_EQ(size_t(CAPA / 2), c.size());

    ASSERT_TRUE(q.pushAll(c));  // <= capa
    ASSERT_EQ(size_t(CAPA), q.size());
    ASSERT_EQ(size_t(0), c.size());

    ASSERT_TRUE(q.pushAll(c));  //empty
    ASSERT_EQ(size_t(CAPA), q.size());
    ASSERT_EQ(size_t(0), c.size());

    c.push_back(10001);
    ASSERT_EQ(size_t(1), c.size());

    ASSERT_FALSE(q.pushAll(c, 0));  // > capa
    {
        time_t s = tools::Time(NULL);
        for(int i = 0;i < 10;++i)
            ASSERT_FALSE(q.pushAll(c, 400));    // > capa
        time_t e = tools::Time(NULL);
        ASSERT_EQ(4, e - s);
    }
    ASSERT_EQ(size_t(CAPA), q.size());
    ASSERT_EQ(size_t(1), c.size());

    for(int i = 0;i < CAPA;++i){
        int x;
        ASSERT_TRUE(q.pop(x));
        ASSERT_EQ(i, x);
    }
    ASSERT_EQ(size_t(0), q.size());
}

TEST(CLockQueue, pop)
{
    __Con c;
    for(int i = 0;i < 100;++i)
        c.push_back(i);

    __Que q(c);
    ASSERT_EQ(size_t(100), q.size());

    for(int i = 0;i < 100;++i){
        int x;
        ASSERT_TRUE(q.pop(x));
        ASSERT_EQ(size_t(100 - i - 1), q.size());
        ASSERT_EQ(i, x);
    }
    ASSERT_EQ(size_t(0), q.size());

    int x;
    ASSERT_FALSE(q.pop(x, 0));
    time_t s = tools::Time(NULL);
    for(int i = 0;i < 10;++i)
        ASSERT_FALSE(q.pop(x, 900));
    time_t e = tools::Time(NULL);
    ASSERT_EQ(9, e - s);
    ASSERT_EQ(size_t(0), q.size());
}

TEST(CLockQueue, popAll)
{
    __Con cc;
    for(int i = 0;i < 100;++i)
        cc.push_back(i);

    __Que q(cc);
    ASSERT_EQ(size_t(100), q.size());

    __Con c(10, 8);
    ASSERT_TRUE(q.popAll(c));
    ASSERT_EQ(size_t(0), q.size());
    ASSERT_EQ(size_t(100), c.size());

    __Con::const_iterator it = c.begin();
    for(int i = 0;i < 100;++i, ++it){
        ASSERT_NE(c.end(), it);
        ASSERT_EQ(i, *it);
    }

    ASSERT_FALSE(q.popAll(c, 0));
    ASSERT_EQ(size_t(0), c.size());
    time_t s = tools::Time(NULL);
    for(int i = 0;i < 10;++i)
        ASSERT_FALSE(q.popAll(c, 400));
    time_t e = tools::Time(NULL);
    ASSERT_EQ(4, e - s);
    ASSERT_EQ(size_t(0), q.size());
    ASSERT_EQ(size_t(0), c.size());
}
