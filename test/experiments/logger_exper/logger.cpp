#include "logger.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#ifdef _WIN32
#   include <io.h>
#   define open _open
#   define write _write
#   define close _close
#else
#   include <unistd.h>
#endif

#include <string>

namespace im_logger {

    namespace impl {

        struct CGlobalLogger
        {
            static CGlobalLogger & Instance(){
                static CGlobalLogger inst;
                return inst;
            }
            void setLevel(const std::string & level){
                if("METHOD" == level)
                    level_ = kMethod;
                else if("TRACE" == level)
                    level_ = kTrace;
                else if("DEBUG" == level)
                    level_ = kDebug;
                else if("INFO" == level)
                    level_ = kInfo;
                else if("NOTICE" == level)
                    level_ = kNotice;
                else if("WARN" == level)
                    level_ = kWarn;
                else if("ERROR" == level)
                    level_ = kError;
                else if("EVENT" == level)
                    level_ = kEvent;
                else if("FATAL" == level)
                    level_ = kFatal;
                else
                    level_ = kOff;
            }
            ELevel getLevel() const{return level_;}
            void setSecPerLog(int sec){ secPerLog_ = (sec < 1 ? 3600 : sec); }
            void init(const std::string & fname){fname_ = fname;}
            bool log(time_t now, const std::string & msg){
                const int kOpenFlags = O_WRONLY | O_CREAT | O_APPEND;
#ifdef _WIN32
                const int kCreatMode = _S_IREAD | _S_IWRITE;
#else
                const int kCreatMode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
#endif
                if(msg.empty())
                    return true;
                now = now / secPerLog_ * secPerLog_;
                std::ostringstream oss;
                oss<<fname_<<'.'<<now;
                const int fd = ::open(oss.str().c_str(), kOpenFlags, kCreatMode);
                if(fd < 0)
                    return false;
                if(::write(fd, msg.c_str(), msg.size()) < 0)
                    return false;
                return (0 == ::close(fd));
            }
            bool shouldLog(ELevel level) const{return level <= level_;}
        private:
            CGlobalLogger()
                : secPerLog_(3600)
                  , level_(kOff)
            {}
            CGlobalLogger(const CGlobalLogger &);   //disable copy & assignment
            CGlobalLogger & operator =(const CGlobalLogger &);
            ~CGlobalLogger(){}
            //fields
            std::string fname_;
            int secPerLog_;
            ELevel level_;
        };

        static const char * LevelName(ELevel level)
        {
            switch(level){
                case kMethod:return "METHOD";
                case kTrace:return "TRACE";
                case kDebug:return "DEBUG";
                case kInfo:return "INFO";
                case kNotice:return "NOTICE";
                case kWarn:return "WARN";
                case kError:return "ERROR";
                case kEvent:return "EVENT";
                case kFatal:return "FATAL";
                default:;
            }
            return "UNKNOWN";
        }

        static std::string TimeString(time_t timeS)
        {
            time_t t = timeS;
            struct tm cur_tm;
#ifdef _WIN32
            if (localtime_s(&cur_tm, &t))
                return std::string();
#else
            if (NULL == ::localtime_r(&t, &cur_tm))
                return std::string();
#endif
            std::string buf(255, 0);
            buf.resize(::strftime(&buf[0], buf.size(), "%y-%m-%d %H:%M:%S", &cur_tm));
            return buf;
        }

        static const char * Basename(const char * fname)
        {
            const char * r = fname;
            for(bool b = false;fname && *fname;++fname){
#ifdef _WIN32
                if ('/' == *fname || '\\' == *fname)
#else
                    if ('/' == *fname)
#endif
                        b = true;
                    else{
                        if(b)
                            r = fname;
                        b = false;
                    }
            }
            return r;
        }

    } // namespace

    CLocalLogger::CLocalLogger(ELevel level, const char * file, int line, const char * func, const char * tag)
        : now_(::time(NULL))
    {
        oss<<'['<<impl::TimeString(now_)<<']'
            <<'['<<impl::LevelName(level)<<']';
        if(file && line)
            oss<<'['<<impl::Basename(file)<<':'<<line<<']';
        if(func)
            oss<<'['<<func<<']';
        if(tag)
            oss<<'['<<tag<<']';
    }

    void CLocalLogger::log() const{impl::CGlobalLogger::Instance().log(now_, oss.str());}

    void LOGGER_SET_LEVEL(const std::string & level)
    {
        impl::CGlobalLogger::Instance().setLevel(level);
    }

    ELevel LOGGER_GET_LEVEL()
    {
        return impl::CGlobalLogger::Instance().getLevel();
    }

    bool LOGGER_SHOULD_LOG(ELevel level)
    {
        return impl::CGlobalLogger::Instance().shouldLog(level);
    }

    void LOGGER_SET_SECONDS_PER_LOG(int sec)
    {
        impl::CGlobalLogger::Instance().setSecPerLog(sec);
    }

    void LOGGER_INIT(const std::string & fname, const std::string & level, int secPerLog)
    {
        LOGGER_SET_LEVEL(level);
        LOGGER_SET_SECONDS_PER_LOG(secPerLog);
        impl::CGlobalLogger::Instance().init(fname);
    }

    void LOGGER_LOG(ELevel level, const char * file, int line, const char * func, const char * tag, const std::string & msg)
    {
        if(!LOGGER_SHOULD_LOG(level))
            return;
        CLocalLogger logger(level, file, line, func, tag);
        logger.oss<<msg<<'\n';
        logger.log();
    }

    void LOGGER_LOG(ELevel level, const char * file, int line, const char * func, const std::string & msg)
    {
        LOGGER_LOG(level, file, line, func, NULL, msg);
    }

} //namespace
