#ifndef DOZERG_PFTEST_IMPL_STATS_TIME_H_20130820
#define DOZERG_PFTEST_IMPL_STATS_TIME_H_20130820

#include <sstream>
#include "pftest_impl_stats.hh"

NS_IMPL_BEGIN

class CThousandSep : public std::numpunct<char>
{
    typedef std::numpunct<char> __MyBase;
public:
    explicit CThousandSep(size_t r = 0)
       : __MyBase(r)
    {}
protected:
    std::string do_grouping() const{return "\003";}
};

class CPfStats__TIME : public CPfStats
{
public:
    CPfStats__TIME():time_(0),cnt_(0){}
    virtual void start(){
        time_ = tools::MonoTimeUs();
        cnt_ = 0;
    }
    virtual void stop(uint64_t cnt){
        time_ = tools::MonoTimeUs(&time_);
        cnt_ = cnt;
    }
    void add(const CPfStats__TIME & a){
        if(a.time_ > time_)
            time_ = a.time_;
        cnt_ += a.cnt_;
    }
    virtual std::string show() const{
        std::ostringstream oss;
        if(cnt_ > 0){
            oss<<timestr(double(time_) / cnt_)<<"per call";
            if(time_ > 0){
                const double t = double(cnt_) * 1000000 / time_;
                oss<<std::fixed;
                CThousandSep ts(1);
                oss.imbue(std::locale(std::locale(), &ts));
                if(99 < t && t < uint64_t(-1)){
                    uint64_t tt = static_cast<uint64_t>(t);
                    oss<<" ("<<tt<<" calls per second)";
                }else
                    oss<<" ("<<t<<" calls per second)";
            }
        }
        return oss.str();
    }
    virtual std::string showThread(int i) const{
        std::ostringstream oss;
        if(i >= 0){
            oss<<"\n  thread "<<i<<": "<<show();
        }else
            oss<<"\n  Total:    "<<show();
        return oss.str();
    }
private:
    static std::string timestr(double us){
        std::ostringstream oss;
        const double S = 1000000.0;
        const double M = S * 60;
        const double H = M * 60;
        const double D = H * 24;
        const bool gt = (us > S);
        if(us > D){
            int d = static_cast<int>(us / D);
            oss<<d<<" d ";
            us -= d * D;
        }
        if(us > H){
            int h = static_cast<int>(us / H);
            oss<<h<<" h ";
            us -= h * H;
        }
        if(us > M){
            int m = static_cast<int>(us / M);
            oss<<m<<" m ";
            us -= m * M;
        }
        if(gt){
            oss<<(us / S)<<" s ";
            return oss.str();
        }else if(us > 1000.){
            oss<<(us / 1000.)<<" ms ";
            return oss.str();
        }else if(us > 1.){
            oss<<us<<" us ";
            return oss.str();
        }else
            oss<<(us / 1000)<<" ns ";
        return oss.str();
    }
    //fields
    uint64_t time_;
    uint64_t cnt_;
};

NS_IMPL_END

#endif

