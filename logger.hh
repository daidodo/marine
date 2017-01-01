#ifndef DOZERG_LOGGER_H_20130320
#define DOZERG_LOGGER_H_20130320

/*
    轻量级日志系统
        LOGGER_INIT             初始化logger
        LOGGER_SET_LEVEL        设置logger级别
        LOGGER_SET_MAX_SIZE     设置log文件的最大字节长度
        LOGGER_SET_MAX_NUMBER   设置备份log文件的个数
        LOGGER_CRASH_HANDLER    注册线程崩溃时的信号处理函数和堆栈
        LOGGER_TRACE/TRACE      输出指定级别的log
        LOGGER_DEBUG/DEBUG
        LOGGER_INFO/INFO
        LOGGER_WARN/WARN
        LOGGER_ERROR/ERROR
        LOGGER_FATAL/FATAL
        LOGGER_ASSERT/ASSERT    可输出log的断言
//*/

#include "impl/logger_impl.hh"

NS_SERVER_BEGIN

//设置logger级别
//level:
//  "TRACE"     记录TRACE及以上log
//  "DEBUG"     记录DEBUG及以上log
//  "INFO"      记录INFO及以上log
//  "WARN"      记录WARN及以上log
//  "ERROR"     记录ERROR及以上log
//  "FATAL"     记录FATAL及以上log
//  "OFF"或其他 关闭log
inline void LOGGER_SET_LEVEL(const std::string & level)
{
    NS_IMPL::CGlobalLogger::Instance().setLevel(level);
}

//设置log文件的最大字节长度
//maxSize:
//  <4K     log文件最大4K
//  其他    log文件最大字节数
inline void LOGGER_SET_MAX_SIZE(size_t maxSize)
{
    NS_IMPL::CGlobalLogger::Instance().setMaxSize(maxSize);
}

//设置备份log文件的个数
inline void LOGGER_SET_MAX_NUMBER(int maxNumber)
{
    NS_IMPL::CGlobalLogger::Instance().setMaxNumber(maxNumber);
}

//注册线程崩溃时的信号处理函数和堆栈
//当前处理的信号有：SIGSEGV, SIGBUS, SIGFPE, SIGILL, SIGABRT
//每个线程应该单独注册，否则信号处理函数的堆栈设置会无效
//return:
//  true    succ
//  false   failed
inline bool LOGGER_CRASH_HANDLER()
{
    return NS_IMPL::CGlobalLogger::Instance().registerCrashHandler();
}

//初始化logger
//fname: log文件名，备份log文件名为: fname.1, fname.2, ...
//level: 同LOGGER_SET_LEVEL(level)
//maxSize: 同LOGGER_SET_MAX_SIZE(maxSize)
//maxNumber: 同LOGGER_SET_MAX_NUMBER(maxNumber)
//return:
//  true    succ
//  false   failed
inline bool LOGGER_INIT(const std::string & fname, const std::string & level, size_t maxSize, int maxNumber)
{
    LOGGER_SET_LEVEL(level);
    LOGGER_SET_MAX_SIZE(maxSize);
    LOGGER_SET_MAX_NUMBER(maxNumber);
    return NS_IMPL::CGlobalLogger::Instance().init(fname);
}

//记录指定level的log
//console: 是否同时在终端打印出来
#define LOGGER_LOG(level, msg, console)  \
do{ \
    const bool file = NS_SERVER::NS_IMPL::CGlobalLogger::Instance().shouldLog(level);    \
    if(file || console){    \
        NS_SERVER::NS_IMPL::CLocalLogger logger(level, __FILE__, __LINE__, __FUNCTION__);  \
        logger.oss()<<msg<<'\n'; \
        if(file) \
            logger.log();   \
        if(console)   \
            logger.print(); \
    }   \
}while(0)

//记录log
#define LOGGER_TRACE(msg)   LOGGER_LOG(NS_SERVER::NS_IMPL::CGlobalLogger::kTrace, msg, false)
#define LOGGER_DEBUG(msg)   LOGGER_LOG(NS_SERVER::NS_IMPL::CGlobalLogger::kDebug, msg, false)
#define LOGGER_INFO(msg)    LOGGER_LOG(NS_SERVER::NS_IMPL::CGlobalLogger::kInfo, msg, false)
#define LOGGER_WARN(msg)    LOGGER_LOG(NS_SERVER::NS_IMPL::CGlobalLogger::kWarn, msg, false)
#define LOGGER_ERROR(msg)   LOGGER_LOG(NS_SERVER::NS_IMPL::CGlobalLogger::kError, msg, false)
#define LOGGER_FATAL(msg)   LOGGER_LOG(NS_SERVER::NS_IMPL::CGlobalLogger::kFatal, msg, true)

//断言log
#ifndef NDEBUG
#   define LOGGER_ASSERT(expr, msg) do{if(!(expr)){LOGGER_FATAL("ASSERT(" #expr ") failed: "<<msg);::abort();}}while(0)
#else
#   define LOGGER_ASSERT(expr, msg)
#endif

//方便使用
//注意：如果其他库或文件也定义了以下的宏，则需要在本文件之前#include
#ifndef TRACE
#   define TRACE    LOGGER_TRACE
#endif
#ifndef DEBUG
#   define DEBUG    LOGGER_DEBUG
#endif
#ifndef INFO
#   define INFO     LOGGER_INFO
#endif
#ifndef WARN
#   define WARN     LOGGER_WARN
#endif
#ifndef ERROR
#   define ERROR    LOGGER_ERROR
#endif
#ifndef FATAL
#   define FATAL    LOGGER_FATAL
#endif
#ifndef ASSERT
#   define ASSERT   LOGGER_ASSERT
#endif

NS_SERVER_END

#endif

