#ifndef DOZERG_FREQ_CONTROL_H_20120224
#define DOZERG_FREQ_CONTROL_H_20120224

/*
    频率控制类，线程不安全
        CFreqControl        适用于高频控制，大于1 Hz
        CWideFreqControl    适用于所有频率情况，包括低于1 Hz情况
//*/

#include "tools/time.hh"     //MonoTimeUs

NS_SERVER_BEGIN

struct CFreqControl
{
    CFreqControl()
        : freq_(0)
        , buckSz_(0)
        , token_(0)
        , delta_(0)
        , time_(0)
    {}
    //freq: 频率(次/s)
    //bucketSz: 令牌桶大小
    CFreqControl(size_t freq, size_t bucketSz)
        : freq_(0)
        , buckSz_(0)
        , token_(0)
        , delta_(0)
        , time_(0)
    {
        init(freq, bucketSz);
    }
    //初始化令牌桶，可以重复初始化，修改频率和桶大小
    void init(size_t freq, size_t bucketSz){
        if(!freq_){
            token_ = delta_ = 0;
            time_ = tools::MonoTimeUs();
        }
        freq_ = freq;
        buckSz_ = bucketSz;
    }
    //生产令牌
    //nowUs: 当前机器时间(tools::MonoTimeUs(), 微秒)
    void generate(uint64_t nowUs){
        if(nowUs > time_){
            delta_ += freq_ * (nowUs - time_);
            if(delta_ < 0){
                delta_ = 0;     //overflow
                token_ = buckSz_;
            }else{
                ssize_t tok = delta_ / 1000000 + token_;
                delta_ %= 1000000;
                if(tok < token_ || tok > ssize_t(buckSz_))
                    tok = buckSz_;  //overflow
                token_ = tok;
            }
        }
        time_ = nowUs;
    }
    void generate(){return generate(tools::MonoTimeUs());}
    //检查令牌是否足够
    //need: 需要的令牌数目
    bool check(size_t need) const{return (token_ >= 0 && size_t(token_) >= need);}
    //获取当前令牌数
    ssize_t token() const{return token_;}
    //扣除令牌
    //如果令牌数不够，不会进行扣除操作，并返回false，
    bool get(size_t need = 1){
        if(token_ < 0 || size_t(token_) < need)
            return false;
        token_ -= need;
        return true;
    }
    //透支令牌，可能导致token_为负
    //如果need过大(overflow)，不会进行扣除操作，并返回false
    bool overdraft(size_t need){
        if(ssize_t(need) < 0)
            return false;
        token_ -= need;
        return true;
    }
private:
    size_t freq_;
    size_t buckSz_;
    ssize_t token_;
    ssize_t delta_;
    uint64_t time_;      //上次generate的时间(微秒)
};

struct CWideFreqControl
{
    explicit CWideFreqControl(double freq = 0){
        init(freq);
    }
    void init(double freq){
        time_ = tools::MonoTimeUs();
        token_ = 0;
        freq_ = freq;
    }
    bool get(){
        if(getAux())
            return true;
        uint64_t now = tools::MonoTimeUs();
        if(time_ < now)
            token_ += freq_ * (now - time_) / 1000000;
        if(token_ > freq_ + 1)
            token_ = freq_ + 1;
        time_ = now;
        return getAux();
    }
private:
    bool getAux(){
        if(token_ < 1)
            return false;
        --token_;
        return true;
    }
    uint64_t time_;
    double token_;
    double freq_;
};

NS_SERVER_END

#endif

