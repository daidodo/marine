#ifndef DOZERG_PFTEST_IMPL_STATS_H_20130820
#define DOZERG_PFTEST_IMPL_STATS_H_20130820

#include <string>
#include "environment.hh"

NS_IMPL_BEGIN

class CPfStats
{
public:
    virtual void init(){}
    virtual void uninit(){}
    virtual void start(){}
    virtual void stop(uint64_t cnt){}
    virtual std::string show() const{
        return "finish";
    }
    virtual std::string showThread(int i) const{
        return (i >= 0 ? std::string() : show());
    }
    void add(const CPfStats & a){}
    virtual ~CPfStats(){}
};

NS_IMPL_END

#endif

