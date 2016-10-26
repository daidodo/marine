#ifndef DOZERG_COROUTINE_IMPL_H_20150421
#define DOZERG_COROUTINE_IMPL_H_20150421

#include <ucontext.h>
#include <stdint.h>     // intptr_t
#include <cstring>      // memset
#include <cstdlib>      // malloc, free
#include "environment.hh"

NS_IMPL_BEGIN

class CUnwindStack{};

class CYieldBase;

class CCoroutineBase
{
    friend class CYieldBase;
    typedef void (*__CoroProc)(int, int);
    static const int kFlagsRunning = 1;
    static const int kFlagsUnwindStack = 2;
protected:
    CCoroutineBase(__CoroProc func, size_t stackSz)
        : flags_(0)
    {
        typedef void (*__VFunc)();
        ::getcontext(&cur_);
        cur_.uc_stack.ss_sp = std::malloc(stackSz);
        cur_.uc_stack.ss_size = stackSz;
        cur_.uc_link = &prev_;
        const intptr_t ptr = reinterpret_cast<intptr_t>(this);
        ::makecontext(&cur_, reinterpret_cast<__VFunc>(func), 2, ptr >> 32, ptr & 0xFFFFFFFF);
        std::memset(&prev_, 0, sizeof prev_);
    }
    ~CCoroutineBase(){
        if(running()){
            flags_ |= kFlagsUnwindStack;
            //resume();    //TODO
        }
        std::free(cur_.uc_stack.ss_sp);
    }
    void resume(){
        cur_.uc_link = &prev_;
        ::swapcontext(&prev_, &cur_);
    }
    template<class T>
    static T & cast(int high, int low){
        return *reinterpret_cast<T *>((intptr_t(high) << 32) | (intptr_t(low) & 0xFFFFFFFF));
    }
    template<class F, class Y>
    void run(F func, Y & yield){
        flags_ |= kFlagsRunning;
        try{
            func(yield);
        }catch(const CUnwindStack &){}
        flags_ &= ~kFlagsRunning;
    }
private:
    void yield(){
        ::swapcontext(&cur_, cur_.uc_link);
        unwind();
    }
    void yield(CCoroutineBase & coro){
        coro.cur_.uc_link = &cur_;
        ::swapcontext(&cur_, &coro.cur_);
        unwind();
    }
    bool running() const{return (0 != (flags_ & kFlagsRunning));}
    void unwind(){
        if(0 != (flags_ & kFlagsUnwindStack))
            throw CUnwindStack();
    }
    CCoroutineBase(const CCoroutineBase &);  //disable copy and assignment
    CCoroutineBase & operator =(const CCoroutineBase &);
    //fields
    ucontext cur_, prev_;
    int flags_;
};

class CYieldBase
{
    CCoroutineBase & coro_;
    CYieldBase(const CYieldBase &);  //disable copy and assignment
    CYieldBase & operator =(const CYieldBase &);
protected:
    explicit CYieldBase(CCoroutineBase & coro):coro_(coro){}
    void operator ()(){coro_.yield();}
    void operator ()(CCoroutineBase & coro){coro_.yield(coro);}
};

NS_IMPL_END

#endif

