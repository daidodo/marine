#ifndef DOZERG_PFTEST_IMPL_STATS_GPERF_H_20130820
#define DOZERG_PFTEST_IMPL_STATS_GPERF_H_20130820

#include <gperftools/profiler.h>
#include "pftest_impl_stats.hh"

NS_IMPL_BEGIN

class CPfStats__GPERF : public CPfStats
{
public:
    virtual void init(){
        if(!::ProfilerStart("./1.pprof"))
            throw std::runtime_error("gperf start failed");
    }
    virtual void uninit(){
        ::ProfilerStop();
    }
    virtual std::string show() const{
        return std::string();
    }
};

NS_IMPL_END

#endif
