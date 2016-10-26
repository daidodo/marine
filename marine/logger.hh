#ifndef DOZERG_LOGGER_H_20130320
#define DOZERG_LOGGER_H_20130320

/*
    ��������־ϵͳ
        LOGGER_INIT             ��ʼ��logger
        LOGGER_SET_LEVEL        ����logger����
        LOGGER_SET_MAX_SIZE     ����log�ļ�������ֽڳ���
        LOGGER_SET_MAX_NUMBER   ���ñ���log�ļ��ĸ���
        LOGGER_CRASH_HANDLER    ע���̱߳���ʱ���źŴ������Ͷ�ջ
        LOGGER_TRACE/TRACE      ���ָ�������log
        LOGGER_DEBUG/DEBUG
        LOGGER_INFO/INFO
        LOGGER_WARN/WARN
        LOGGER_ERROR/ERROR
        LOGGER_FATAL/FATAL
        LOGGER_ASSERT/ASSERT    �����log�Ķ���
//*/

#include "impl/logger_impl.hh"

NS_SERVER_BEGIN

//����logger����
//level:
//  "TRACE"     ��¼TRACE������log
//  "DEBUG"     ��¼DEBUG������log
//  "INFO"      ��¼INFO������log
//  "WARN"      ��¼WARN������log
//  "ERROR"     ��¼ERROR������log
//  "FATAL"     ��¼FATAL������log
//  "OFF"������ �ر�log
inline void LOGGER_SET_LEVEL(const std::string & level)
{
    NS_IMPL::CGlobalLogger::Instance().setLevel(level);
}

//����log�ļ�������ֽڳ���
//maxSize:
//  <4K     log�ļ����4K
//  ����    log�ļ�����ֽ���
inline void LOGGER_SET_MAX_SIZE(size_t maxSize)
{
    NS_IMPL::CGlobalLogger::Instance().setMaxSize(maxSize);
}

//���ñ���log�ļ��ĸ���
inline void LOGGER_SET_MAX_NUMBER(int maxNumber)
{
    NS_IMPL::CGlobalLogger::Instance().setMaxNumber(maxNumber);
}

//ע���̱߳���ʱ���źŴ������Ͷ�ջ
//��ǰ������ź��У�SIGSEGV, SIGBUS, SIGFPE, SIGILL, SIGABRT
//ÿ���߳�Ӧ�õ���ע�ᣬ�����źŴ������Ķ�ջ���û���Ч
//return:
//  true    succ
//  false   failed
inline bool LOGGER_CRASH_HANDLER()
{
    return NS_IMPL::CGlobalLogger::Instance().registerCrashHandler();
}

//��ʼ��logger
//fname: log�ļ���������log�ļ���Ϊ: fname.1, fname.2, ...
//level: ͬLOGGER_SET_LEVEL(level)
//maxSize: ͬLOGGER_SET_MAX_SIZE(maxSize)
//maxNumber: ͬLOGGER_SET_MAX_NUMBER(maxNumber)
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

//��¼ָ��level��log
//console: �Ƿ�ͬʱ���ն˴�ӡ����
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

//��¼log
#define LOGGER_TRACE(msg)   LOGGER_LOG(NS_SERVER::NS_IMPL::CGlobalLogger::kTrace, msg, false)
#define LOGGER_DEBUG(msg)   LOGGER_LOG(NS_SERVER::NS_IMPL::CGlobalLogger::kDebug, msg, false)
#define LOGGER_INFO(msg)    LOGGER_LOG(NS_SERVER::NS_IMPL::CGlobalLogger::kInfo, msg, false)
#define LOGGER_WARN(msg)    LOGGER_LOG(NS_SERVER::NS_IMPL::CGlobalLogger::kWarn, msg, false)
#define LOGGER_ERROR(msg)   LOGGER_LOG(NS_SERVER::NS_IMPL::CGlobalLogger::kError, msg, false)
#define LOGGER_FATAL(msg)   LOGGER_LOG(NS_SERVER::NS_IMPL::CGlobalLogger::kFatal, msg, true)

//����log
#ifndef NDEBUG
#   define LOGGER_ASSERT(expr, msg) do{if(!(expr)){LOGGER_FATAL("ASSERT(" #expr ") failed: "<<msg);::abort();}}while(0)
#else
#   define LOGGER_ASSERT(expr, msg)
#endif

//����ʹ��
//ע�⣺�����������ļ�Ҳ���������µĺ꣬����Ҫ�ڱ��ļ�֮ǰ#include
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

