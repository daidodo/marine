#ifndef DOZERG_TOOLS_SYSTEM_H_20130122
#define DOZERG_TOOLS_SYSTEM_H_20130122

/*
    系统工具函数
        GetProcessorCount
        GetPhysicalMemorySize
        GetPageSize
        ErrorMsg
        Daemon
        DaemonMonitor
        SetMaxFileDescriptor
        GetMaxFileDescriptor
        GetProcessMemoryUsage
        ProcessExclusion
        GetFilenameByFd
        THROW_IF_FAIL
*/

#include <sys/resource.h>   //setrlimit,getrlimit
#include <sys/sysinfo.h>    //sysinfo
#include <sys/param.h>      //NOFILE
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>         //signal
#include <stdint.h>
#include <unistd.h>
#include <cstring>          //strerror_r
#include <cstdlib>          //exit
#include <string>
#include "time.hh"
#include "../to_string.hh"
#include "../configuration.hh"
#include "../semaphore.hh"

NS_SERVER_BEGIN

namespace tools
{
    //获得机器处理器个数,返回-1表示获取失败
    inline int GetProcessorCount()
    {
        return (int)sysconf(_SC_NPROCESSORS_ONLN);
    }

    //获取机器物理内存大小(bytes)
    inline uint64_t GetPhysicalMemorySize()
    {
        struct sysinfo si;
        if(0 != sysinfo(&si))
            return 0;
        if(si.mem_unit > 0)
            return uint64_t(si.totalram) * si.mem_unit;
        return si.totalram;
    }

    //获取系统页表大小(Bytes)
    inline size_t GetPageSize()
    {
        return (size_t)sysconf(_SC_PAGESIZE);
    }

    //得到错误码error_no对应的系统错误信息
    inline std::string ErrorMsg(int error_no)
    {
        CToString os;
        os<<" errno "<<error_no<<" - ";
#if defined __USE_XOPEN2K || defined __USE_MISC
        std::string buf(256, 0);
        os<<strerror_r(error_no, &buf[0], buf.size());
#else
        os<<strerror(error_no);
#endif
        return os.str();
    }

    //程序进到后台运行
    inline void Daemon()
    {
#if _BSD_SOURCE || (_XOPEN_SOURCE && _XOPEN_SOURCE < 500)
        if(0 != daemon(0, 1))
            ::exit(1);
#else
        //fork child process
        if(::fork())
            ::exit(0);
        //creates  a new session
        if(::setsid() == -1)
            ::exit(1);
        if(::chdir("/") < 0)
            ::exit(1);
        ::umask(0);
#endif
        //ignore some signals
        ::signal(SIGALRM, SIG_IGN);
        ::signal(SIGINT,  SIG_IGN);
        ::signal(SIGHUP,  SIG_IGN);
        ::signal(SIGQUIT, SIG_IGN);
        ::signal(SIGPIPE, SIG_IGN);
        ::signal(SIGTTOU, SIG_IGN);
        ::signal(SIGTTIN, SIG_IGN);
        ::signal(SIGCHLD, SIG_IGN);
    }

    //程序进到后台运行，并实现秒级重启（仅异常退出时）
    //注意：
    //1. 所有全局变量，文件offset等，在程序重启后都会恢复到当前值
    //2. 1分钟内最多重启5次
    inline void DaemonMonitor()
    {
        Daemon();
        const int kRestart = 6;     //times
        const int kTimeout = 60;    //s
        time_t times[kRestart];
        int index = 0;
        //prepare
        for(int i = 0;i < int(ARRAY_SIZE(times));++i)
            times[i] = 1;
        for(;;::sleep(1)){
            //check freq
            const int cur = index++;
            if(index >= int(ARRAY_SIZE(times)))
                index = 0;
            if(!tools::IsTimeout(times[index], kTimeout, 0, 0))
                ::exit(1);
            //fork
            const pid_t pid = ::fork();
            if(-1 == pid){
                ::exit(1);
            }else if(0 == pid) //child process
                break;
            //record
            times[cur] = tools::Time(NULL);
            //monitor process
            ::signal(SIGTERM, SIG_DFL);
            ::signal(SIGCHLD, SIG_DFL);
            int status = 0;
            while(-1 == ::waitpid(pid, &status, 0))
                if(ECHILD == errno)
                    break;
            if(0 == status){
                ::exit(0);
            }else if(WIFEXITED(status)){
                ::exit(WEXITSTATUS(status));
            }else if(WIFSIGNALED(status)){
                const int sig = WTERMSIG(status);
                switch(sig){
                    case SIGSEGV:
                    case SIGABRT:
                    case SIGFPE:
                    case SIGBUS:
                    case SIGILL:break;
                    default: ::exit(0);  //::raise(sig) again?
                }
            }
        }
    }

    //设置进程允许打开的最大文件数
    inline bool SetMaxFileDescriptor(uint32_t numfiles)
    {
        struct rlimit rt;
        rt.rlim_max = rt.rlim_cur = numfiles;
        return (0 == setrlimit(RLIMIT_NOFILE, &rt));
    }

    //获得进程允许打开的最大文件数,返回0表示获取失败
    inline uint32_t GetMaxFileDescriptor()
    {
        struct rlimit rt;
        if(0 != getrlimit(RLIMIT_NOFILE, &rt))
           return 0;
        if(RLIM_INFINITY == rt.rlim_cur)
            return uint32_t(-1);
        return rt.rlim_cur;
    }

    //获取进程的内存使用量
    //vm: 非NULL时返回虚拟内存使用量(Bytes)
    //res: 非NULL时返回物理内存使用量(Bytes)
    //return:
    //  true    成功
    //  false   失败
    inline bool GetProcessMemoryUsage(size_t * vm, size_t * res)
    {
        if(!vm && !res)
            return true;
        CConfiguration config;
        if(!config.load("/proc/self/status", CConfiguration::kFormatColon))
            return false;
        if(vm)
            *vm = config.getInt<size_t>("VmSize");
        if(res)
            *res = config.getInt<size_t>("VmRSS");
        return true;
    }

    //检查进程是否重复启动
    //key: 标识进程的key
    //pathname: 标识进程的文件路径
    //proj_id: project id, non-zero
    //timeMs: 最多等待时间(毫秒)
    //return:
    //  false   进程重复启动
    //  true    进程不是重复启动
    inline bool ProcessExclusion(key_t key, uint32_t timeMs)
    {
        const int kFlags = S_IRUSR | S_IWUSR;
        CXsiSemaphoreSet sem(key, 0, kFlags);
        if(!sem.valid()){
            if(!sem.init(key, 1, IPC_CREAT | kFlags))
                return false;
            sem.setVal(0, 1);
        }
        return sem.timeApply(0, -1, true, true, timeMs);
    }

    inline bool ProcessExclusion(const char * pathname, int proj_id, uint32_t timeMs)
    {
        key_t key = ::ftok(pathname, proj_id);
        if(-1 == key)
            return false;
        return ProcessExclusion(key, timeMs);
    }

    inline bool ProcessExclusion(const char * pathname, uint32_t timeMs)
    {
        return ProcessExclusion(pathname, 1, timeMs);
    }

#ifdef __HAS_READLINK
    //通过fd得到打开的文件名
    //return:
    //  空字符串    失败
    //  其他        文件名
    inline std::string GetFilenameByFd(int fd)
    {
        if(fd < 0)
            return std::string();
        CToString oss;
        oss<<"/proc/self/fd/"<<fd;
        std::string path(1024, 0);
        const ssize_t n = ::readlink(oss.str().c_str(), &path[0], path.size());
        if(n <= 0)
            return std::string();
        path.resize(n);
        return path;
    }
#endif

    //如果ret不为0，则抛出exception异常
#define THROW_IF_FAIL(exception, ret)  do{ \
    int err = ret; \
    if(0 != err)    \
    throw exception(tools::ErrorMsg(err).c_str());  \
}while(0)

    //如果ret不为0，则抛出std::runtime_error异常
#define THROW_RT_IF_FAIL(ret)  THROW_IF_FAIL(std::runtime_error, ret)

}//namespace tools

NS_SERVER_END

#endif

