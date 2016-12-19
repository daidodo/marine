/*
 * Copyright (c) 2016 Zhao DAI <daidodo@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or any
 * later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see accompanying file LICENSE.txt
 * or <http://www.gnu.org/licenses/>.
 */

/**
 * @file
 * @brief A lightweight implementation of [coroutine](https://en.wikipedia.org/wiki/Coroutine).
 * @warning This file is still under development.
 * @author Zhao DAI
 */

#ifndef DOZERG_COROUTINE_H_20150420
#define DOZERG_COROUTINE_H_20150420

#ifndef UNIT_TEST
#   warning "This file is still under development."
#endif

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

