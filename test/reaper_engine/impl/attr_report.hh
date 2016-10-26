#ifndef DOZERG_ATTR_REPORT_H_20130606
#define DOZERG_ATTR_REPORT_H_20130606

#include <threads.hh>
#include "sock_session.hh"

NS_HSHA_IMPL_BEGIN

class CHshaEngineImpl;

class CAttrReporter : public marine::CThreadPool
{
    typedef marine::CThreadPool __MyBase;
public:
    explicit CAttrReporter(CHshaEngineImpl & engine);
    bool init(size_t capacity, int interval);
private:
    virtual void run();
    //fields
    CHshaEngineImpl & engine_;
    int int_;
};

NS_HSHA_IMPL_END

#endif

