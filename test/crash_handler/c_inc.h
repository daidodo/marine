#include <marine/threads.hh>

#ifndef THREAD_COUNT
#define THREAD_COUNT    1
#endif

#ifdef __USE_GLOG
#   include <glog/logging.h>
#   define CRASH_HANDLER    google::InstallFailureSignalHandler
#else
#   include <marine/logger.hh>
#   define CRASH_HANDLER    LOGGER_CRASH_HANDLER
#endif

#include "my.h"

using namespace marine;
using namespace std;

static void init(const char * exe)
{
#ifdef __USE_GLOG
    google::InitGoogleLogging(exe);
#else
    std::string fname = exe;
    fname += ".log";
    LOGGER_INIT(fname, "TRACE", 1 << 20, 0);
#endif
}

static void * TestProc(void * arg);
static void * TestProc2(void * arg);
static void Test();

int main(int argc, const char ** argv)
{
    init(argv[0]);
    std::vector<CThread> threads(THREAD_COUNT);
    std::vector<CThread> threads2(3);
    for(size_t i = 0;i < threads.size();++i)
        threads[i].start(TestProc);
    for(size_t i = 0;i < threads2.size();++i)
        threads2[i].start(TestProc2);

    for(size_t i = 0;i < threads.size();++i)
        threads[i].join();
    for(size_t i = 0;i < threads2.size();++i)
        threads2[i].join();
    return 0;
}

static void * TestProc(void * arg)
{
    CRASH_HANDLER();
    sleep(1);
    Test();
    return NULL;
}

static void * TestProc2(void * arg)
{
    CRASH_HANDLER();
    for(int x = 1;;++x){
        INFO("x="<<x);
        usleep(100000);
    }
    return NULL;
}
