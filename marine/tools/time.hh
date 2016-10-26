#ifndef DOZERG_TOOLS_TIME_H_20130122
#define DOZERG_TOOLS_TIME_H_20130122

/*
    时间相关辅助函数
        GetAbsTimespec
        GetRelativeTimespec
        Time
        GetTimeUs
        MonoTime
        MonoTimeUs
        TimeString
        TimeStringUs
        IsTimeout
*/

#include <sys/time.h>
#include <stdint.h>
#include <string>
#include "../to_string.hh"
#include "../impl/environment.hh"

NS_SERVER_BEGIN

namespace tools
{
    //根据timeMs毫秒生成绝对到期时间
    //timeMs: 到期时间，毫秒
    //ts: 结果timespec
    //return: true-succ; false-failed
    inline bool GetAbsTimespec(uint32_t timeMs, struct timespec * ts)
    {
        if(NULL == ts)
            return false;
#ifdef __USE_CLOCK_GETTIME
#   ifdef CLOCK_REALTIME_COARSE
        if(0 != ::clock_gettime(CLOCK_REALTIME_COARSE, ts))
#   else
        if(0 != ::clock_gettime(CLOCK_REALTIME, ts))
#   endif
            return false;
        const unsigned long long nsec = 1000000ULL * timeMs + ts->tv_nsec;
        ts->tv_nsec = nsec % 1000000000ULL;
        ts->tv_sec += nsec / 1000000000ULL;
#else
        struct timeval now;
        if(0 != ::gettimeofday(&now, NULL))
            return false;
        const unsigned long usec = now.tv_usec + timeMs * 1000UL;
        ts->tv_sec = now.tv_sec + usec / 1000000;
        ts->tv_nsec = (usec % 1000000) * 1000;
#endif
        return true;
    }

    //根据timeMs毫秒生成超时时间
    //timeMs: 到期时间，毫秒
    //ts: 结果timespec
    //return: true-succ; false-failed
    inline bool GetRelativeTimespec(uint32_t timeMs, struct timespec * ts)
    {
        if(NULL == ts)
            return false;
        ts->tv_sec = timeMs / 1000;
        ts->tv_nsec = timeMs % 1000 * 1000000;
        return true;
    }

    //得到秒级时间
    //timeS:
    //  NULL    忽略
    //  其他    返回当前时间，秒
    //fine:
    //  true    使用最高精度，等同于系统time调用，性能约 1000 ns/call
    //  false   使用一般精度，性能约 15 ns/call
    //return:
    //  0       失败
    //  其他    当前时间，秒
    inline time_t Time(time_t * timeS, bool fine = false)
    {
#ifdef __USE_CLOCK_GETTIME
        if(fine){
            struct timespec ts;
            if(0 != ::clock_gettime(CLOCK_REALTIME, &ts))
                return 0;
            if(timeS)
                *timeS = ts.tv_sec;
            return ts.tv_sec;
        }else
            return ::time(timeS);
#else
        return ::time(timeS);
#endif
    }

    //得到微秒级的当前时间 + elapse(微秒)
    //fine:
    //  true    使用最高精度，性能约 1000 ns/call
    //  false   使用一般精度，性能约 15 ns/call
    //return:
    //  0       出错
    //  other   微秒级的当前时间 + elapse
    inline uint64_t GetTimeUs(int64_t elapse = 0, bool fine = false)
    {
        uint64_t cur = 0;
#ifdef __USE_CLOCK_GETTIME
        struct timespec ts;
#   ifdef CLOCK_REALTIME_COARSE
        if(0 != ::clock_gettime((fine ? CLOCK_REALTIME : CLOCK_REALTIME_COARSE), &ts))
#   else
        if(0 != ::clock_gettime(CLOCK_REALTIME, &ts))
#   endif
            return 0;
        cur = ts.tv_sec;
        cur *= 1000000;
        cur += ts.tv_nsec / 1000;
#else
        struct timeval now;
        if(0 != ::gettimeofday(&now,0))
            return 0;
        cur = now.tv_sec;
        cur *= 1000000;
        cur += now.tv_usec;
#endif
        return cur + elapse;
    }

    //TODO: unit test
    //获取当前机器时间(monotonic time)，计算时间差
    //times:
    //  NULL    忽略
    //  其他    输入旧的机器时间，输出当前机器时间
    //fine:
    //  true    使用最高精度，性能约 1000 ns/call
    //  false   使用一般精度，性能约 20 ns/call
    //return: 当前时间，或者与timeS输入的旧时间的差值，秒
    //  0       同一秒内
    //  <0      当前时间在timeS之前
    //  >0      当前时间在timeS之后
    inline time_t MonoTime(time_t * timeS = NULL, bool fine = true)
    {
        time_t cur = 0;
#ifdef __USE_CLOCK_GETTIME
        struct timespec ts;
#   ifdef CLOCK_MONOTONIC_COARSE
        if(0 != ::clock_gettime((fine ? CLOCK_MONOTONIC : CLOCK_MONOTONIC_COARSE), &ts))
#   else
        if(0 != ::clock_gettime(CLOCK_MONOTONIC, &ts))
#   endif
            return 0;
        cur = ts.tv_sec;
#else
        cur = Time(NULL, fine);
#endif
        if(NULL != timeS){
            cur -= *timeS;
            *timeS += cur;
        }
        return cur;
    }

    //获取当前机器时间(monotonic time)，计算时间差
    //timeUs:
    //  NULL    忽略
    //  其他    输入旧的机器时间，输出当前机器时间
    //fine:
    //  true    使用最高精度，性能约 1000 ns/call
    //  false   使用一般精度，性能约 20 ns/call
    //return: 当前时间，或者与timeUs输入的旧时间的差值，微秒
    //  0       出错
    //  <0      当前时间在timeUs之前
    //  >0      当前时间在timeUs之后
    inline int64_t MonoTimeUs(uint64_t * timeUs = NULL, bool fine = true)
    {
        uint64_t cur = 0;
#ifdef __USE_CLOCK_GETTIME
        struct timespec ts;
#   ifdef CLOCK_MONOTONIC_COARSE
        if(0 != ::clock_gettime((fine ? CLOCK_MONOTONIC : CLOCK_MONOTONIC_COARSE), &ts))
#   else
        if(0 != ::clock_gettime(CLOCK_MONOTONIC, &ts))
#   endif
            return 0;
        cur = ts.tv_sec;
        cur *= 1000000;
        cur += ts.tv_nsec / 1000;
#else
        cur = GetTimeUs();
#endif
        if(NULL != timeUs){
            cur -= *timeUs;
            *timeUs += cur;
        }
        return (!cur ? 1 : cur);
    }

    //秒级别的时间字符串,格式设置参考strftime函数
    inline std::string TimeString(uint32_t timeS, const std::string & format = "%y-%m-%d %H:%M:%S")
    {
        time_t t = timeS;
        struct tm cur_tm;
        if(NULL == localtime_r(&t, &cur_tm))
            return std::string();
        std::string buf(255, 0);
        buf.resize(strftime(&buf[0], buf.size(), format.c_str(), &cur_tm));
        return buf;
    }

    //微秒级别的时间字符串,格式设置参考strftime函数
    inline std::string TimeStringUs(uint64_t timeUs, const std::string & format = "%y-%m-%d %H:%M:%S")
    {
        CToString oss;
        oss<<TimeString(uint32_t(timeUs / 1000000), format)<<"."<<(timeUs % 1000000);
        return oss.str();
    }

    //判断是否超时
    //oldtime: 需要判断的时间,0表示永不超时
    //timeout: 超时间隔,<0表示永不超时
    //curtime: 当前时间,0表示从time(NULL)取
    //jumping: 允许的时间跳变
    inline bool IsTimeout(time_t oldtime, int timeout, time_t curtime = 0, int jumping = 60)
    {
        if(!oldtime || timeout < 0)
            return false;   //不会超时
        if(!curtime)
            curtime = Time(NULL);
        return (curtime + jumping < oldtime
                || curtime > oldtime + timeout);
    }

}//namespace tools

NS_SERVER_END

#endif

