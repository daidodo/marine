#ifndef DOZERG_TOOLS_TIME_H_20130122
#define DOZERG_TOOLS_TIME_H_20130122

/*
    ʱ����ظ�������
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
    //����timeMs�������ɾ��Ե���ʱ��
    //timeMs: ����ʱ�䣬����
    //ts: ���timespec
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

    //����timeMs�������ɳ�ʱʱ��
    //timeMs: ����ʱ�䣬����
    //ts: ���timespec
    //return: true-succ; false-failed
    inline bool GetRelativeTimespec(uint32_t timeMs, struct timespec * ts)
    {
        if(NULL == ts)
            return false;
        ts->tv_sec = timeMs / 1000;
        ts->tv_nsec = timeMs % 1000 * 1000000;
        return true;
    }

    //�õ��뼶ʱ��
    //timeS:
    //  NULL    ����
    //  ����    ���ص�ǰʱ�䣬��
    //fine:
    //  true    ʹ����߾��ȣ���ͬ��ϵͳtime���ã�����Լ 1000 ns/call
    //  false   ʹ��һ�㾫�ȣ�����Լ 15 ns/call
    //return:
    //  0       ʧ��
    //  ����    ��ǰʱ�䣬��
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

    //�õ�΢�뼶�ĵ�ǰʱ�� + elapse(΢��)
    //fine:
    //  true    ʹ����߾��ȣ�����Լ 1000 ns/call
    //  false   ʹ��һ�㾫�ȣ�����Լ 15 ns/call
    //return:
    //  0       ����
    //  other   ΢�뼶�ĵ�ǰʱ�� + elapse
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
    //��ȡ��ǰ����ʱ��(monotonic time)������ʱ���
    //times:
    //  NULL    ����
    //  ����    ����ɵĻ���ʱ�䣬�����ǰ����ʱ��
    //fine:
    //  true    ʹ����߾��ȣ�����Լ 1000 ns/call
    //  false   ʹ��һ�㾫�ȣ�����Լ 20 ns/call
    //return: ��ǰʱ�䣬������timeS����ľ�ʱ��Ĳ�ֵ����
    //  0       ͬһ����
    //  <0      ��ǰʱ����timeS֮ǰ
    //  >0      ��ǰʱ����timeS֮��
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

    //��ȡ��ǰ����ʱ��(monotonic time)������ʱ���
    //timeUs:
    //  NULL    ����
    //  ����    ����ɵĻ���ʱ�䣬�����ǰ����ʱ��
    //fine:
    //  true    ʹ����߾��ȣ�����Լ 1000 ns/call
    //  false   ʹ��һ�㾫�ȣ�����Լ 20 ns/call
    //return: ��ǰʱ�䣬������timeUs����ľ�ʱ��Ĳ�ֵ��΢��
    //  0       ����
    //  <0      ��ǰʱ����timeUs֮ǰ
    //  >0      ��ǰʱ����timeUs֮��
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

    //�뼶���ʱ���ַ���,��ʽ���òο�strftime����
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

    //΢�뼶���ʱ���ַ���,��ʽ���òο�strftime����
    inline std::string TimeStringUs(uint64_t timeUs, const std::string & format = "%y-%m-%d %H:%M:%S")
    {
        CToString oss;
        oss<<TimeString(uint32_t(timeUs / 1000000), format)<<"."<<(timeUs % 1000000);
        return oss.str();
    }

    //�ж��Ƿ�ʱ
    //oldtime: ��Ҫ�жϵ�ʱ��,0��ʾ������ʱ
    //timeout: ��ʱ���,<0��ʾ������ʱ
    //curtime: ��ǰʱ��,0��ʾ��time(NULL)ȡ
    //jumping: �����ʱ������
    inline bool IsTimeout(time_t oldtime, int timeout, time_t curtime = 0, int jumping = 60)
    {
        if(!oldtime || timeout < 0)
            return false;   //���ᳬʱ
        if(!curtime)
            curtime = Time(NULL);
        return (curtime + jumping < oldtime
                || curtime > oldtime + timeout);
    }

}//namespace tools

NS_SERVER_END

#endif

