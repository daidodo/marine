#ifndef DOZERG_LOGGER_IMPL_H_20130320
#define DOZERG_LOGGER_IMPL_H_20130320

#include <sys/prctl.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <execinfo.h>
#include <algorithm>
#include <iostream>
#include <string>
#include <cassert>
#include "../file.hh"
#include "../atomic_sync.hh"
#include "../mutex.hh"
#include "../tools/time.hh"     //TimeStringUs, GetTimeUs
#include "../tools/string.hh"   //AbsFilename, Basename
#include "../tools/signal.hh"
#include "save_stack_impl.hh"

NS_IMPL_BEGIN

class CGlobalLogger
{
    typedef CGlobalLogger   __Myt;
    typedef CGuard<CMutex>  __Guard;
    struct CSignalParams{
        char msg_[4096];
        CAddrStack stack_;
        CSymbolInfo sym_;
        siginfo_t info_;
        pthread_t entered_;
        int again_;
        int sig_;
        CSignalParams()
            : entered_(0)
            , again_(0)
            , sig_(0)
        {}
    };
    struct CSignalStack{
        char buf_[SIGSTKSZ];
    };
    typedef CThreadData<CSignalStack>   __SignalStack;
public:
    enum ELevel{
        kOff,
        kFatal,
        kError,
        kWarn,
        kInfo,
        kDebug,
        kTrace
    };
    static CGlobalLogger & Instance(){
        static CGlobalLogger inst;
        return inst;
    }
    static const char * LevelName(ELevel level){
        switch(level){
            case kTrace:return "TRACE";
            case kDebug:return "DEBUG";
            case kInfo:return "INFO";
            case kWarn:return "WARN";
            case kError:return "ERROR";
            case kFatal:return "FATAL";
            default:;
        }
        return "UNKNOWN";
    }
    void setLevel(const std::string & level){
        if("TRACE" == level)
            level_ = kTrace;
        else if("DEBUG" == level)
            level_ = kDebug;
        else if("INFO" == level)
            level_ = kInfo;
        else if("WARN" == level)
            level_ = kWarn;
        else if("ERROR" == level)
            level_ = kError;
        else if("FATAL" == level)
            level_ = kFatal;
        else
            level_ = kOff;
    }
    void setMaxSize(size_t maxSize){maxSize_ = std::max(size_t(4 << 10), maxSize);}
    void setMaxNumber(int maxNumber){maxNumber_ = maxNumber;}
    bool init(const std::string & fname){
        //get process name
        exe_.resize(1024);
        ssize_t ret = ::readlink("/proc/self/exe", &exe_[0], exe_.size());
        if(ret < 0)
            ret = 0;
        exe_.resize(ret);
        exe_ = tools::Basename(exe_);
        //init logger
        std::string abs = tools::AbsFilename(fname);
        __Guard g(lock_);
        fname_ = abs;
        if(file_.valid())
            file_.close();  //re-init
        if(level_ > kOff){
            if(!ensureFile())
                return false;
            fname_ = file_.filename();
        }
        return true;
    }
    bool log(const std::string & msg){
        if(msg.empty())
            return true;
        __Guard g(lock_);
        if(file_.valid()){
            if(file_.deleted()){
                file_.close();  //re-open later
            }else if(maxSize_ > 0){
                off_t len = file_.length();
                if(len >= 0 && size_t(len) >= maxSize_)
                    rollFile();
            }
        }
        if(!writeLog(msg)){
            //re-open and try
            file_.close();
            if(!writeLog(msg))
                return false;
        }
        return true;
    }
    bool shouldLog(ELevel level) const{return level <= level_;}
    bool registerCrashHandler(){
        //init key
        stackInit_.runOnce(&InitSignalStack);
        if(NULL == stack_)
            return false;
        //set signal handler stack
        ::stack_t st;
        st.ss_sp = stack_->ref().buf_;
        st.ss_size = sizeof stack_->ref().buf_;
        st.ss_flags = 0;
        if(0 != ::sigaltstack(&st, NULL))
            return false;
        //register signal handler
        struct sigaction act;
        ::sigemptyset(&act.sa_mask);
        act.sa_flags = SA_ONSTACK | SA_SIGINFO | SA_NODEFER;
        act.sa_sigaction = &SignalHandler;
        ::sigaction(SIGSEGV, &act, NULL);
        ::sigaction(SIGBUS, &act, NULL);
        ::sigaction(SIGFPE, &act, NULL);
        ::sigaction(SIGILL, &act, NULL);
        ::sigaction(SIGABRT, &act, NULL);
        return true;
    }
private:
    CGlobalLogger()
        : stack_(NULL)
        , maxSize_(0)
        , maxNumber_(0)
        , level_(kOff)
    {}
    ~CGlobalLogger(){
        if(stack_)
            tools::Delete(stack_);
    }
    bool ensureFile(){
        const int kOpenFlags = O_WRONLY | O_CREAT | O_APPEND;
        const int kCreatMode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
        return (file_.valid()
                || (!fname_.empty() && file_.open(fname_, kOpenFlags, kCreatMode)));
    }
    bool writeLog(const std::string & msg){return writeLog(msg.c_str(), msg.size());}
    bool writeLog(const char * msg, size_t len){
        if(NULL == msg || 0 == len)
            return true;
        if(!ensureFile())
            return false;
        for(size_t cur = 0;cur < len;){
            const size_t left = len - cur;
            const ssize_t ret = file_.write(msg + cur, left);
            if(ret < 0){
                if(EINTR == errno)
                    continue;
                return false;
            }
            cur += ret;
        }
        return true;
    }
    void rollFile(){
        if(0 == maxNumber_){
            file_.unlink();
            file_.close();
        }else{
            assert(!fname_.empty());
            CToString oss;
            oss<<fname_<<'.'<<maxNumber_;
            std::string newfile = oss.str();
            for(int i = maxNumber_ - 1;i > 0;--i){
                oss.str(fname_);
                oss<<'.'<<i;
                std::string oldfile = oss.str();
                CFile::Rename(oldfile, newfile);
                newfile = oldfile;
            }
            file_.rename(newfile);
            file_.close();
        }
    }
    static void InitSignalStack(){
        __Myt & inst = Instance();
        if(NULL == inst.stack_)
            inst.stack_ = tools::New<__SignalStack>();
    }
    static const char * SigCodeName(int sig, int code){
        if(SIGSEGV == sig){
            switch(code){
                case SEGV_MAPERR:return "address not mapped to object";
                case SEGV_ACCERR:return "invalid permissions for mapped object";
                default:;
            }
        }else if(SIGBUS == sig){
            switch(code){
                case BUS_ADRALN:return "invalid address alignment";
                case BUS_ADRERR:return "nonexistent physical address";
                case BUS_OBJERR:return "object-specific hardware error";
                default:;
            }
        }else if(SIGFPE == sig){
            switch(code){
                case FPE_INTDIV:return "integer divide by zero";
                case FPE_INTOVF:return "integer overflow";
                case FPE_FLTDIV:return "floating-point divide by zero";
                case FPE_FLTOVF:return "floating-point overflow";
                case FPE_FLTUND:return "floating-point underflow";
                case FPE_FLTRES:return "floating-point inexact result";
                case FPE_FLTINV:return "floating-point invalid operation";
                case FPE_FLTSUB:return "subscript out of range";
                default:;
            }
        }else if(SIGILL == sig){
            switch(code){
                case ILL_ILLOPC:return "illegal opcode";
                case ILL_ILLOPN:return "illegal operand";
                case ILL_ILLADR:return "illegal addressing mode";
                case ILL_ILLTRP:return "illegal trap";
                case ILL_PRVOPC:return "privileged opcode";
                case ILL_PRVREG:return "privileged register";
                case ILL_COPROC:return "coprocessor error";
                case ILL_BADSTK:return "internal stack error";
                default:;
            }
        }
        switch(code){
            case SI_USER:return "kill(2) or raise(3)";
            case SI_KERNEL:return "Sent by the kernel";
            case SI_QUEUE:return "sigqueue(2)";
            case SI_TIMER:return "POSIX timer expired";
            case SI_MESGQ:return "POSIX message queue state changed, see mq_notify(3)";
            case SI_ASYNCIO:return "AIO completed";
            case SI_SIGIO:return "queued SIGIO";
            case SI_TKILL:return "tkill(2) or tgkill(2)";
            default:;
        }
        return "unknown";
    }
    static void SignalHandler(int sig, siginfo_t * info, void * context){
        //Following routines can only use 'safe functions' defined in signal(7)
        typedef CAtomicSync<pthread_t> __AtomicEnter;
        //Instance() will not invoke constructor,
        //since this is not the first call (which may be init() or registerCrashHandler())
        __Myt & inst = Instance();
        CSignalParams & params = inst.params_;
        {   //check and lock
            //If more than 1 threads entered, only 1 can go on
            pthread_t entered = 0;
            if(!__AtomicEnter::compare_swap(&params.entered_, 0, pthread_self(), &entered))
                if(pthread_self() != entered)
                    for(;;::pause());
        //}{  //stop other threads
        //    //mask SIGSTOP for current thread
        //    sigset_t set;
        //    ::sigemptyset(&set);
        //    ::sigaddset(&set, SIGSTOP);
        //    ::pthread_sigmask(SIG_BLOCK, &set, NULL);
        //    //send stop signal
        //    ::kill(::getpid(), SIGSTOP);
        }
        int cnt = 0;    //valid address count in signal handler's stack in DIY backtrace
        //save backtrace
        //Any function call could change stack or invoke ::__cyg_profile_func_enter(),
        //so save backtrace as soon as possible.
        //If this is first time entering SignalHandler(), we use ::backtrace() to save effort.
        if(0 == params.again_++){
            //save original signal info
            if(NULL != info)
                memcpy(&params.info_, info, sizeof params.info_);
            params.sig_ = sig;
#ifdef __SAVE_STACK
#   error "__SAVE_STACK is deprecated"
            //if(!__getStack(&params.stack_))
#endif
                params.stack_.top_ = ::backtrace(params.stack_.addr_, ARRAY_SIZE(params.stack_.addr_));
                //If ::backtrace() raises SEGV, we'll re-enter SignalHandler() with again_ > 0
        }
        //If following routines crashed, we'll end up with a loop, so give a limitation
        if(params.again_ > 5)
            goto __end;
        else if(params.again_ > 1){
            //If re-entering SignalHandler(), we should not trust ::backtrace() any more,
            //so DIY backtrace
            struct Frame{   //call frame layout
                void * next_;
                void * sp_;
                void * func_;
            };
            union Ptr{  //save type casts
                uintptr_t v_;
                void * p_;
                void ** pp_;
                Frame * f_;
            };
            params.stack_.top_ = 0;
            if(NULL != inst.stack_ && params.sym_.getStackRange()){    //get normal stack range
                ::stack_t st; //SignalHandler() use an alternate stack
                st.ss_sp = inst.stack_->ref().buf_;
                st.ss_size = sizeof inst.stack_->ref().buf_;
                Ptr s, e;
                e.p_ = st.ss_sp;
                //search for code address in SignalHandler()'s stack
                for(s.pp_ = &context;e.v_ <= s.v_ && s.v_ + sizeof(Frame) < e.v_ + st.ss_size;++s.pp_)
                    if(params.sym_.isCode(s.f_->func_))
                        params.stack_.addr_[params.stack_.top_++] = s.f_->func_;
                cnt = params.stack_.top_;
                //search for call frame in normal stack
                s.v_ = params.sym_.stackTop();
                for(;params.sym_.isInStack(s.p_, sizeof(Frame));++s.pp_)
                    if(params.sym_.isInStack(s.f_->next_) && params.sym_.isCode(s.f_->func_))
                        params.stack_.addr_[params.stack_.top_++] = s.f_->func_;
            }
        }{  //dump signal info
            char * p = params.msg_;
            int len = sizeof params.msg_;
            time_t now = ::time(NULL);
            SAFE_PRINT_L(p, len, "\n\nProcess %s(PID %d / TID %lu) crashed", inst.exe_.c_str(), ::getpid(), (unsigned long)(::pthread_self()));
            SAFE_PRINT_L(p, len, "Time: %ld (date +\"%%Y-%%m-%%d %%H:%%M:%%S\" -d@%ld)", long(now), long(now));
            SAFE_PRINT_L(p, len, "Signal number: %d(%s)", params.sig_, strsignal(params.sig_));
            SAFE_PRINT_L(p, len, "Signal code: %d(%s)", params.info_.si_code, SigCodeName(params.sig_, params.info_.si_code));
            params.sym_.symbolize(params.info_.si_addr);
            SAFE_PRINT(p, len, "Memory location which caused fault: ");
            if(params.sym_.isInDynamic()){
                //dynamic link symbol format:
                //"base+offset (name from file)"
                SAFE_PRINT(p, len, " 0x%llx", (long long)params.sym_.mapOffset());
                uint64_t pc = reinterpret_cast<uintptr_t>(params.info_.si_addr);
                if(pc >= params.sym_.mapOffset()){
                    SAFE_PRINT(p, len, "+0x%llx", (long long)(pc - params.sym_.mapOffset()));
                }else{
                    SAFE_PRINT(p, len, "-0x%llx", (long long)(params.sym_.mapOffset() - pc));
                }
            }else{
                //normal symbol format:
                //"address (name from file)"
                SAFE_PRINT(p, len, " %p", params.info_.si_addr);
            }
            const char * const sname = params.sym_.symbolName();
            const char * const fname = params.sym_.fileName();
            if(NULL != sname && '\0' != *sname){
                SAFE_PRINT(p, len, " (%s from %s)", sname, fname);
            }else if(NULL != fname && '\0' != *fname){
                SAFE_PRINT(p, len, " (from %s)", fname);
            }
            SAFE_PRINT_L(p, len, "\n\nMemory maps:");
            inst.writeLog(params.msg_, sizeof params.msg_ - len);
        }{  //dump process maps
            CFile maps("/proc/self/maps", O_RDONLY);
            if(maps.valid()){
                char * const p = params.msg_;
                const size_t len = sizeof params.msg_;
                for(;;){
                    ssize_t ret = maps.read(p, len);
                    if(ret < 0){
                        if(EINTR == errno)
                            continue;
                        break;
                    }
                    if(ret > 0)
                        inst.writeLog(p, ret);
                    if(size_t(ret) < len)
                        break;
                }
                inst.writeLog("\n", 1);
            }
        }{  //dump backtrace
            {   //header
                char * p = params.msg_;
                int len = sizeof params.msg_;
                SAFE_PRINT_L(p, len, "Backtrace: (addr2line ADDR -e FILE)");
                if(params.again_ > 1){
                    SAFE_PRINT_L(p, len, " (stack frames is corrupted, the following content is just speculation)");
                }
                inst.writeLog(params.msg_, sizeof params.msg_ - len);
            }
            if(params.stack_.top_ > 0){
                //if DIY backtrace, from 0; otherwise from 1(skip SignalHandler())
                const size_t from = (params.again_ > 1 ? 0 : 1);
                for(int i = 0;from + i < params.stack_.top_;++i){
                    void * const addr = params.stack_.addr_[from + i];
                    params.sym_.symbolize(addr);
                    char * p = params.msg_;
                    int len = sizeof params.msg_;
                    SAFE_PRINT(p, len, "#%d", i);
                    if(params.sym_.isInDynamic()){
                        //dynamic link symbol format:
                        //"#level [base+offset] name from file"
                        SAFE_PRINT(p, len, " [0x%llx", (long long)params.sym_.mapOffset());
                        uint64_t pc = reinterpret_cast<uintptr_t>(addr);
                        if(pc >= params.sym_.mapOffset()){
                            SAFE_PRINT(p, len, "+0x%llx]", (long long)(pc - params.sym_.mapOffset()));
                        }else{
                            SAFE_PRINT(p, len, "-0x%llx]", (long long)(params.sym_.mapOffset() - pc));
                        }
                    }else{
                        //normal symbol format:
                        //"#level [address] name from file"
                        SAFE_PRINT(p, len, " [%p]", addr);
                    }
                    const char * const sname = params.sym_.symbolName();
                    const char * const fname = params.sym_.fileName();
                    if(NULL != sname && '\0' != *sname){
                        SAFE_PRINT(p, len, " %s", sname);
                    }
                    if(NULL != fname && '\0' != *fname){
                        SAFE_PRINT(p, len, " from %s", fname);
                    }
                    if(params.again_ > 1 && int(from + i) < cnt)
                        SAFE_PRINT_L(p, len, ".");
                    else
                        SAFE_PRINT_L(p, len, "");
                    inst.writeLog(params.msg_, sizeof params.msg_ - len);
                }
            }
        }
        inst.writeLog("\n", 1);
__end:
        {
            //restore signal handlers
            //After dumping all message, we can restore signal handlers
            struct sigaction act;
            ::sigemptyset(&act.sa_mask);
            act.sa_flags = 0;
            act.sa_handler = SIG_DFL;
            ::sigaction(SIGSEGV, &act, NULL);
            ::sigaction(SIGBUS, &act, NULL);
            ::sigaction(SIGFPE, &act, NULL);
            ::sigaction(SIGILL, &act, NULL);
            ::sigaction(SIGABRT, &act, NULL);
            //re-raise signal
            ::raise(params.sig_);
        }
    }
    //fields
    CFile file_;
    CMutex lock_;
    std::string fname_;
    std::string exe_;
    COnceGuard stackInit_;
    __SignalStack * stack_;
    size_t maxSize_;
    int maxNumber_;
    ELevel level_;
    CSignalParams params_;
};

class CLocalLogger
{
public:
    CLocalLogger(CGlobalLogger::ELevel level, const char * file, int line, const char * func){
        oss_<<'['<<tools::TimeStringUs(tools::GetTimeUs())<<']'
            <<'['<<pthread_self()<<']'
            <<'['<<CGlobalLogger::LevelName(level)<<']'
            <<'['<<tools::Basename(file)<<':'<<line<<']'
            <<'['<<func<<']';
    }
    CToString & oss(){return oss_;}
    void log() const{CGlobalLogger::Instance().log(oss_.str());}
    void print() const{std::cerr<<oss_.str()<<std::flush;}
private:
    CToString oss_;
};

NS_IMPL_END

#endif

