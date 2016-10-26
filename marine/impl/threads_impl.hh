#ifndef DOZERG_THREADS_IMPL_H_20130222
#define DOZERG_THREADS_IMPL_H_20130222

#include "environment.hh"

NS_IMPL_BEGIN

template<class Int>
class CActive
{
public:
    explicit CActive(Int & cnt)
        : cnt_(cnt)
    {
        ++cnt_;
    }
    ~CActive(){--cnt_;}
private:
    CActive(const CActive &);
    CActive & operator =(const CActive &);
    Int & cnt_;
};

template<typename Int>
struct IsPositive
{
    bool operator ()(const Int & v) const{return v > 0;}
};

NS_IMPL_END

#endif

