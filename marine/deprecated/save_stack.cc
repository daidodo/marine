#ifdef __SAVE_STACK

#include <cstdlib>
#include <pthread.h>
#include "template.hh"
#include "impl/save_stack_impl.hh"

NS_IMPL_BEGIN

static void __initKey() __attribute__ ((no_instrument_function));

static bool __pushAddr(void * addr) __attribute__ ((no_instrument_function));

static bool __popAddr(void * addr) __attribute__ ((no_instrument_function));

static pthread_key_t key;
static pthread_once_t once = PTHREAD_ONCE_INIT;
static volatile bool shutdown = false;

static void __initKey()
{
    ::pthread_key_create(&key, ::free);
}

#define __GET_STACK(stack)  do{ \
    stack = static_cast<CAddrStack *>(::pthread_getspecific(key)); \
    if(NULL == stack){  \
        stack = static_cast<CAddrStack *>(::malloc(sizeof(CAddrStack)));    \
        if(NULL == stack)   \
            return false;   \
        stack->top_ = 0;    \
        if(0 != ::pthread_setspecific(key, stack)){ \
            ::free(stack);  \
            return false;   \
        }   \
    }   \
}while(0)

static bool __pushAddr(void * addr)
{
    //init key
    if(0 != ::pthread_once(&once, __initKey))
        return false;
    //get stack
    CAddrStack * stack = NULL;
    __GET_STACK(stack);
    //operate
    if(stack->top_ >= ARRAY_SIZE(stack->addr_))
        return false;   //full
    ++stack->top_;
    stack->addr_[ARRAY_SIZE(stack->addr_) - stack->top_] = addr;
    return true;
}

static bool __popAddr(void * addr)
{
    //init key
    if(0 != ::pthread_once(&once, __initKey))
        return false;
    //get stack
    CAddrStack * stack = NULL;
    __GET_STACK(stack);
    //operate
    if(0 == stack->top_
            || stack->addr_[ARRAY_SIZE(stack->addr_) - stack->top_] != addr)
        return false;
    --stack->top_;
    return true;
}

bool __getStack(CAddrStack * s)
{
    if(NULL == s)
        return false;
    //init key
    if(0 != ::pthread_once(&once, __initKey))
        return false;
    //get stack
    CAddrStack * stack = static_cast<CAddrStack *>(::pthread_getspecific(key));
    if(NULL == stack)
        return false;
    //operate
    ::memcpy(s->addr_,
           stack->addr_ + ARRAY_SIZE(stack->addr_) - stack->top_,
           stack->top_ * sizeof(void *));
    s->top_ = stack->top_;
    return true;
}

NS_IMPL_END

#ifdef __cplusplus
extern "C" {
#endif

    void __cyg_profile_func_enter(void *, void *) __attribute__ ((no_instrument_function));

    void __cyg_profile_func_exit(void *, void *) __attribute__ ((no_instrument_function));

    void __cyg_profile_func_enter(void * func, void * call)
    {
        NS_SERVER::NS_IMPL::__pushAddr(func);
    }

    void __cyg_profile_func_exit(void * func, void * call)
    {
        NS_SERVER::NS_IMPL::__popAddr(func);
    }

#ifdef __cplusplus
}
#endif

#undef __GET_STACK

#endif
