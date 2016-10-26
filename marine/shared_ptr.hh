#ifndef DOZERG_SHARED_PTR_H_20070828
#define DOZERG_SHARED_PTR_H_20070828

/*
    CSharedPtr      采用引用计数的智能指针，多线程安全
                    需要与allocator一起使用
                    既可以防止内存泄漏,又减少内存碎片
    History
        20070924    在__RefImp中加入pthread_mutex_t,修正多线程下操作的错误
        20070925    把cnt_和pthread_mutex_t改成CLockInt
        20080123    __RefImp的cnt_成员类型改为模板参数IntType.CSharedPtr加入Lock模板参数,表示是否需要加锁
        20080604    增加release，set成员函数，safe_bool_type，和4个比较操作
        20080912    增加swap函数，并重载std::swap
        20080920    使用模板参数决定锁类型
        20120118    增加release()
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
