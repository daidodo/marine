#include <unistd.h>     //sleep
#include <logger.hh>
#include <attr_stats.hh>
#include "engine_impl.hh"
#include "attr_report.hh"

using namespace marine;

NS_HSHA_IMPL_BEGIN

CAttrReporter::CAttrReporter(CHshaEngineImpl & engine)
    : engine_(engine)
    , int_(60)
{}

bool CAttrReporter::init(size_t capacity, int interval)
{
    int_ = (interval > 0 ? interval : 1);
    if(capacity < 1000)
        capacity = 1000;
    return ATTR_INIT(capacity);
}

struct __AttrOp
{
    explicit __AttrOp(CHshaEngineImpl & e):e_(e){}
    void operator ()(int attr, uint64_t val) const{
        e_.reportAttr(attr, val);
    }
    private:
    CHshaEngineImpl & e_;
};

void CAttrReporter::run()
{
    LOGGER_CRASH_HANDLER();
    for(time_t old = tools::Time(NULL);;){
        ::usleep(500000);
        time_t cur = tools::Time(NULL);
        if(tools::IsTimeout(old, int_, cur)){
            engine_.onAttrReport();
            ATTR_ITERATE(__AttrOp(engine_));
            old = cur;
        }
    }
}

NS_HSHA_IMPL_END
