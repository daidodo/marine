#ifndef DOZERG_LOCK_INT_IMPL_H_20130612
#define DOZERG_LOCK_INT_IMPL_H_20130612

#include "../mutex.hh"

NS_IMPL_BEGIN

//predictors

template<typename T, bool B>
struct __PredBool
{
    bool operator ()(const T & v) const{return B;}
};

template<typename T>
class __PredEqual
{
    T c_;
public:
    explicit __PredEqual(T c):c_(c){}
    bool operator ()(const T & v) const{return (v == c_);}
};

//operators

template<typename T>
class __OpAdd
{
    T c_;
public:
    explicit __OpAdd(T c):c_(c){}
    void operator ()(T & v) const{v += c_;}
};

template<typename T>
class __OpSub
{
    T c_;
public:
    explicit __OpSub(T c):c_(c){}
    void operator ()(T & v) const{v -= c_;}
};

template<typename T>
class __OpOr
{
    T c_;
public:
    explicit __OpOr(T c):c_(c){}
    void operator ()(T & v) const{v |= c_;}
};

template<typename T>
class __OpAnd
{
    T c_;
public:
    explicit __OpAnd(T c):c_(c){}
    void operator ()(T & v) const{v &= c_;}
};

template<typename T>
class __OpXor
{
    T c_;
public:
    explicit __OpXor(T c):c_(c){}
    void operator ()(T & v) const{v ^= c_;}
};

template<typename T>
class __OpNand
{
    T c_;
public:
    explicit __OpNand(T c):c_(c){}
    void operator ()(T & v) const{v = ~(v & c_);}
};

template<typename T>
class __OpAssign
{
    T c_;
public:
    explicit __OpAssign(T c):c_(c){}
    void operator ()(T & v) const{v = c_;}
};

//extra

template<typename T>
struct __ExtraEmpty
{
    explicit __ExtraEmpty(const T &){}
    void operator ()(const T & v) volatile{}
};

template<typename T>
struct __ExtraMax
{
    T m_;
    explicit __ExtraMax(const T & v):m_(v){}
    void operator ()(const T & v) volatile{
        if(v > m_)
            m_ = v;
    }
};

template<typename T>
struct __ExtraMinMax
{
    T mi_, ma_;
    explicit __ExtraMinMax(const T & v):mi_(v), ma_(v){}
    void operator ()(const T & v) volatile{
        if(v < mi_)
            mi_ = v;
        if(v > ma_)
            ma_ = v;
    }
};

#define __FETCH_TEST_MEMBER(name, OP)  \
    value_type fetch_##name (value_type c) volatile{  \
        value_type ret; \
        mutate(OP<value_type>(c), &ret, NULL);  \
        return ret; \
    }   \
    value_type name##_fetch (value_type c) volatile{  \
        value_type ret; \
        mutate(OP<value_type>(c), NULL, &ret);  \
        return ret; \
    }   \
    template<class Pred>    \
    bool test_##name (Pred pred, value_type c, value_type * oldVal, value_type * newVal) volatile{ \
        return test_mutate(pred, OP<value_type>(c), oldVal, newVal);    \
    }

template<typename T, class LockT, class Extra>
class CLockIntBase
{
    typedef CLockIntBase<T, LockT, Extra>   __Myt;
    typedef Extra                           __Extra;
public:
    typedef T                   value_type;
    typedef LockT               lock_type;
    typedef CGuard<lock_type>   guard_type;
    explicit CLockIntBase(value_type c):v_(c), e_(v_){}
    CLockIntBase(const __Myt & c):v_(c.load()), e_(v_){}
    //basic APIs
    value_type load() const volatile{
        guard_type g(lock_);
        return v_;
    }
    void store(value_type c) volatile{
        mutate(__OpAssign<value_type>(c), NULL, NULL);
    }
    //如果满足条件，则进行操作
    //pred: 需要满足的条件，需要实现
    //      bool operator ()(const value_type & v) const;
    //op: 满足条件时进行的操作，需要实现
    //      void operator ()(value_type & v) const;
    //oldVal: 返回调用之前的值
    //newVal: 返回调用之后的值
    //return:
    //  true    条件被满足，操作完成
    //  false   条件不满足，未进行操作
    template<class Pred, class Op>
    bool test_mutate(const Pred & pred, Op & op, value_type * oldVal, value_type * newVal) volatile{
        guard_type g(lock_);
        value_type v(v_);
        if(oldVal)
            *oldVal = v;
        const bool ret = pred(v);
        if(ret){
            op(v);
            e_(v);
            v_ = v;
        }
        if(newVal)
            *newVal = v_;
        return ret;
    }
    template<class Pred, class Op>
    bool test_mutate(const Pred & pred, const Op & op, value_type * oldVal, value_type * newVal) volatile{
        return test_mutate(pred, const_cast<Op &>(op), oldVal, newVal); //const_cast is only for saving typing
    }
    //进行任意操作
    //op: 进行的操作
    //oldVal: 返回调用之前的值
    //newVal: 返回调用之后的值
    template<class Op>
    void mutate(Op & op, value_type * oldVal, value_type * newVal) volatile{
        test_mutate(__PredBool<value_type, true>(), op, oldVal, newVal);
    }
    template<class Op>
    void mutate(const Op & op, value_type * oldVal, value_type * newVal) volatile{
        test_mutate(__PredBool<value_type, true>(), op, oldVal, newVal);
    }
    __FETCH_TEST_MEMBER(add, __OpAdd)
    __FETCH_TEST_MEMBER(sub, __OpSub)
    __FETCH_TEST_MEMBER(or, __OpOr)
    __FETCH_TEST_MEMBER(and, __OpAnd)
    __FETCH_TEST_MEMBER(xor, __OpXor)
    __FETCH_TEST_MEMBER(nand, __OpNand)
#undef __FETCH_TEST_MEMBER
    //伪码如下：
    //return = v_; swap(v_, c)
    value_type swap(value_type c) volatile{
        value_type ret;
        mutate(__OpAssign<value_type>(c), &ret, NULL);
        return ret;
    }
    //伪码如下：
    //if(v_ == expval){ *oldval = v_; v_ = newval; return true; }
    //if(v_ != expval){ *oldval = v_; return false; }
    bool compare_swap(value_type expval, value_type newval, value_type * oldval = NULL) volatile{
        return test_mutate(__PredEqual<value_type>(expval), __OpAssign<value_type>(newval), oldval, NULL);
    }
    //operators
    value_type operator =(const __Myt & c) volatile{
        value_type ret;
        mutate(__OpAssign<value_type>(c.load()), NULL, &ret);
        return ret;
    }
    value_type operator =(value_type c) volatile{
        value_type ret;
        mutate(__OpAssign<value_type>(c), NULL, &ret);
        return ret;
    }
    operator value_type() const volatile{return load();}
    value_type operator ++() volatile{return add_fetch(1);}
    value_type operator --() volatile{return sub_fetch(1);}
    value_type operator ++(int) volatile{return fetch_add(1);}
    value_type operator --(int) volatile{return fetch_sub(1);}
    value_type operator +=(value_type c) volatile{return add_fetch(c);}
    value_type operator -=(value_type c) volatile{return sub_fetch(c);}
    value_type operator |=(value_type c) volatile{return or_fetch(c);}
    value_type operator &=(value_type c) volatile{return and_fetch(c);}
    value_type operator ^=(value_type c) volatile{return xor_fetch(c);}
protected:
    lock_type lock_;
    value_type v_;
    __Extra e_;
};

NS_IMPL_END

#endif

