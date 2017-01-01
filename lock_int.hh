#ifndef DOZERG_LOCK_INT_H_20070916
#define DOZERG_LOCK_INT_H_20070916

/*
    提供加锁保护的整数类型模板
        CLockInt        提供加锁的整数类型
        CLockIntMax     有统计最大值功能的加锁整数类型
        CLockIntMinMax  有统计最小/最大值功能的加锁整数类型
    History
        20070925    把pthread_mutex_t改成CMutex
        20080509    使用guard_type;加入CLockIntRange
        20080920    使用模板参数决定锁类型
        20130612    重写，去掉CLockIntRange，增加CLockIntMinMax
//*/

#include "impl/lock_int_impl.hh"

NS_SERVER_BEGIN

template<typename T, class LockT = CSpinLock>
class CLockInt : public NS_IMPL::CLockIntBase<T, LockT, NS_IMPL::__ExtraEmpty<T> >
{
    typedef NS_IMPL::CLockIntBase<T, LockT, NS_IMPL::__ExtraEmpty<T> >   __MyBase;
    typedef CLockInt<T, LockT>  __Myt;
public:
    typedef typename __MyBase::value_type   value_type;
    explicit CLockInt(value_type c = value_type()):__MyBase(c){}
    CLockInt(const __Myt & c):__MyBase(c.load()){}
    value_type operator =(value_type c) volatile{
        __MyBase::operator =(c);
        return *this;
    }
};

template<typename T, class LockT = CSpinLock>
class CLockIntMax : public NS_IMPL::CLockIntBase<T, LockT, NS_IMPL::__ExtraMax<T> >
{
    typedef NS_IMPL::CLockIntBase<T, LockT, NS_IMPL::__ExtraMax<T> >    __MyBase;
    typedef CLockIntMax<T, LockT>  __Myt;
public:
    typedef typename __MyBase::value_type   value_type;
    explicit CLockIntMax(value_type c = value_type()):__MyBase(c){}
    CLockIntMax(const __Myt & c):__MyBase(c.load()){}
    value_type operator =(value_type c) volatile{
        __MyBase::operator =(c);
        return *this;
    }
    value_type max() const volatile{
        typename __MyBase::guard_type g(__MyBase::lock_);
        return __MyBase::e_.m_;
    }
    //重置max值，返回之前的max值
    value_type resetMax() volatile{
        typename __MyBase::guard_type g(__MyBase::lock_);
        value_type ret = __MyBase::e_.m_;
        __MyBase::e_.m_ = __MyBase::v_;
        return ret;
    }
};

template<typename T, class LockT = CSpinLock>
class CLockIntMinMax : public NS_IMPL::CLockIntBase<T, LockT, NS_IMPL::__ExtraMinMax<T> >
{
    typedef NS_IMPL::CLockIntBase<T, LockT, NS_IMPL::__ExtraMinMax<T> > __MyBase;
    typedef CLockIntMinMax<T, LockT>  __Myt;
public:
    typedef typename __MyBase::value_type   value_type;
    explicit CLockIntMinMax(value_type c = value_type()):__MyBase(c){}
    CLockIntMinMax(const __Myt & c):__MyBase(c.load()){}
    value_type operator =(value_type c) volatile{
        __MyBase::operator =(c);
        return *this;
    }
    value_type min() const volatile{
        typename __MyBase::guard_type g(__MyBase::lock_);
        return __MyBase::e_.mi_;
    }
    value_type max() const volatile{
        typename __MyBase::guard_type g(__MyBase::lock_);
        return __MyBase::e_.ma_;
    }
    //重置min, max值
    //oldMin: 返回之前的min值
    //oldMax: 返回之前的max值
    void resetMinMax(value_type * oldMin, value_type * oldMax) volatile{
        typename __MyBase::guard_type g(__MyBase::lock_);
        if(oldMin)
            *oldMin = __MyBase::e_.mi_;
        if(oldMax)
            *oldMax = __MyBase::e_.ma_;
        __MyBase::e_.mi_ = __MyBase::e_.ma_ = __MyBase::v_;
    }
};

NS_SERVER_END

#endif
