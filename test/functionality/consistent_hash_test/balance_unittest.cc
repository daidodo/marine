#include <marine/consistent_hash.hh>

#include <cmath>
#include <marine/iter_adapter.hh>

#include "../inc.h"

typedef CConsistentHash<uint64_t>   __Hash;
typedef __Hash::value_type          __Val;
typedef std::vector<std::pair<__Val, __Val> >   __Ret;

template<class Iter>
static double StandardDeviation(Iter first, Iter last)
{
    double sum = 0;
    double sq_sum = 0;
    size_t cnt = 0;
    for(Iter i = first;i != last;++i, ++cnt){
        sum += *i;
        sq_sum += double(*i) * (*i);
    }
    if(!cnt)
        return 0;
    const double ave = sum / cnt;
    assert(ave > 0);
    return (::sqrt((sq_sum - sum * ave) / cnt) / ave * 1000);
}

template<class Iter>
static double MaxDeviation(Iter first, Iter last)
{
    double sum = 0;
    size_t cnt = 0;
    for(Iter i = first;i != last;++i, ++cnt)
        sum += *i;
    if(!cnt)
        return 0;
    const double ave = sum / cnt;
    sum = 0;
    for(Iter i = first;i != last;++i, ++cnt){
        const double d = ::fabs(*i - ave);
        if(d > sum)
            sum = d;
    }
    assert(ave > 0);
    return (sum / ave * 100);
}

static const __Val & extractWeight(const std::pair<__Val, __Val> & p)
{
    return p.second;
}

static void calcBalance(size_t c, size_t w, size_t & mw)
{
    __Hash h;
    for(size_t i = 0;i < c;++i)
        h.setValue(i + 1, w);
    __Ret ret;
    h.actualWeight(ret);
#if 1
    const double sd = StandardDeviation(
            const_iter_adapt_fun<__Val>(ret.begin(), extractWeight),
            const_iter_adapt_fun<__Val>(ret.end(), extractWeight));
    if(sd >= 30 && mw < w)
        mw = w;
#else
    const double sd = MaxDeviation(
            const_iter_adapt_fun<__Val>(ret.begin(), extractWeight),
            const_iter_adapt_fun<__Val>(ret.end(), extractWeight));
    if(sd >= 10 && mw < w)
        mw = w;
#endif
    cout<<c<<"\t"<<w<<"\t"<<sd<<"\n";
}

static void balanceTest()
{
    size_t c, w;
    size_t minWeight = 0;

#if 0
    for(c = 100;c <= 2000;c += 100)
        for(w = minWeight + 100;w <= 2000;w += 100)
            calcBalance(c, w, minWeight);
#endif
#if 1
    c = 20;
    for(w = minWeight + 100;w <= 2000;w += 100)
        calcBalance(c, w, minWeight);

    c = 100;
    for(w = minWeight + 100;w <= 2000;w += 100)
        calcBalance(c, w, minWeight);

    return;
    c = 500;
    for(w = minWeight + 100;w <= 2000;w += 100)
        calcBalance(c, w, minWeight);
#endif
#if 0
    w = 1200;
    for(c = 100;c <= 2000;c += 100)
        calcBalance(c, w, minWeight);
#endif

    cout<<"MIN weight: "<<(minWeight + 100)<<"\n";
}

TEST(CConsistentHash, balance)
{
    balanceTest();
}
