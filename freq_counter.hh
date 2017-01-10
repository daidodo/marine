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
 * @brief Events counting and frequency calculation.
 * @author Zhao DAI
 */

#ifndef DOZERG_FREQ_COUNTER_H_20170109
#define DOZERG_FREQ_COUNTER_H_20170109

#include "tools/time.hh"
#include "to_string.hh"

NS_SERVER_BEGIN

template<size_t kDuration, size_t kGranularity = 1>
class CFreqCounter
{
    static const size_t kSize = kDuration + 1;
public:
    CFreqCounter()
        : up_(tools::MonoTime(NULL))
        , last_(up_)
        , lasti_(0)
    {
        ::memset(cnt_, 0, sizeof cnt_);
    }
    static size_t granularity(){return kGranularity;}
    static size_t duration(){return kDuration;}
    void occur(size_t times = 1){
        update();
        cnt_[lasti_ % kSize] += times;
    }
    size_t frequency(){
        update();
        size_t dur = kSize;
        const size_t s = sumTo(&dur);
        return (s / dur);
    }
    double frequencyDouble(){
        update();
        size_t dur = kSize;
        const double s = sumTo(&dur);
        return (s / dur);
    }
    std::string toString() const{
        CToString oss;
        oss<<"{up_="<<up_
            <<", last_="<<last_
            <<", lasti_="<<lasti_
            <<", cnt_={"<<cnt_[0];
        for(size_t i = 1;i < kSize;++i)
            oss<<", "<<cnt_[i];
        oss<<"}}";
        return oss.str();
    }
private:
    size_t sumTo(size_t * dur) const{
        size_t s = 0, d = 0;
        if(lasti_ < 2){
            s = cnt_[0];
            d = 1;
        }else if( lasti_ < kSize){
            for(d = 1;d < lasti_;++d)
                s += cnt_[d];
            d = lasti_ - 1;
        }else{
            size_t cur = lasti_ % kSize;
            for(size_t i = 0;i < kSize;++i)
                if(i != cur)
                    s += cnt_[i];
            d = kSize - 1;
        }
        if(dur)
            *dur = d;
        return s;
    }
    void update(){
        time_t t = tools::MonoTime(NULL);
        if(t > last_){
            size_t e = idx(t);
            if(e < lasti_ + kSize){
                for(;++lasti_ <= e;)
                    cnt_[lasti_ % kSize] = 0;
            }else
                ::memset(cnt_, 0, sizeof cnt_);
            last_ = t;
            lasti_ = e;
        }
    }
    size_t idx(time_t cur) const{
        int t = cur - up_;
        if(t < 0)
            t = 0;
        if(kGranularity > 1)
            t /= kGranularity;
        return t;
    }
    const time_t up_;
    time_t last_;
    size_t lasti_;
    size_t cnt_[kSize];
};

NS_SERVER_END

#endif

