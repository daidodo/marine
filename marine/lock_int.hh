#ifndef DOZERG_LOCK_INT_H_20070916
#define DOZERG_LOCK_INT_H_20070916

/*
    �ṩ������������������ģ��
        CLockInt        �ṩ��������������
        CLockIntMax     ��ͳ�����ֵ���ܵļ�����������
        CLockIntMinMax  ��ͳ����С/���ֵ���ܵļ�����������
    History
        20070925    ��pthread_mutex_t�ĳ�CMutex
        20080509    ʹ��guard_type;����CLockIntRange
        20080920    ʹ��ģ���������������
        20130612    ��д��ȥ��CLockIntRange������CLockIntMinMax
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
    //����maxֵ������֮ǰ��maxֵ
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
    //����min, maxֵ
    //oldMin: ����֮ǰ��minֵ
    //oldMax: ����֮ǰ��maxֵ
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
