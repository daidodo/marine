#ifndef DOZERG_PFTEST_IMPL_H_20130619
#define DOZERG_PFTEST_IMPL_H_20130619

#include <vector>
#include <iostream>
#include <sstream>
#include "../threads.hh"
#include "../tools/time.hh"
#include "../tools/memory.hh"
#include "../tools/string.hh"

#ifdef __TIME

#   include "pftest_impl_stats_time.hh"
#   define __PfStats CPfStats__TIME
//#   define __PFASSERT(expr) __PFASSERT_IMP(true)

#elif defined(__GPROF)

#   include "pftest_impl_stats.hh"
#   define __PfStats CPfStats

#elif defined(__GPERF)

#   include "pftest_impl_stats_gperf.hh"
#   define __PfStats CPfStats__GPERF

#elif defined(__PERF)

#   include "pftest_impl_stats.hh"
#   define __PfStats CPfStats

#endif

#define __DOZERG_TEST   0

NS_IMPL_BEGIN

struct CPfInit
{
    virtual const char * pf_toString() const = 0;
    virtual const char * pf_testName() const = 0;
    virtual void pf_run() = 0;
    virtual ~CPfInit(){}
};

class CPfTest
{
    typedef CPfTest __Myt;
    typedef CPfInit __Init;
protected:
    typedef __PfStats __Stats;
private:
    struct __Params{
        __Myt * t_;
        __Stats st_;
        int i_;
        int c_;
    };
    enum __Op{kRegister, kRun, kRunAll};
    virtual bool pf_run(__Stats & st, int index, int count) const = 0;
    virtual const char * pf_toString() const = 0;
    virtual const char * pf_testName() const = 0;
protected:
    static void pf_RegisterTest(__Myt * t){
        OpTest(kRegister, t, NULL, 0, 0);
    }
    CPfTest():sec_(0){}
    virtual ~CPfTest(){}
    bool pf_debug() const{return __DOZERG_TEST;}
    void pf_seconds(int s){sec_ = s;}
    int pf_seconds() const{return sec_;}
public:
    static void pf_RegisterInit(__Init * t, bool init){
        OpInit(kRegister, init, t, NULL);
    }
    static void pf_RunTests(int seconds, int threads, const std::vector<const char *> & names){
        RunInit("***", true);
        if(names.empty()){
            RunAllTest(seconds, threads);
        }else{
            for(size_t i = 0;i < names.size();++i)
                RunTest(names[i], seconds, threads);
        }
        RunInit("***", false);
    }
    static void pf_printUsage(const char * exe){
        std::cerr
            <<"Usage:\n"
            <<"  "<<exe<<" -h  show help message\n"
            <<"  "<<exe<<" [+TIME] [++THREADS] [TESTS]]\n"
            <<"    TIME      run each test for TIME seconds, default 1\n"
            <<"    THREADS   run each test in THREADS threads concurrently, default 1\n"
            <<"    TESTS     only run TESTS, seperated by space, e.g. '.ctor .out'\n"
            ;
    }
private:
    static void RunInit(const char * name, bool init){
        return OpInit(kRun, init, NULL, name);
    }
    static void OpInit(__Op op, bool init, __Init * t, const char * name){
        typedef std::vector<__Init *> __Inits;
        static __Inits inits, uninits;
        if(kRegister == op){
            if(NULL == t)
                return;
            if(init)
                inits.push_back(t);
            else
                uninits.push_back(t);
        }else if(kRun == op){
            if(NULL == name)
                return;
            const __Inits & vec = (init ? inits : uninits);
            for(__Inits::const_iterator i = vec.begin();i != vec.end();++i){
                __Init * const t = *i;
                if(0 == ::strcmp(name, t->pf_toString())
                        || 0 == ::strcmp(name, t->pf_testName()))
                    t->pf_run();
            }
        }
    }
    static void RunAllTest(int seconds, int threads){
        return OpTest(kRunAll, NULL, NULL, seconds, threads);
    }
    static void RunTest(const char * name, int seconds, int threads){
        return OpTest(kRun, NULL, name, seconds, threads);
    }
    static void OpTest(__Op op, __Myt * t, const char * name, int seconds, int threads){
        typedef std::vector<__Myt *> __Tests;
        static __Tests tests;
        if(kRegister == op){
            if(NULL != t)
                tests.push_back(t);
        }else if(kRun == op){
            if(NULL == name)
                return;
            bool found = false;
            for(__Tests::iterator j = tests.begin();j != tests.end();++j){
                t = *j;
                assert(t);
                if(0 != ::strcmp(name, t->pf_toString())
                        && 0 != ::strcmp(name, t->pf_testName()))
                    continue;
                found = true;
                ThreadsTest(t, seconds, threads);
                break;
            }
            if(!found){
                std::cerr<<"Error: test '"<<name<<"' is not found!\n";
            }
        }else if(kRunAll == op){
            for(__Tests::iterator j = tests.begin();j != tests.end();++j)
                ThreadsTest(*j, seconds, threads);
        }
    }
    static void ThreadsTest(__Myt * t, int seconds, int threads){
        assert(t);
        std::cout<<t->pf_toString()<<": "<<std::flush;
        t->pf_seconds(seconds);
        RunInit(t->pf_toString(), true);
        if(threads > 1){
            //prepare
            std::vector<__Params> params(threads);
            std::vector<CThread> ths(threads);
            for(int i = 0;i < threads;++i){
                params[i].t_ = t;
                params[i].i_ = i;
                params[i].c_ = threads;
            }
            //run
            __Stats sum;
            sum.init();
            for(int i = 0;i < threads;++i)
                ths[i].start(ThreadTest, &params[i]);
            for(int i = 0;i < threads;++i)
                ths[i].join();
            sum.uninit();
            //show
            for(int i = 0;i < threads;++i){
                std::cout<<params[i].st_.showThread(i);
                sum.add(params[i].st_);
            }
            std::cout<<sum.showThread(-1)<<"\n";
        }else{
            __Params params;
            params.t_ = t;
            params.i_ = 0;
            params.c_ = 1;
            params.st_.init();
            ThreadTest(&params);
            params.st_.uninit();
            std::cout<<params.st_.show()<<"\n";
        }
        RunInit(t->pf_toString(), false);
    }
    static void * ThreadTest(void * arg){
        assert(arg);
        __Params & p = *static_cast<__Params *>(arg);
        assert(p.t_);
        p.t_->pf_run(p.st_, p.i_, p.c_);
        return NULL;
    }
    //fields
    int sec_;
};

class CPfMessage
{
    typedef CPfMessage __Myt;
public:
    CPfMessage(const char * fname, int line){
        std::cerr<<'\n'<<fname<<':'<<line<<": Failure\n";
    }
    template<class T>
    const __Myt & operator <<(const T & v) const{
        std::cerr<<v;
        return *this;
    }
};

class CPfFail
{
    typedef CPfFail __Myt;
public:
    CPfFail(const CPfMessage &){
        std::cerr<<'\n';
        throw 1;
    }
};

NS_IMPL_END

int main(int argc, const char ** argv)
{
    //parse args
    int seconds = 0;
    int threads = 0;
    std::vector<const char *> names;
    for(int i = 1;i < argc;++i){
        const char * ret = NULL;
        if(NS_SERVER::tools::ExtractArg(argv[i], "-h", NULL)){
            NS_SERVER::NS_IMPL::CPfTest::pf_printUsage(argv[0]);
            return 1;
        }else if(NS_SERVER::tools::ExtractArg(argv[i], "++", &ret)){
            threads = ::atoi(ret);
        }else if(NS_SERVER::tools::ExtractArg(argv[i], "+", &ret)){
            seconds = ::atoi(ret);
        }else if(NS_SERVER::tools::ExtractArg(argv[i], ".", &ret)){
            names.push_back(ret);
        }
    }
    //check
    if(seconds < 1)
        seconds = 1;
    if(threads < 1)
        threads = 1;
    //run
    NS_SERVER::NS_IMPL::CPfTest::pf_RunTests(seconds, threads, names);
    return 0;
}

#define __PF_GLOBAL_INIT(init)  \
class __CPfInit_##init : public NS_SERVER::NS_IMPL::CPfInit{    \
    typedef __CPfInit_##init __Myt; \
    __CPfInit_##init(const __Myt &); \
    __Myt & operator =(const __Myt &);  \
public:__CPfInit_##init(){NS_SERVER::NS_IMPL::CPfTest::pf_RegisterInit(this, init);} \
    virtual void pf_run(); \
    virtual const char * pf_toString() const{return "***";}    \
    virtual const char * pf_testName() const{return pf_toString();}   \
}__g_CPfInit_##init;    \
void __CPfInit_##init::pf_run()


#define __PF_TEST_INIT(case_name, test_name, init)    \
class __CPfInit_##case_name##_##test_name##_##init : public NS_SERVER::NS_IMPL::CPfInit{    \
    typedef __CPfInit_##case_name##_##test_name##_##init __Myt; \
    __CPfInit_##case_name##_##test_name##_##init(const __Myt &); \
    __Myt & operator =(const __Myt &);  \
public:__CPfInit_##case_name##_##test_name##_##init(){NS_SERVER::NS_IMPL::CPfTest::pf_RegisterInit(this, init);} \
    virtual void pf_run(); \
    virtual const char * pf_toString() const{return #case_name "." #test_name;}  \
    virtual const char * pf_testName() const{return #test_name;}   \
}__g_CPfInit_##case_name##_##test_name##_##init;    \
void __CPfInit_##case_name##_##test_name##_##init::pf_run()


#define __PFTEST(case_name, test_name)    \
class __CPfTest_##case_name##_##test_name : public NS_SERVER::NS_IMPL::CPfTest{   \
    typedef __CPfTest_##case_name##_##test_name __Myt;  \
    typedef NS_SERVER::NS_IMPL::CPfTest __MyBase;     \
    inline void pf_inner_test(int, int, bool &) const __ALWAYS_INLINE;    \
    virtual const char * pf_toString() const{return #case_name "." #test_name;}  \
    virtual const char * pf_testName() const{return #test_name;}   \
    virtual bool pf_run(__Stats & st, int tid, int tnum) const{  \
        const int sec = pf_seconds();  \
        if(sec <= 0)    \
            return false;   \
        const int64_t total = int64_t(sec) * 1000000;   \
        bool calc = true, finish = false;   \
        uint64_t cnt = 1, all = 0;  \
        st.start();  \
        while(cnt > 0){ \
            uint64_t cur = 0;   \
            if(calc)    \
                cur = marine::tools::MonoTimeUs();  \
            if(pf_debug()) \
                std::cout<<"\ncnt="<<cnt;   \
            uint64_t i = 0; \
            for(;i < cnt && !finish;++i) \
                pf_inner_test(tid, tnum, finish); \
            all += i; \
            if(!calc || finish)   \
                break;  \
            int64_t use = marine::tools::MonoTimeUs(&cur);  \
            if(use > total){    \
                if(pf_debug()) \
                    std::cout<<" use="<<use;    \
                break;  \
            }else if(use > 100000){ \
                cnt = uint64_t(double(sec) * 1000000 * cnt / use);  \
                calc = false;   \
                if(pf_debug()) \
                    std::cout<<" real cnt="<<cnt;   \
            }else   \
                cnt <<= 4;  \
        }   \
        st.stop(all);  \
        return true;    \
    }   \
    __CPfTest_##case_name##_##test_name(const __Myt &); \
    __Myt & operator =(const __Myt &);  \
public:__CPfTest_##case_name##_##test_name(){__MyBase::pf_RegisterTest(this);} \
}__g_##case_name##_##test_name; \
void __CPfTest_##case_name##_##test_name::pf_inner_test(int pf_thread_index, int pf_thread_count, bool & pf_thread_finish) const

#define __PFASSERT_IMP(expr)    if(!(expr)) __UNUSED NS_SERVER::NS_IMPL::CPfFail f = NS_SERVER::NS_IMPL::CPfMessage(__FILE__, __LINE__)

#ifndef __PFASSERT
#   define __PFASSERT(expr)    __PFASSERT_IMP(expr)
#endif

#endif

