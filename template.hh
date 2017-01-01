#ifndef DOZERG_TEMPLATE_H_20091226
#define DOZERG_TEMPLATE_H_20091226

/*
    CTypeTraits
    CByteOrderTraits
    COmitCV
    ARRAY_SIZE
    STATIC_ASSERT
    CHECK_TYPE_SIZE
    CHECK_MEM_OFFSET
//*/

#include <byteswap.h>   //bswap_16, bswap_32, bswap_64
#include <functional>   //std::unary_function
#include <string>
#include "impl/template_impl.hh"

NS_SERVER_BEGIN

//struct CTypeTraits
template<typename Integer>
struct CTypeTraits
{
    typedef NS_IMPL::CFalseType __IsInteger;
    static const bool kCanMemcpy = false;
    static const bool kIsChar = false;
};

template<class T>
struct CTypeTraits<T *>
{
    typedef NS_IMPL::CFalseType __IsInteger;
    typedef T * __Unsigned;
    static const bool kCanMemcpy = true;
    static const int kMaxBits = sizeof(T *) * 8;
};

template<>
struct CTypeTraits<wchar_t>
{
    typedef NS_IMPL::CTrueType __IsInteger;
    static const bool kCanMemcpy = true;
    static const int kMaxBits = sizeof(wchar_t) * 8;
};

template<>
struct CTypeTraits<bool>
{
    typedef NS_IMPL::CTrueType __IsInteger;
    static const bool kCanMemcpy = true;
    static const int kMaxBits = sizeof(bool) * 8;
};

#define __INTEGER_TRAITS_FOR_POD(TYPE, IS_CHAR, SIGNED, UNSIGNED) template<>struct CTypeTraits<TYPE>{  \
    typedef NS_IMPL::CTrueType __IsInteger; \
    typedef SIGNED __Signed;    \
    typedef UNSIGNED __Unsigned;    \
    static const bool kCanMemcpy = true;    \
    static const bool kIsChar = IS_CHAR;       \
    static const int kMaxBits = sizeof(TYPE) * 8;   \
}

__INTEGER_TRAITS_FOR_POD(char, true, char, unsigned char);
__INTEGER_TRAITS_FOR_POD(signed char, true, signed char, unsigned char);
__INTEGER_TRAITS_FOR_POD(unsigned char, true, signed char, unsigned char);
__INTEGER_TRAITS_FOR_POD(short, false, short, unsigned short);
__INTEGER_TRAITS_FOR_POD(unsigned short, false, short, unsigned short);
__INTEGER_TRAITS_FOR_POD(int, false, int, unsigned int);
__INTEGER_TRAITS_FOR_POD(unsigned int, false, int, unsigned int);
__INTEGER_TRAITS_FOR_POD(long, false, long, unsigned long);
__INTEGER_TRAITS_FOR_POD(unsigned long, false, long, unsigned long);
__INTEGER_TRAITS_FOR_POD(long long, false, long long, unsigned long long);
__INTEGER_TRAITS_FOR_POD(unsigned long long, false, long long, unsigned long long);

#undef __INTEGER_TRAITS_FOR_POD

//转字节序函数选择器
template<typename T, size_t N>
struct CByteOrderTraits{};

template<typename T>
struct CByteOrderTraits<T, 1>
{
    inline static T swap(T a){return a;}
};

template<typename T>
struct CByteOrderTraits<T, 2>
{
    inline static T swap(T a){return bswap_16(a);}
};

template<typename T>
struct CByteOrderTraits<T, 4>
{
    inline static T swap(T a){return bswap_32(a);}
};

template<typename T>
struct CByteOrderTraits<T, 8>
{
    inline static T swap(T a){return bswap_64(a);}
};

//去掉类型的const和volatile修饰
//TODO:unit test
template<class T>
struct COmitCV
{
    typedef T result_type;
};

template<class T>
struct COmitCV<const T>
{
    typedef T result_type;
};

template<class T>
struct COmitCV<volatile T>
{
    typedef T result_type;
};

template<class T>
struct COmitCV<const volatile T>
{
    typedef T result_type;
};

//编译期获取数组元素个数
#define ARRAY_SIZE(a) sizeof(NS_SERVER::NS_IMPL::__array_size(a))

//from google protobuf(google/protobuf/stubs/common.h)
//#define ARRAY_SIZE(a)   ((sizeof(a) / sizeof(*(a))) / static_cast<size_t>(!(sizeof(a) % sizeof(*(a)))))

//compilation assertion
#define STATIC_ASSERT( expr )   __STATIC_ASSERT( JOIN_TOKEN(compile_assert_failed_at_line_, __LINE__), (expr) )

//check type size
#define CHECK_TYPE_SIZE( type, size )   __STATIC_ASSERT( size_of_type_is_not_##size, sizeof(type) == (size) )


//编译期检查结构体成员的偏移
#define CHECK_MEM_OFFSET(type, member, value)   __STATIC_ASSERT(offset_of_##member##_in_##type##_is_##value, __builtin_offsetof(type, member) == size_t(value))

//类型选择器
//TODO: unit test
template<class T1, class T2, bool Sel>
struct CTypeSelector
{
    typedef T1 result_type;
};

template<class T1, class T2>
struct CTypeSelector<T1, T2, false>
{
    typedef T2 result_type;
};

//CIdentity
//TODO:unit test
template<class T>
struct CIdentity : public std::unary_function<T, T>
{
    T & operator()(T & v) const{return v;}
    const T & operator()(const T & v) const{return v;}
};

//hash函数集合
//TODO:unit test
template<class Key>
struct CHashFn: public std::unary_function<Key, size_t>{};

#define __HASH_FUNCIONT_FOR_TYPE(TYPE, HASH)  \
    template<>struct CHashFn<TYPE> : public std::unary_function<TYPE, size_t>{  \
    size_t operator()(TYPE v) const{return (HASH);}}

__HASH_FUNCIONT_FOR_TYPE(char *, NS_IMPL::__stl_hash_string(v));
__HASH_FUNCIONT_FOR_TYPE(const char *, NS_IMPL::__stl_hash_string(v));
__HASH_FUNCIONT_FOR_TYPE(signed char *, NS_IMPL::__stl_hash_string((const char *)v));
__HASH_FUNCIONT_FOR_TYPE(const signed char *, NS_IMPL::__stl_hash_string((const char *)v));
__HASH_FUNCIONT_FOR_TYPE(unsigned char *, NS_IMPL::__stl_hash_string((const char *)v));
__HASH_FUNCIONT_FOR_TYPE(const unsigned char *, NS_IMPL::__stl_hash_string((const char *)v));
__HASH_FUNCIONT_FOR_TYPE(char, size_t(v));
__HASH_FUNCIONT_FOR_TYPE(signed char, size_t(v));
__HASH_FUNCIONT_FOR_TYPE(unsigned char, size_t(v));
__HASH_FUNCIONT_FOR_TYPE(signed short, size_t(v));
__HASH_FUNCIONT_FOR_TYPE(unsigned short, size_t(v));
__HASH_FUNCIONT_FOR_TYPE(signed int, size_t(v));
__HASH_FUNCIONT_FOR_TYPE(unsigned int, size_t(v));
__HASH_FUNCIONT_FOR_TYPE(signed long, size_t(v));
__HASH_FUNCIONT_FOR_TYPE(unsigned long, size_t(v));
__HASH_FUNCIONT_FOR_TYPE(signed long long, size_t(v));
__HASH_FUNCIONT_FOR_TYPE(unsigned long long, size_t(v));
__HASH_FUNCIONT_FOR_TYPE(std::string, NS_IMPL::__stl_hash_string(v.c_str(),v.length()));

#undef __HASH_FUNCIONT_FOR_TYPE
/*
//CSameType
template<class T1, class T2>
struct CSameType
{
    static const bool kResult = false;
};

template<class T>
struct CSameType<T, T>
{
    static const bool kResult = true;
};

//CSelect1st
template<class Pair>
struct CSelect1st : public std::unary_function<Pair, typename Pair::first_type>
{
    typename Pair::first_type & operator()(Pair & p) const{
        return p.first;
    }
    const typename Pair::first_type & operator()(const Pair & p) const {
        return p.first;
    }
};

*/

NS_SERVER_END

#endif
