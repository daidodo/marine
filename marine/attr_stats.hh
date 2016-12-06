/*
 *  Copyright (c) 2016 Zhao DAI <daidodo@gmail.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or any
 *  later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see accompanying file LICENSE.txt
 *  or <http://www.gnu.org/licenses/>.
 */

/// @file
/// Program statistics and profiling.
/// APIs for profiling program based on lock-free, thread-safe counters.
/// @n Each counter is called an @em attribute, identified by an positive integer. You can put as
/// many attributes in program as you want.

#ifndef DOZERG_ATTR_STATS_H_20130606
#define DOZERG_ATTR_STATS_H_20130606

#include "impl/attr_stats_impl.hh"

NS_SERVER_BEGIN

/// Increase an attribute atomically.
/// @details An example usage shows below:
/// @code
/// // init statistics APIs before use
/// ATTR_INIT();
/// ...
/// ...
/// // attributes 100, 101, 102 and 103 are used to count number
/// // of commands for Login, Logout, Query and others, respectively.
/// void process(Command & cmd){
///     switch(cmd.type){
///         case Login: ATTR_ADD(100, 1); break;
///         case Logout:ATTR_ADD(101, 1); break;
///         case Query: ATTR_ADD(102, 1); break;
///         default:    ATTR_ADD(103, 1); break;
///     }
/// }
/// @endcode
/// @param attr A positive number identifying the attribute
/// @param val A non-negative number to add to the attribute
/// @note For performance reason, @c attr must be a [constant expression]
/// (http://en.cppreference.com/w/cpp/language/constant_expression), i.e. can be evaluated at
/// compile time. And this is the recommended way of usage.
/// @n But if that is not the case, please use @ref ATTR_ADD_SLOW instead.
/// @sa ATTR_ADD_SLOW
/// @hideinitializer
#define ATTR_ADD(attr, val) \
    do{ \
        __UNUSED typedef NS_SERVER::NS_IMPL::__AttrDummy<attr> attr_must_be_constant;    \
        static uint64_t * __Attr = NULL;    \
        if(!__Attr){    \
            __Attr = NS_SERVER::NS_IMPL::CAttrStats::Inst().attrAdd(attr, val);    \
        }else   \
            NS_SERVER::NS_IMPL::CAttrStats::attrAdd(__Attr, val);   \
    }while(0)

/// Initialize statistics APIs.
/// This function must be called @em once only @em before any other statistics APIs.
/// @param capacity The maximum number of attributes in the program
/// @return @c true if succeeded; otherwise @c false
inline bool ATTR_INIT(size_t capacity = 1000)
{
    return NS_IMPL::CAttrStats::Inst().init(capacity);
}

/// Increase an attribute atomically.
/// @param attr A positive number identifying the attribute
/// @param val A non-negative number to add to the attribute
/// @return @c true if succeeded; otherwise @c false
/// @sa ATTR_ADD
inline bool ATTR_ADD_SLOW(int attr, uint64_t val)
{
    return (NULL != NS_IMPL::CAttrStats::Inst().attrAdd(attr, val));
}

/// Modify an attribute atomically.
/// @details An example usage shows below:
/// @code
/// // init statistics APIs before use
/// ATTR_INIT();
/// ...
/// // a message queue shared between threads or processes
/// MessageQueue queue;
/// ...
/// // attributes 104 is used to monitor the length of queue in a dedicated thread
/// void monitorThread(){
///     for(;;sleep(1)){
///         ATTR_SET(104, queue.length());
///     }
/// }
/// @endcode
/// @param attr A positive number identifying the attribute
/// @param val A non-negative number
/// @note For performance reason, @c attr must be a [constant expression]
/// (http://en.cppreference.com/w/cpp/language/constant_expression), i.e. can be evaluated at
/// compile time. And this is the recommended way of usage.
/// @n But if that is not the case, please use @ref ATTR_SET_SLOW instead.
/// @sa ATTR_SET_SLOW
/// @hideinitializer
#define ATTR_SET(attr, val) \
    do{ \
        __UNUSED typedef NS_SERVER::NS_IMPL::__AttrDummy<attr> attr_must_be_constant;    \
        static uint64_t * __Attr = NULL;    \
        if(!__Attr){    \
            __Attr = NS_SERVER::NS_IMPL::CAttrStats::Inst().attrSet(attr, val);    \
        }else   \
            NS_SERVER::NS_IMPL::CAttrStats::attrSet(__Attr, val);   \
    }while(0)

/// Get an attribute and then modify it atomically.
/// @param attr A positive number identifying the attribute
/// @param val A non-negative number
/// @param old Pointer to a variable to receive the value of attribute @em before modification; or
///   @c NULL if not interested
/// @note For performance reason, @c attr must be a [constant expression]
/// (http://en.cppreference.com/w/cpp/language/constant_expression), i.e. can be evaluated at
/// compile time. And this is the recommended way of usage.
/// @n But if that is not the case, please use @ref ATTR_SET_SLOW instead.
/// @sa ATTR_SET_SLOW
/// @hideinitializer
#define ATTR_SET_EX(attr, val, old) \
    do{ \
        __UNUSED typedef NS_SERVER::NS_IMPL::__AttrDummy<attr> attr_must_be_constant;    \
        static uint64_t * __Attr = NULL;    \
        if(!__Attr){    \
            __Attr = NS_SERVER::NS_IMPL::CAttrStats::Inst().attrSet(attr, val, old);    \
        }else   \
            NS_SERVER::NS_IMPL::CAttrStats::attrSet(__Attr, val, old);   \
    }while(0)

/// Get an attribute and then modify it atomically.
/// @param attr A positive number identifying the attribute
/// @param val A non-negative number
/// @param old Pointer to a variable to receive the value of attribute @em before modification; or
///   @c NULL if not interested
/// @sa ATTR_SET ATTR_SET_EX
inline bool ATTR_SET_SLOW(int attr, uint64_t val, uint64_t * old = NULL)
{
    return (NULL != NS_IMPL::CAttrStats::Inst().attrSet(attr, val, old));
}

/// Iterate and apply an operation to all attributes.
/// @param op Operation to apply to all attributes, should implement:
///   @code void operator ()(int attr, uint64_t value) const; @endcode
///   In which:
///     @li @c attr is a positive number identifying the attribute
///     @li @c value is the current value of the attribute
/// @note After the operation, each attribute will be reset to 0 for a new turn of counting.
template<class Op>
inline void ATTR_ITERATE(Op op)
{
    return NS_IMPL::CAttrStats::Inst().iterate(op);
}

NS_SERVER_END

#endif

