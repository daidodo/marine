#ifndef DOZERG_COROUTINE_H_20150420
#define DOZERG_COROUTINE_H_20150420

#include <signal.h>     // MINSIGSTKSZ
#include "impl/coroutine_impl.hh"

NS_SERVER_BEGIN

template<class T>
class CCoroutine : public NS_IMPL::CCoroutineBase
{
    //typedefs
    typedef NS_IMPL::CCoroutineBase __MyBase;
public:
    class yield_type : public NS_IMPL::CYieldBase {
        friend class CCoroutine<T>;
        //typedefs
        typedef NS_IMPL::CYieldBase __MyBase;
    public:
        typedef T               arg_type;
        typedef CCoroutine<T>   coro_type;
        //functions
        arg_type get() const{return arg_;}
        using __MyBase::operator ();
        template<class U>
        void operator ()(CCoroutine<U> & coro, typename CCoroutine<U>::arg_type arg){
            coro.set(arg);
            __MyBase::operator ()(coro);
        }
    private:
        explicit yield_type(coro_type & coro):__MyBase(coro){}
        void set(arg_type arg){arg_ = arg;}
        //fields
        arg_type arg_;
    };
    typedef typename yield_type::arg_type arg_type;
    typedef void (* func_type)(yield_type &);
    //functions
    explicit CCoroutine(func_type func, size_t stackSz = MINSIGSTKSZ)
        : __MyBase(CoroProc, stackSz), y_(*this), func_(func)
    {}
    void operator ()(arg_type arg){
        set(arg);
        __MyBase::resume();
    }
    void set(arg_type arg){y_.set(arg);}
private:
    static void CoroProc(int high, int low){
        CCoroutine & coro = __MyBase::cast<CCoroutine>(high, low);
        coro.run(coro.func_, coro.y_);
    }
    //fields
    yield_type y_;
    func_type func_;
};

template<class T>
class CCoroutine<T &> : public NS_IMPL::CCoroutineBase
{
    //typedefs
    typedef NS_IMPL::CCoroutineBase __MyBase;
public:
    class yield_type : public NS_IMPL::CYieldBase {
        friend class CCoroutine<T &>;
        //typedefs
        typedef NS_IMPL::CYieldBase __MyBase;
    public:
        typedef T &             arg_type;
        typedef CCoroutine<T &> coro_type;
        //functions
        arg_type get() const{return *arg_;}
        using __MyBase::operator ();
        template<class U>
        void operator ()(CCoroutine<U> & coro, typename CCoroutine<U>::arg_type arg){
            coro.set(arg);
            __MyBase::operator ()(coro);
        }
    private:
        explicit yield_type(coro_type & coro):__MyBase(coro){}
        void set(T * arg){arg_ = arg;}
        //fields
        T * arg_;
    };
    typedef typename yield_type::arg_type arg_type;
    typedef void (* func_type)(yield_type &);
private:
    //functions
    static void CoroProc(int high, int low){
        CCoroutine & coro = __MyBase::cast<CCoroutine>(high, low);
        coro.run(coro.func_, coro.y_);
    }
public:
    explicit CCoroutine(func_type func, size_t stackSz = MINSIGSTKSZ)
        : __MyBase(CoroProc, stackSz)
        , y_(*this)
        , func_(func)
    {}
    void operator ()(arg_type arg){
        set(arg);
        __MyBase::resume();
    }
    void set(arg_type arg){y_.set(&arg);}
private:
    //fields
    yield_type y_;
    func_type func_;
};

template<>
class CCoroutine<void> : public NS_IMPL::CCoroutineBase
{
    //typedefs
    typedef NS_IMPL::CCoroutineBase __MyBase;
public:
    class yield_type : public NS_IMPL::CYieldBase {
        friend class CCoroutine<void>;
        //typedefs
        typedef NS_IMPL::CYieldBase __MyBase;
    public:
        typedef CCoroutine<void>   coro_type;
        //functions
        using __MyBase::operator ();
        template<class U>
        void operator ()(CCoroutine<U> & coro, typename CCoroutine<U>::arg_type arg){
            coro.set(arg);
            __MyBase::operator ()(coro);
        }
    private:
        explicit yield_type(coro_type & coro):__MyBase(coro){}
    };
    typedef void (* func_type)(yield_type &);
private:
    //functions
    static void CoroProc(int high, int low){
        CCoroutine & coro = __MyBase::cast<CCoroutine>(high, low);
        coro.run(coro.func_, coro.y_);
    }
public:
    explicit CCoroutine(func_type func, size_t stackSz = MINSIGSTKSZ)
        : __MyBase(CoroProc, stackSz)
        , y_(*this)
        , func_(func)
    {}
    void operator ()(){__MyBase::resume();}
private:
    //fields
    yield_type y_;
    func_type func_;
};

NS_SERVER_END

#endif

