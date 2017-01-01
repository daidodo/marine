#ifndef DOZERG_TOOLS_MEMORY_IMPL_H_20130109
#define DOZERG_TOOLS_MEMORY_IMPL_H_20130109

#include <cstddef>  //std::size_t
#include <memory>   //std::allocator
#include "../template.hh"

NS_IMPL_BEGIN

//allocate
template<class T, class A>
T * __Allocate(A a, size_t sz)
{
    return a.allocate(sz);
}

template<class T, class A>
T * __Allocate(A a, CFalseType)
{
    return __Allocate<T>(a, size_t(1));
}

template<class T>
T * __Allocate(size_t sz, CTrueType)
{
    return __Allocate<T>(std::allocator<T>(), sz);
}

//deallocate
template<class T, class A>
void __Deallocate(T *& p, size_t sz, A a) throw()
{
    __UNUSED typedef char (&dummy)[sizeof(T)];   //保证T类型的完整性
    if(p){
        a.deallocate(p, sz);
        p = 0;
    }
}

template<class T, class A>
void __Deallocate(T *& p, A a, CFalseType) throw()
{
    __Deallocate(p, size_t(1), a);
}

template<class T>
void __Deallocate(T *& p, size_t sz, CTrueType) throw()
{
    __Deallocate(p, sz, std::allocator<T>());
}

//construct
template<class T>
T * __Construct(T * p)
{
    return new (p) T();
}

template<class T, class V>
T * __Construct(T * p, const V & v)
{
    return new (p) T(v);
}

template<class T, class V1, class V2>
T * __Construct(T * p, const V1 & v1, const V2 & v2)
{
    return new (p) T(v1, v2);
}

//destroy
template<class T>
void __Destroy(T * p) throw()
{
    typedef char type_must_be_complete[sizeof(T) ? 1 : -1];
    (void)sizeof(type_must_be_complete);
    if(p)
        p->~T();
}

//new
template<class T, class A>
T * __New(A a, size_t sz)
{
    T * p = __Allocate<T>(a, sz);
    if(p)
        for(size_t i = 0;i < sz;++i)
            __Construct(p + i);
    return p;
}

template<class T, class A>
T * __New(A a, CFalseType)
{
    return __New<T>(a, size_t(1));
}

template<class T>
T * __New(size_t sz, CTrueType)
{
    return __New<T>(std::allocator<T>(), sz);
}

//new1
template<class T, class V, class A>
T * __New1(const V & v, A a, size_t sz)
{
    T * p = __Allocate<T>(a, sz);
    if(p)
        for(size_t i = 0;i < sz;++i)
            __Construct(p + i, v);
    return p;
}

template<class T, class V, class A>
T * __New1(const V & v, A a, CFalseType)
{
    return __New1<T>(v, a, size_t(1));
}

template<class T, class V>
T * __New1(const V & v, size_t sz, CTrueType)
{
    return __New1<T>(v, std::allocator<T>(), sz);
}

//new2
template<class T, class V1, class V2, class A>
T * __New2(const V1 & v1, const V2 & v2, A a, size_t sz)
{
    T * p = __Allocate<T>(a, sz);
    if(p)
        for(size_t i = 0;i < sz;++i)
            __Construct(p + i, v1, v2);
    return p;
}

template<class T, class V1, class V2, class A>
T * __New2(const V1 & v1, const V2 & v2, A a, CFalseType)
{
    return __New2<T>(v1, v2, a, size_t(1));
}

template<class T, class V1, class V2>
T * __New2(const V1 & v1, const V2 & v2, size_t sz, CTrueType)
{
    return __New2<T>(v1, v2, std::allocator<T>(), sz);
}

//delete
template<class T, class A>
void __Delete(T *& p, size_t sz, A a) throw()
{
    if(p){
        for(size_t i = 0;i < sz;++i)
            __Destroy(p + i);
        __Deallocate(p, sz, a);
        p = 0;
    }
}

template<class T, class A>
void __Delete(T *& p, A a, CFalseType) throw()
{
    __Delete(p, size_t(1), a);
}

template<class T>
void __Delete(T *& p, size_t sz, CTrueType) throw()
{
    __Delete(p, sz, std::allocator<T>());
}

NS_IMPL_END

#endif

