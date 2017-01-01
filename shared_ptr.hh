#ifndef DOZERG_SHARED_PTR_H_20070828
#define DOZERG_SHARED_PTR_H_20070828

/*
    CSharedPtr      �������ü���������ָ�룬���̰߳�ȫ
                    ��Ҫ��allocatorһ��ʹ��
                    �ȿ��Է�ֹ�ڴ�й©,�ּ����ڴ���Ƭ
    History
        20070924    ��__RefImp�м���pthread_mutex_t,�������߳��²����Ĵ���
        20070925    ��cnt_��pthread_mutex_t�ĳ�CLockInt
        20080123    __RefImp��cnt_��Ա���͸�Ϊģ�����IntType.CSharedPtr����Lockģ�����,��ʾ�Ƿ���Ҫ����
        20080604    ����release��set��Ա������safe_bool_type����4���Ƚϲ���
        20080912    ����swap������������std::swap
        20080920    ʹ��ģ���������������
        20120118    ����release()
//*/

#include <algorithm>     //std::swap
#include "impl/shared_ptr_impl.hh"

NS_SERVER_BEGIN

template<class T, class Alloc = std::allocator<T> >
class CSharedPtr
{   //typedefs
    typedef CSharedPtr<T, Alloc>        __Myt;
    typedef NS_IMPL::__RefImp<T, Alloc> __RefType;
public:
    typedef T   element_type;
    typedef T & reference;
    typedef T * pointer;
    typedef const T & const_reference;
    typedef const T * const_pointer;
    typedef typename __RefType::__ElemAlloc allocator_type;
private:
    typedef typename __RefType::__RefAlloc __RefAlloc;
    typedef void (__Myt::*safe_bool_type)(pointer);
public:
    //functions
    explicit CSharedPtr(pointer p = NULL):ref_(NULL){init(p);}
    CSharedPtr(const __Myt & a)
        : ref_(NULL)
    {
        __RefType::changeRef(ref_, a.ref_);
    }
    ~CSharedPtr() throw(){__RefType::subRef(ref_);}
    // cppcheck-suppress operatorEqVarError
    __Myt & operator =(const __Myt & a) throw(){
        __RefType::changeRef(ref_, a.ref_);
        return *this;
    }
    __Myt & operator =(pointer a){
        reset(a);
        return *this;
    }
    pointer get() const throw(){return (operator !() ? NULL : ref_->ptr_);}
    pointer operator ->() const throw(){return get();}
    reference operator *() const throw(){return *get();}
    bool operator !() const throw(){return !ref_;}
    operator safe_bool_type() const throw(){return (operator !() ? 0 : &__Myt::init);}
    bool operator ==(const __Myt & a) const throw(){return (ref_ == a.ref_);}
    bool operator !=(const __Myt & a) const throw(){return !operator ==(a);}
    bool operator ==(const_pointer a) const throw(){
        return (operator !() ? !a : a == operator ->());
    }
    bool operator !=(const_pointer a) const throw(){return !operator ==(a);}
    void reset(pointer a = NULL){
        if(operator !=(a)){
            __RefType::subRef(ref_);
            init(a);
        }
    }
    void swap(__Myt & a) throw(){std::swap(ref_, a.ref_);}
private:
    void init(pointer p){
        if(p)
            ref_ = tools::New1<__RefType>(p, __RefAlloc());
    }
    //field
    __RefType * ref_;
};

template<class T, class A>
inline void swap(CSharedPtr<T, A> & a, CSharedPtr<T, A> & b) throw()
{
    a.swap(b);
}

template<class T, class A>
inline bool operator ==(const T * p, const CSharedPtr<T, A> & a) throw()
{
    return (a == p);
}

template<class T, class A>
inline bool operator !=(const T * p, const CSharedPtr<T, A> & a) throw()
{
    return (a != p);
}

NS_SERVER_END

#endif
