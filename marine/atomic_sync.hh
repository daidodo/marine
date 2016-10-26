#ifndef DOZERG_ATOMIC_SYNC_H_20130117
#define DOZERG_ATOMIC_SYNC_H_20130117

/*
    �ṩ��POD���͵�ԭ�Ӳ�����ʹ��__sync_xxxʵ��
        CAtomicSync     �ṩ��POD���͵�ԭ�Ӳ���
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

//T: ����POD����
template<typename T>
class CAtomicSync
{
    typedef CAtomicSync<T>  __Myt;
public:
    typedef T   value_type;
    //����/����
    explicit CAtomicSync(value_type c = value_type()):v_(c){}
    CAtomicSync(const __Myt & c):v_(c.load()){}
    //��ȡֵ
    value_type load() const volatile{
        return const_cast<__Myt *>(this)->add_fetch(0);
    }
    //v: Ҫ��ȡֵ�ı�����ַ
    static value_type load(const value_type * v){
        assert(v);
        return add_fetch(const_cast<value_type *>(v), 0);
    }
    //����ֵ
    //c: Ҫ���õ�ֵ
    //v: Ҫ���õı�����ַ
    void store(value_type c) volatile{swap(c);}
    static void store(value_type * v, value_type c){
        assert(v);
        swap(v, c);
    }
    //����API����:
    //fetch_xxx     ��ȡֵ��Ȼ�����xxx����
    //xxx_fetch     �Ƚ���xxx������Ȼ��ȡֵ
    __FETCH_MEMBER(add)
    __FETCH_MEMBER(sub)
    __FETCH_MEMBER(or)
    __FETCH_MEMBER(and)
    __FETCH_MEMBER(xor)
    //__FETCH_MEMBER(nand)
#undef __FETCH_MEMBER
    //����ֵΪc������֮ǰ��ֵ
    value_type swap(value_type c) volatile{return __sync_lock_test_and_set(&v_, c);}
    //����*v��ֵΪc������*v֮ǰ��ֵ
    static value_type swap(value_type * v, value_type c){assert(v);return __sync_lock_test_and_set(v, c);}
    //α�����£�
    //if(v_ == expval){ *oldval = v_; v_ = newval; return true; }
    //if(v_ != expval){ *oldval = v_; return false; }
    bool compare_swap(value_type expval, value_type newval, value_type * oldval = NULL) volatile{
        if(NULL == oldval)
            return __sync_bool_compare_and_swap(&v_, expval, newval);
        *oldval = __sync_val_compare_and_swap(&v_, expval, newval);
        return (*oldval == expval);
    }
    //α�����£�
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

