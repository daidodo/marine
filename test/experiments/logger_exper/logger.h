#ifndef DOZERG_LOGGER_H_20130320
#define DOZERG_LOGGER_H_20130320

#include <string>
#include <sstream>

namespace im_logger {

    // logger levels
    enum ELevel
    {
        kOff,
        kFatal,
        kEvent,
        kError,
        kWarn,
        kNotice,
        kInfo,
        kDebug,
        kTrace,
        kMethod
    };

    // set logger level:
    // "OFF"
    // "FATAL"
    // "EVENT"
    // "ERROR"
    // "WARN"
    // "NOTICE"
    // "INFO"
    // "DEBUG"
    // "TRACE"
    // "METHOD"
    // others   -> "OFF"
    void LOGGER_SET_LEVEL(const std::string & level);

    // get logger level
    ELevel LOGGER_GET_LEVEL();

    // test if level should be logged
    bool LOGGER_SHOULD_LOG(ELevel level);

    // set file rolling interval
    void LOGGER_SET_SECONDS_PER_LOG(int sec);

    // initialize logger
    void LOGGER_INIT(const std::string & fname, const std::string & level, int secPerLog = 3600);

    // record a message
    void LOGGER_LOG(ELevel level, const char * file, int line, const char * func, const std::string & msg);
    void LOGGER_LOG(ELevel level, const char * file, int line, const char * func, const char * tag, const std::string & msg);

    // logger class
    struct CLocalLogger
    {
        explicit CLocalLogger(ELevel level, const char * file = NULL, int line = 0, const char * func = NULL, const char * tag = NULL);
        void log() const;
        std::ostringstream oss;
    private:
        time_t now_;
    };

} // namespace

#define LOGGER_LOGM(level, msg)  \
do{ \
    if(LOGGER_SHOULD_LOG(level)){  \
        im_logger::CLocalLogger logger(level, __FILE__, __LINE__, __FUNCTION__);  \
        logger.oss<<msg<<'\n'; \
        logger.log();   \
    }   \
}while(0)

#define LOGGER_METHOD(msg)  LOGGER_LOGM(im_logger::kMethod, msg)
#define LOGGER_TRACE(msg)   LOGGER_LOGM(im_logger::kTrace, msg)
#define LOGGER_DEBUG(msg)   LOGGER_LOGM(im_logger::kDebug, msg)
#define LOGGER_INFO(msg)    LOGGER_LOGM(im_logger::kInfo, msg)
#define LOGGER_NOTICE(msg)  LOGGER_LOGM(im_logger::kNotice, msg)
#define LOGGER_WARN(msg)    LOGGER_LOGM(im_logger::kWarn, msg)
#define LOGGER_ERROR(msg)   LOGGER_LOGM(im_logger::kError, msg)
#define LOGGER_EVENT(msg)   LOGGER_LOGM(im_logger::kEvent, msg)
#define LOGGER_FATAL(msg)   LOGGER_LOGM(im_logger::kFatal, msg)

// for convenience uses
#ifndef METHOD
#   define METHOD   LOGGER_METHOD
#endif
#ifndef TRACE
#   define TRACE    LOGGER_TRACE
#endif
#ifndef DEBUG
#   define DEBUG    LOGGER_DEBUG
#endif
#ifndef INFO
#   define INFO     LOGGER_INFO
#endif
#ifndef NOTICE
#   define NOTICE   LOGGER_NOTICE
#endif
#ifndef WARN
#   define WARN     LOGGER_WARN
#endif
#ifndef ERROR
#   define ERROR    LOGGER_ERROR
#endif
#ifndef EVENT
#   define EVENT    LOGGER_EVENT
#endif
#ifndef FATAL
#   define FATAL    LOGGER_FATAL
#endif

#endif

