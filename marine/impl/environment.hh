#ifndef DOZERG_ENVIRONMENT_H_20111231
#define DOZERG_ENVIRONMENT_H_20111231

//project namespaces
#define NS_SERVER       marine
#define NS_IMPL         marine_impl

//external lib namespace
#define NS_EXTERN_LIB   __marine_external_lib

#define NAMESAPCE_BEGIN(name)   namespace name{
#define NAMESAPCE_END           }

#define NS_SERVER_BEGIN     NAMESAPCE_BEGIN(NS_SERVER)
#define NS_SERVER_END       NAMESAPCE_END
#define NS_IMPL_BEGIN       NS_SERVER_BEGIN NAMESAPCE_BEGIN(NS_IMPL)
#define NS_IMPL_END         NAMESAPCE_END   NS_SERVER_END
#define NS_EXTLIB_BEGIN     NS_SERVER_BEGIN NAMESAPCE_BEGIN(NS_EXTERN_LIB)
#define NS_EXTLIB_END       NAMESAPCE_END   NS_SERVER_END

//system support

#if (_POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600)
#   define __API_HAS_SEM_TIMEWAIT       //support sem_timedwait()
#   define __API_HAS_PTHREAD_ATTR_SETSTACK  //support pthread_attr_setstack()
#endif

//support semtimedop
#ifdef _GNU_SOURCE
#   define __API_HAS_SEMTIMEDOP
#endif

#if __GNUC__ < 4
#   define __builtin_offsetof   offsetof
#endif

//has __sync_xxx_and_xxx APIs
#if defined( __GNUC__ ) && ( __GNUC__ * 100 + __GNUC_MINOR__ >= 401 )
//#   define __HAS_GCC_ATOMIC_BUILTIN
#   define __HAS_GCC_SYNC_BUILTIN
#endif

//use clock_gettime or gettimeofday
#define __USE_CLOCK_GETTIME

//has lseek64
#ifdef _LARGEFILE64_SOURCE
#   define __HAS_LSEEK64
#endif

//has ftruncate, readlink
#if defined( _BSD_SOURCE ) || _XOPEN_SOURCE >= 500 || _POSIX_C_SOURCE >= 200112L
#   define __HAS_FTRUNCATE
#   define __HAS_READLINK
#endif

//设置std::showpos标志后，oss<<0输出"+0"还是"0"
#if (__GNUC__ <= 4 && __GNUC_MINOR__ <= 1)
#   define __0_NO_POSITIVE  //老版本GCC输出"0"
#endif

#if defined( __GNUC__ ) && ( __GNUC__ > 3 || ( __GNUC__ == 3 && __GNUC_MINOR__ >= 1 ))
#   define __ALWAYS_INLINE __attribute__ ((always_inline))
#else
#   define __ALWAYS_INLINE
#endif

#define __UNUSED __attribute__((unused))

#endif

