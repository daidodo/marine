#ifndef DOZERG_FREQ_CONTROL_H_20120224
#define DOZERG_FREQ_CONTROL_H_20120224

/*
    Ƶ�ʿ����࣬�̲߳���ȫ
        CFreqControl        �����ڸ�Ƶ���ƣ�����1 Hz
        CWideFreqControl    ����������Ƶ���������������1 Hz���
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
    //freq: Ƶ��(��/s)
    //bucketSz: ����Ͱ��С
    CFreqControl(size_t freq, size_t bucketSz)
        : freq_(0)
        , buckSz_(0)
        , token_(0)
        , delta_(0)
        , time_(0)
    {
        init(freq, bucketSz);
    }
    //��ʼ������Ͱ�������ظ���ʼ�����޸�Ƶ�ʺ�Ͱ��С
    void init(size_t freq, size_t bucketSz){
        if(!freq_){
            token_ = delta_ = 0;
            time_ = tools::MonoTimeUs();
        }
        freq_ = freq;
        buckSz_ = bucketSz;
    }
    //��������
    //nowUs: ��ǰ����ʱ��(tools::MonoTimeUs(), ΢��)
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
    //��������Ƿ��㹻
    //need: ��Ҫ��������Ŀ
    bool check(size_t need) const{return (token_ >= 0 && size_t(token_) >= need);}
    //��ȡ��ǰ������
    ssize_t token() const{return token_;}
    //�۳�����
    //���������������������п۳�������������false��
    bool get(size_t need = 1){
        if(token_ < 0 || size_t(token_) < need)
            return false;
        token_ -= need;
        return true;
    }
    //͸֧���ƣ����ܵ���token_Ϊ��
    //���need����(overflow)��������п۳�������������false
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
    uint64_t time_;      //�ϴ�generate��ʱ��(΢��)
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

