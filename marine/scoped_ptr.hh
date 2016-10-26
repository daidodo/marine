#ifndef DOZERG_SCOPED_POINTER_H_20130603
#define DOZERG_SCOPED_POINTER_H_20130603

#include <algorithm>        //std::swap
#include "tools/memory.hh"

NS_SERVER_BEGIN

template<class T, class Alloc = std::allocator<T> >
class CScopedPtr
{
    typedef CScopedPtr<T, Alloc>    __Myt;
    typedef void (__Myt::*__SafeBool)(__Myt &);
public:
    typedef T       element_type;
    typedef Alloc   allocator_type;
    typedef T *     pointer;
    typedef T &     reference;
    //functions
    explicit CScopedPtr(pointer p = NULL):p_(p){}
    ~CScopedPtr(){tools::Delete(p_, allocator_type());}
    bool operator !() const throw(){return (NULL == p_);}
    operator __SafeBool() const throw(){return (operator !() ? NULL : &__Myt::swap);}
    pointer get() const throw(){return p_;}
    pointer operator ->() const throw(){return get();}
    reference operator *() const throw(){return *get();}
    void reset(pointer p = NULL) throw(){
        if(p != p_)
            __Myt(p).swap(*this);
    }
    pointer release() throw(){
        pointer p = p_;
        p_ = NULL;
        return p;
    }
    void swap(__Myt & a) throw(){std::swap(p_, a.p_);}
private:
    CScopedPtr(const __Myt &);      //disable copy and assignment
    __Myt & operator =(const __Myt &);
    //fields
    pointer p_;
};

template<class T, class Alloc>
inline void swap(CScopedPtr<T, Alloc> & a, CScopedPtr<T, Alloc> & b) throw()
{
    a.swap(b);
}

NS_SERVER_END

#endif

