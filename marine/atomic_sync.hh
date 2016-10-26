#ifndef DOZERG_ATOMIC_SYNC_H_20130117
#define DOZERG_ATOMIC_SYNC_H_20130117

/*
    提供对POD类型的原子操作，使用__sync_xxx实现
        CAtomicSync     提供对POD类型的原子操作
//*/

#include <cassert>
#include <cstddef>  //NULL
#include "impl/environment.hh"

NS_SERVER_BEGIN

#define __FETCH_MEMBER(op)  \
    value_type fetch_##op (value_type c) volatile{return __sync_fetch_and_##op (&v_, c);}   \
    value_type op##_fetch (value_type c) volatile{return __sync_##op##_and_fetch (&v_, c);} \
    static value_type fetch_##op (value_type * v, value_type c){assert(v);return __sync_fetch_and_##op (v, c);}   \
    static value_type op##_fetch (value_type * v, value_type c){assert(v);return __sync_##op##_and_fetch (v, c);}

//T: 整型POD类型
template<typename T>
class CAtomicSync
{
    typedef CAtomicSync<T>  __Myt;
public:
    typedef T   value_type;
    //构造/析构
    explicit CAtomicSync(value_type c = value_type()):v_(c){}
    CAtomicSync(const __Myt & c):v_(c.load()){}
    //获取值
    value_type load() const volatile{
        return const_cast<__Myt *>(this)->add_fetch(0);
    }
    //v: 要获取值的变量地址
    static value_type load(const value_type * v){
        assert(v);
        return add_fetch(const_cast<value_type *>(v), 0);
    }
    //设置值
    //c: 要设置的值
    //v: 要设置的变量地址
    void store(value_type c) volatile{swap(c);}
    static void store(value_type * v, value_type c){
        assert(v);
        swap(v, c);
    }
    //以下API包括:
    //fetch_xxx     先取值，然后进行xxx操作
    //xxx_fetch     先进行xxx操作，然后取值
    __FETCH_MEMBER(add)
    __FETCH_MEMBER(sub)
    __FETCH_MEMBER(or)
    __FETCH_MEMBER(and)
    __FETCH_MEMBER(xor)
    //__FETCH_MEMBER(nand)
#undef __FETCH_MEMBER
    //设置值为c，返回之前的值
    value_type swap(value_type c) volatile{return __sync_lock_test_and_set(&v_, c);}
    //设置*v的值为c，返回*v之前的值
    static value_type swap(value_type * v, value_type c){assert(v);return __sync_lock_test_and_set(v, c);}
    //伪码如下：
    //if(v_ == expval){ *oldval = v_; v_ = newval; return true; }
    //if(v_ != expval){ *oldval = v_; return false; }
    bool compare_swap(value_type expval, value_type newval, value_type * oldval = NULL) volatile{
        if(NULL == oldval)
            return __sync_bool_compare_and_swap(&v_, expval, newval);
        *oldval = __sync_val_compare_and_swap(&v_, expval, newval);
        return (*oldval == expval);
    }
    //伪码如下：
    //if(*val == expval){ *oldval = *val; *val = newval; return true; }
    //if(*val != expval){ *oldval = *val; return false; }
    static bool compare_swap(value_type * val, value_type expval, value_type newval, value_type * oldval){
        assert(val);
        if(NULL == oldval)
            return __sync_bool_compare_and_swap(val, expval, newval);
        *oldval = __sync_val_compare_and_swap(val, expval, newval);
        return (*oldval == expval);
    }
    //operators
    value_type operator =(const __Myt & c) volatile{return (v_ = c.load());}
    value_type operator =(value_type c) volatile{return (v_ = c);}
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
private:
    value_type v_;
};

NS_SERVER_END

#endif

