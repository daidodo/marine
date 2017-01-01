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
 * @brief Lock-free atomic operations for integral types.
 * @author Zhao DAI
 */

#ifndef DOZERG_ATOMIC_SYNC_H_20130117
#define DOZERG_ATOMIC_SYNC_H_20130117

#include <cassert>
#include <cstddef>  //NULL
#include "impl/environment.hh"

NS_SERVER_BEGIN

/**
 * @brief GCC @c __sync_XXX based atomic operations for [integral types]
 * (http://en.cppreference.com/w/cpp/types/is_integral).
 * A common use of CAtomicSync is a high performance counter shared between threads or processes.
 * @tparam T An integral type, i.e. int, long, etc.
 */
template<typename T>
class CAtomicSync
{
    typedef CAtomicSync<T>  __Myt;
public:
    typedef T   value_type;
    /**
     * @brief Initialize with a value.
     * If no value is provided, default to 0.
     * @param c A value
     */
    explicit CAtomicSync(value_type c = value_type()):v_(c){}
    /**
     * @brief Initialize from another CAtomicSync.
     * @param c Another CAtomicSync
     */
    CAtomicSync(const __Myt & c):v_(c.load()){}
    /**
     * @brief Get value atomically.
     * @return Current value, i.e. @c *this
     */
    value_type load() const volatile{
        return const_cast<__Myt *>(this)->add_fetch(0);
    }
    /**
     * @brief Get value atomically.
     * @param v Pointer to a value
     * @return Current value of @c v, i.e. @c *v
     */
    static value_type load(const value_type * v){
        assert(v);
        return add_fetch(const_cast<value_type *>(v), 0);
    }
    /**
     * @brief Set value atomically.
     * This function performs `{*this = c}`.
     * @param c A Value
     */
    void store(value_type c) volatile{swap(c);}
    /**
     * @brief Set value atomically.
     * This function performs `{*v = c}`.
     * @param v Pointer to a value
     * @param c A Value
     */
    static void store(value_type * v, value_type c){
        assert(v);
        swap(v, c);
    }
    /**
     * @brief Get value and then ADD atomically.
     * This function performs `{T old = *this; *this += c; return old;}`
     * @param c A value
     * @return Current value @em before ADD'ing
     */
    value_type fetch_add(value_type c) volatile{return __sync_fetch_and_add(&v_, c);}
    /**
     * @brief Get value and then ADD atomically.
     * This function performs `{T old = *v; *v += c; return old;}`
     * @param v Pointer to a value
     * @param c A value
     * @return Value @em before ADD'ing
     */
    static value_type fetch_add(value_type * v, value_type c){
        assert(v);
        return __sync_fetch_and_add(v, c);
    }
    /**
     * @brief ADD and then get value atomically.
     * This function performs `{*this += c; return *this;}`
     * @param c A value
     * @return Current value @em after ADD'ing
     */
    value_type add_fetch(value_type c) volatile{return __sync_add_and_fetch(&v_, c);}
    /**
     * @brief ADD and then get value atomically.
     * This function performs `{*v += c; return *v;}`
     * @param v Pointer to a value
     * @param c A value
     * @return Value @em after ADD'ing
     */
    static value_type add_fetch(value_type * v, value_type c){
        assert(v);
        return __sync_add_and_fetch (v, c);
    }
    /**
     * @brief Get value and then SUBTRACT atomically.
     * This function performs `{T old = *this; *this -= c; return old;}`
     * @param c A value
     * @return Current value @em before SUBTRACT'ing
     */
    value_type fetch_sub(value_type c) volatile{return __sync_fetch_and_sub(&v_, c);}
    /**
     * @brief Get value and then SUBTRACT atomically.
     * This function performs `{T old = *v; *v -= c; return old;}`
     * @param v Pointer to a value
     * @param c A value
     * @return Value @em before SUBTRACT'ing
     */
    static value_type fetch_sub(value_type * v, value_type c){
        assert(v);
        return __sync_fetch_and_sub(v, c);
    }
    /**
     * @brief SUBTRACT and then get value atomically.
     * This function performs `{*this -= c; return *this;}`
     * @param c A value
     * @return Current value @em after SUBTRACT'ing
     */
    value_type sub_fetch(value_type c) volatile{return __sync_sub_and_fetch(&v_, c);}
    /**
     * @brief SUBTRACT and then get value atomically.
     * This function performs `{*v -= c; return *v;}`
     * @param v Pointer to a value
     * @param c A value
     * @return Value @em after SUBTRACT'ing
     */
    static value_type sub_fetch(value_type * v, value_type c){
        assert(v);
        return __sync_sub_and_fetch (v, c);
    }
    /**
     * @brief Get value and then OR atomically.
     * This function performs `{T old = *this; *this |= c; return old;}`
     * @param c A value
     * @return Current value @em before OR'ing
     */
    value_type fetch_or(value_type c) volatile{return __sync_fetch_and_or(&v_, c);}
    /**
     * @brief Get value and then OR atomically.
     * This function performs `{T old = *v; *v |= c; return old;}`
     * @param v Pointer to a value
     * @param c A value
     * @return Value @em before OR'ing
     */
    static value_type fetch_or(value_type * v, value_type c){
        assert(v);
        return __sync_fetch_and_or(v, c);
    }
    /**
     * @brief OR and then get value atomically.
     * This function performs `{*this |= c; return *this;}`
     * @param c A value
     * @return Current value @em after OR'ing
     */
    value_type or_fetch(value_type c) volatile{return __sync_or_and_fetch(&v_, c);}
    /**
     * @brief OR and then get value atomically.
     * This function performs `{*v |= c; return *v;}`
     * @param v Pointer to a value
     * @param c A value
     * @return Value @em after OR'ing
     */
    static value_type or_fetch(value_type * v, value_type c){
        assert(v);
        return __sync_or_and_fetch (v, c);
    }
    /**
     * @brief Get value and then AND atomically.
     * This function performs `{T old = *this; *this &= c; return old;}`
     * @param c A value
     * @return Current value @em before AND'ing
     */
    value_type fetch_and(value_type c) volatile{return __sync_fetch_and_and(&v_, c);}
    /**
     * @brief Get value and then AND atomically.
     * This function performs `{T old = *v; *v &= c; return old;}`
     * @param v Pointer to a value
     * @param c A value
     * @return Value @em before AND'ing
     */
    static value_type fetch_and(value_type * v, value_type c){
        assert(v);
        return __sync_fetch_and_and(v, c);
    }
    /**
     * @brief AND and then get value atomically.
     * This function performs `{*this &= c; return *this;}`
     * @param c A value
     * @return Current value @em after AND'ing
     */
    value_type and_fetch(value_type c) volatile{return __sync_and_and_fetch(&v_, c);}
    /**
     * @brief AND and then get value atomically.
     * This function performs `{*v &= c; return *v;}`
     * @param v Pointer to a value
     * @param c A value
     * @return Value @em after AND'ing
     */
    static value_type and_fetch(value_type * v, value_type c){
        assert(v);
        return __sync_and_and_fetch (v, c);
    }
    /**
     * @brief Get value and then XOR atomically.
     * This function performs `{T old = *this; *this ^= c; return old;}`
     * @param c A value
     * @return Current value @em before XOR'ing
     */
    value_type fetch_xor(value_type c) volatile{return __sync_fetch_and_xor(&v_, c);}
    /**
     * @brief Get value and then XOR atomically.
     * This function performs `{T old = *v; *v ^= c; return old;}`
     * @param v Pointer to a value
     * @param c A value
     * @return Value @em before XOR'ing
     */
    static value_type fetch_xor(value_type * v, value_type c){
        assert(v);
        return __sync_fetch_and_xor(v, c);
    }
    /**
     * @brief XOR and then get value atomically.
     * This function performs `{*this ^= c; return *this;}`
     * @param c A value
     * @return Current value @em after XOR'ing
     */
    value_type xor_fetch(value_type c) volatile{return __sync_xor_and_fetch(&v_, c);}
    /**
     * @brief XOR and then get value atomically.
     * This function performs `{*v ^= c; return *v;}`
     * @param v Pointer to a value
     * @param c A value
     * @return Value @em after XOR'ing
     */
    static value_type xor_fetch(value_type * v, value_type c){
        assert(v);
        return __sync_xor_and_fetch (v, c);
    }
    /**
     * @brief Get value and then set atomically.
     * This function performs `{T old = *this; *this = c; return old;}`
     * @param c A value
     * @return Value @em before setting
     */
    value_type swap(value_type c) volatile{return __sync_lock_test_and_set(&v_, c);}
    /**
     * @brief Get value and then set atomically.
     * This function performs `{T old = *v; *v = c; return old;}`
     * @param v Pointer to a value
     * @param c A value
     * @return Value @em before setting
     */
    static value_type swap(value_type * v, value_type c){
        assert(v);
        return __sync_lock_test_and_set(v, c);
    }
    /**
     * @brief Compare and set atomically.
     * This function performs:
     * @code{.cpp}
     * if(*this == expval){
     *     *oldval = *this; *this = newval; return true;
     * }else{
     *     *oldval = *this; return false;
     * }
     * @endcode
     * @param[in] expval Expected value
     * @param[in] newval New value to set
     * @param[out] oldval Pointer to a value to receive the old value @em before setting; Or @c
     *   NULL if not interested
     * @return @c true if `*this == expval` ; otherwise @c false
     * @note Setting operation will be performed if and only if compare operation returns @c true.
     */
    bool compare_swap(value_type expval, value_type newval, value_type * oldval = NULL) volatile{
        if(NULL == oldval)
            return __sync_bool_compare_and_swap(&v_, expval, newval);
        *oldval = __sync_val_compare_and_swap(&v_, expval, newval);
        return (*oldval == expval);
    }
    /**
     * @brief Compare and set atomically.
     * This function performs:
     * @code{.cpp}
     * if(*val == expval){
     *     *oldval = *val; *val = newval; return true;
     * }else{
     *     *oldval = *val; return false;
     * }
     * @endcode
     * @param[inout] val Pointer to a value
     * @param[in] expval Expected value
     * @param[in] newval New value to set
     * @param[out] oldval Pointer to a value to receive the old value @em before setting; Or @c
     *   NULL if not interested
     * @return @c true if `*this == expval` ; otherwise @c false
     * @note Setting operation will be performed if and only if compare operation returns @c true.
     */
    static bool compare_swap(value_type * val, value_type expval, value_type newval, value_type * oldval){
        assert(val);
        if(NULL == oldval)
            return __sync_bool_compare_and_swap(val, expval, newval);
        *oldval = __sync_val_compare_and_swap(val, expval, newval);
        return (*oldval == expval);
    }
    /**
     * @name Operators
     * Those operators just act as what you've already known, except that they are all atomic
     * operations.
     * @{ */
    value_type operator =(const __Myt & c) volatile{return (v_ = c.load());}
    value_type operator =(value_type c) volatile{return (v_ = c);}
    operator value_type() const volatile{return load();}
    value_type operator ++() volatile{return add_fetch(1);}
    value_type operator --() volatile{return sub_fetch(1);}
    value_type operator ++(int) volatile{return fetch_add(1);}
    value_type operator --(int) volatile{return fetch_sub(1);}
    value_type operator +=(value_type c) volatile{return add_fetch(c);}
    value_type operator -=(value_type c) volatile{return sub_fetch(c);}
    value_type operator |=(value_type c) volatile{return or_fetch(c);}
    value_type operator &=(value_type c) volatile{return and_fetch(c);}
    value_type operator ^=(value_type c) volatile{return xor_fetch(c);}
    /**  @} */
private:
    value_type v_;
};

NS_SERVER_END

#endif

