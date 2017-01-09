/*
 * Copyright (c) 2016 Zhao DAI <daidodo@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or any
 * later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see accompanying file LICENSE.txt
 * or <http://www.gnu.org/licenses/>.
 */

/**
 * @file
 * @brief Tools for rate limiting, bandwidth control and burstiness suppression.
 * @author Zhao DAI
 */

#ifndef DOZERG_FREQ_CONTROL_H_20120224
#define DOZERG_FREQ_CONTROL_H_20120224

/*
    频率控制类，线程不安全
        CFreqControl        适用于高频控制，大于1 Hz
        CWideFreqControl    适用于所有频率情况，包括低于1 Hz情况
//*/

#include "tools/time.hh"     //MonoTimeUs

NS_SERVER_BEGIN

/**
 * @brief Rate limiting for high frequency (>1Hz) jobs.
 * CFreqControl is a convenient tool for rate limiting, based on [Token Bucket]
 * (https://en.wikipedia.org/wiki/Token_bucket) algorithm.
 * @n Given a frequency @c F, CFreqControl generates @c F tokens to bucket per second. Each token
 * represents a job. When there is no token in the bucket, no new job should be created.
 * @n The size of bucket is important for burstiness suppression. It denotes the maximum number of
 * jobs created at once. In case of network transmission control for example, a proper bucket size
 * leads to smooth traffic flow in despite of request bursts.
 */
struct CFreqControl
{
    /**
     * @brief Default constructor.
     * You need to call @ref init before you can use this object.
     */
    CFreqControl()
        : freq_(0)
        , buckSz_(0)
        , token_(0)
        , delta_(0)
        , time_(0)
    {}
    /**
     * @{
     * @brief Initialize this object.
     * @param freq A positive integer denoting frequency
     * @param bucketSz Max number of tokens the bucket can hold
     * @note You can @ref init this object again to change frequency and bucket size.
     */
    CFreqControl(size_t freq, size_t bucketSz)
        : freq_(0)
        , buckSz_(0)
        , token_(0)
        , delta_(0)
        , time_(0)
    {
        init(freq, bucketSz);
    }
    void init(size_t freq, size_t bucketSz){
        if(!freq_){
            token_ = delta_ = 0;
            time_ = tools::MonoTimeUs();
        }
        freq_ = freq;
        buckSz_ = bucketSz;
    }
    /**  @} */
    /**
     * @brief Generate tokens.
     * @param nowUs Current monotonic time, obtained from @ref tools::MonoTimeUs(), can be omitted
     */
    void generate(uint64_t nowUs = 0){
        if(!valid())
            return;
        if(nowUs < time_)
            nowUs = tools::MonoTimeUs();
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
    /**
     * @brief Check if there are enough tokens.
     * @param need Number of tokens needed
     * @return @c true if there are at least @c need tokens in bucket; @c false otherwise
     */
    bool check(size_t need) const{return (token_ >= 0 && size_t(token_) >= need);}
    /**
     * @brief Get number of tokens in bucket.
     * This function may return a negative number when tokens have been overdrawn.
     * @return
     *   @li Positive number: Number of tokens in bucket;
     *   @li Negative number: Number of tokens overdrawn;
     * @sa overdraw
     */
    ssize_t token() const{return token_;}
    /**
     * @brief Get tokens.
     * @param need Number of tokens needed
     * @return
     *   @li @c true: Succeeded. @c need tokens are removed from bucket;
     *   @li @c false: Failed. No tokens are removed from bucket;
     */
    bool get(size_t need = 1){
        if(token_ < 0 || size_t(token_) < need)
            return false;
        token_ -= need;
        return true;
    }
    /**
     * @brief Overdraw tokens.
     * @param need Number of tokens needed
     * @return
     *   @li @c true: Succeeded. @c need tokens are removed or overdrawn from bucket;
     *   @li @c false: Failed. No tokens are removed from bucket;
     */
    bool overdraw(size_t need){
        if(!valid() || ssize_t(need) < 0)
            return false;
        token_ -= need;
        return true;
    }
private:
    bool valid() const {return freq_ > 0;}
    size_t freq_;
    size_t buckSz_;
    ssize_t token_;
    ssize_t delta_;
    uint64_t time_;      //last generate() time
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

