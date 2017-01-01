#ifndef DOZERG_TEMPLATE_IMPL_H_20091226
#define DOZERG_TEMPLATE_IMPL_H_20091226

#include <cstddef>          //size_t
#include "environment.hh"

NS_IMPL_BEGIN

struct CTrueType{};
struct CFalseType{};

template<bool>
struct CAssert;

template<>
struct CAssert<true>
{
    static const bool result = true;
};

#define __JOIN_TOKEN(a, b)  a##b
#define JOIN_TOKEN(a, b)    __JOIN_TOKEN(a, b)

#define __STATIC_ASSERT( token, expr )   enum { token = 1 / !!(expr) }

//array size
template<size_t N>
struct __size_type
{
    // cppcheck-suppress unusedStructMember
    char dummy_[N];
};

template<class T, size_t N>
__size_type<N> __array_size(T (&a)[N]);

#ifdef __GNUC__ // gcc allow 0 sized array

template<class T>
__size_type<0> __array_size(T (&a)[0]);

#endif

inline size_t __stl_hash_string(const char * s)
{
    size_t ret = 0;
    for(;s && *s;++s)
        ret = 5 * ret + *s;
    return ret;
}

inline size_t __stl_hash_string(const char * s, size_t sz)
{
    size_t ret = 0;
    for(size_t i = 0;i < sz;++s,++i)
        ret = 5 * ret + *s;
    return ret;
}

NS_IMPL_END

#endif

