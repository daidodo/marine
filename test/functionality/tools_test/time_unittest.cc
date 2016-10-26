#include <marine/tools/time.hh>

#include "../inc.h"

TEST(GetAbsTimespec, all)
{
    const uint64_t timeMs = 1234567890;
    struct timespec ts;
    ASSERT_TRUE(tools::GetAbsTimespec(timeMs, &ts));
    ASSERT_EQ(ts.tv_sec, int(tools::GetTimeUs(timeMs * 1000) / 1000000));
    ASSERT_FALSE(tools::GetAbsTimespec(5234, NULL));
}

TEST(GetRelativeTimespec, all)
{
    const uint32_t timeMs = 1234567890;
    struct timespec ts;
    ASSERT_TRUE(tools::GetRelativeTimespec(timeMs, &ts));
    ASSERT_EQ(ts.tv_sec, timeMs / 1000);
    ASSERT_EQ(ts.tv_nsec, timeMs % 1000 * 1000000);
    ASSERT_FALSE(tools::GetAbsTimespec(5234, NULL));
}

TEST(Time, all)
{
    time_t t1, t2 = 0;
    ASSERT_EQ(time(NULL), tools::Time(NULL));
    ASSERT_EQ(time(&t1), tools::Time(&t2));
    ASSERT_EQ(t1, t2);

    t2 = 0;
    ASSERT_EQ(time(&t1), tools::Time(&t2, true));
    ASSERT_EQ(t1, t2);

    t2 = 0;
    ASSERT_EQ(time(&t1), tools::Time(&t2, false));
    ASSERT_EQ(t1, t2);
}

TEST(GetTimeUs, all)
{
    ASSERT_TRUE( tools::GetTimeUs() > 0 );
    ASSERT_TRUE( tools::GetTimeUs(21311) > 0 );
    ASSERT_TRUE( tools::GetTimeUs(21311, true) > 0 );
    ASSERT_TRUE( tools::GetTimeUs(21311, false) > 0 );
}

TEST(MonoTime, all)
{
    const int SLEEP = 2;
    {
        time_t cur = tools::MonoTime();
        ASSERT_TRUE(cur > 0)<<cur;
        sleep(SLEEP);
        time_t elapse = tools::MonoTime(&cur);
        ASSERT_TRUE(elapse > 0);
        ASSERT_EQ(SLEEP, elapse);
    }{
        time_t cur = tools::MonoTime(NULL, true);
        ASSERT_TRUE(cur > 0);
        sleep(SLEEP);
        time_t elapse = tools::MonoTime(&cur, true);
        ASSERT_TRUE(elapse > 0);
    }{
        time_t cur = tools::MonoTime(NULL, false);
        ASSERT_TRUE(cur > 0);
        sleep(SLEEP);
        time_t elapse = tools::MonoTime(&cur, false);
        ASSERT_TRUE(elapse > 0);
        ASSERT_EQ(SLEEP, elapse);
    }
}

TEST(MonoTimeUs, all)
{
    const int SLEEP = 3;
    {
        uint64_t cur = tools::MonoTimeUs();
        ASSERT_TRUE(cur > 0)<<cur;
        sleep(SLEEP);
        int64_t elapse = tools::MonoTimeUs(&cur);
        ASSERT_TRUE(elapse > 0);
        ASSERT_EQ(SLEEP, elapse / 1000000);
    }{
        uint64_t cur = tools::MonoTimeUs(NULL, true);
        ASSERT_TRUE(cur > 0);
        sleep(SLEEP);
        int64_t elapse = tools::MonoTimeUs(&cur, true);
        ASSERT_TRUE(elapse > 0);
    }{
        uint64_t cur = tools::MonoTimeUs(NULL, false);
        ASSERT_TRUE(cur > 0);
        sleep(SLEEP);
        int64_t elapse = tools::MonoTimeUs(&cur, false);
        ASSERT_TRUE(elapse > 0);
        ASSERT_EQ(SLEEP, elapse / 1000000);
    }
}

TEST(TimeString, all)
{
    const uint32_t NOW = 1357714755;
    ASSERT_EQ("13-01-09 14:59:15", tools::TimeString(NOW));
    ASSERT_EQ("2013/01/09 14-59-15", tools::TimeString(NOW, "%Y/%m/%d %H-%M-%S"));
}

TEST(TimeStringUs, all)
{
    const uint64_t NOW = 1357714755ull * 1000000 + 123456;
    ASSERT_EQ("13-01-09 14:59:15.123456", tools::TimeStringUs(NOW));
    ASSERT_EQ("2013/01/09 14-59-15.123456", tools::TimeStringUs(NOW, "%Y/%m/%d %H-%M-%S"));
}

TEST(IsTimeout, all)
{
    time_t old = time(NULL);
    int timeout = 10;
    int jumping = 60;

    time_t cur = old - jumping - 1;
    ASSERT_TRUE(tools::IsTimeout(old, timeout, cur, jumping));

    cur = old - jumping + 1;
    ASSERT_FALSE(tools::IsTimeout(old, timeout, cur, jumping));

    cur = old + 1;
    ASSERT_FALSE(tools::IsTimeout(old, timeout, cur, jumping));

    cur = old + timeout - 1;
    ASSERT_FALSE(tools::IsTimeout(old, timeout, cur, jumping));

    cur = old + timeout + 1;
    ASSERT_TRUE(tools::IsTimeout(old, timeout, cur, jumping));
}

TEST(IsTimeout, jumping)
{
    time_t old = time(NULL);
    int timeout = 10;
    int jumping = 60;

    time_t cur = old - jumping - 1;
    ASSERT_TRUE(tools::IsTimeout(old, timeout, cur));

    cur = old - jumping + 1;
    ASSERT_FALSE(tools::IsTimeout(old, timeout, cur));

    cur = old + 1;
    ASSERT_FALSE(tools::IsTimeout(old, timeout, cur));

    cur = old + timeout - 1;
    ASSERT_FALSE(tools::IsTimeout(old, timeout, cur));

    cur = old + timeout + 1;
    ASSERT_TRUE(tools::IsTimeout(old, timeout, cur));
}

TEST(IsTimeout, cur)
{
    time_t cur = time(NULL);
    int timeout = 10;
    int jumping = 60;

    time_t old = cur + jumping + 1;
    ASSERT_TRUE(tools::IsTimeout(old, timeout));

    old = cur + jumping - 1;
    ASSERT_FALSE(tools::IsTimeout(old, timeout));

    old = cur - 1;
    ASSERT_FALSE(tools::IsTimeout(old, timeout));

    old = cur - timeout + 1;
    ASSERT_FALSE(tools::IsTimeout(old, timeout));

    old = cur - timeout - 1;
    ASSERT_TRUE(tools::IsTimeout(old, timeout));
}


