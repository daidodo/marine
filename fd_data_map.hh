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
 * @brief Thread-safe container for fd (file descriptor) related data.
 * @author Zhao DAI
 */

#ifndef DOZERG_FD_DATA_MAP_H_20130228
#define DOZERG_FD_DATA_MAP_H_20130228

#include <vector>
#include <cassert>
#include "mutex.hh"
#include "shared_ptr.hh"
#include "fd_map.hh"

NS_SERVER_BEGIN

/**
 * @brief Thread-safe hash table for fd (file descriptor) related data.
 * Key @em MUST be file descriptors, which are non-negative and of type @c int. In fact the file
 * descriptor acts as an index to an underlying @c vector of values. Because the number of files a
 * process can open is limited, e.g. 1024 (it is modifiable), there's no chance for the underlying
 * @c vector to grow unexpectedly. So do @em NOT use CFdDataMap as a generic @c int to value hash
 * table.
 * @n Value can be of any type, even non-copyable types. Usually you want to keep a session for each
 * socket fd, and the session is non-copyable. CFdDataMap uses smart pointer to hold a value, there
 * are several advantages. Firstly, it avoids copying issue as mentioned; secondly, it simplifies
 * lifetime control for each value among multiple threads; And finally, it is efficient for the
 * underlying @c vector to expand.
 * @n CFdDataMap is thread safe. You can specify any lock type you want, as long as it cooperates with
 * CGuard.
 * @tparam T Value type
 * @tparam LockT Lock type, default to CSpinLock
 */
template<class T, class LockT = CSpinLock>
class CFdDataMap
{
public:
    typedef T                       value_type;
    typedef CSharedPtr<value_type>  pointer;
private:
    typedef CFdMap<pointer>     __Map;
    typedef LockT               lock_type;
    typedef CGuard<lock_type>   guard_type;
public:
    /**
     * @brief Initialize this object.
     * @c capacity is used as a hint to the number of key/value pairs this object wants to hold. But
     * CFdDataMap is free to expand as needed.
     * @param capacity Initial reserved room for key/value pairs
     */
    explicit CFdDataMap(size_t capacity = 100)
        : map_(capacity)
        , sz_(0)
    {}
    /**
     * @brief Get number of key/value pairs.
     * @return Number of key/value pairs
     */
    size_t size() const{return sz_;}
    /**
     * @brief Get current capacity.
     * @return Current capacity
     */
    size_t capacity() const{
        guard_type g(lock_);
        return map_.capacity();
    }
    /**
     * @brief Set current capacity.
     * @param c New capacity
     */
    void capacity(size_t c){
        guard_type g(lock_);
        map_.capacity(c);
    }
    /**
     * @brief Set value for an fd.
     * @param[in] fd A file descriptor
     * @param[in] data New value for @c fd
     * @param[out] old A smart pointer object to obtain old value for @c fd
     */
    void setData(int fd, const pointer & data, pointer * old = NULL){
        if(fd < 0)
            return;
        guard_type g(lock_);
        setAux(fd, data, old);
    }
    /**
     * @brief Get value for an fd.
     * @param fd A file descriptor
     * @return A smart pointer object hold the value for @c fd
     */
    pointer getData(int fd) const{
        if(fd < 0)
            return pointer();
        guard_type g(lock_);
        return map_[fd];
    }
    /**
     * @brief Get value for an fd.
     * @param[in] fd A file descriptor
     * @param[out] data A smart pointer object to obtain the value for @c fd
     */
    void getData(int fd, pointer * data) const{
        if(NULL == data || fd < 0)
            return;
        guard_type g(lock_);
        *data = map_[fd];
    }
    /**
     * @brief Get values for some file descriptors.
     * This function saves many lock/unlock operations.
     * @n Example code:
     * @code{.cpp}
     * CFdDataMap<Data> fdmap;
     *
     * vector<int> fds;
     * vector<CFdDataMap<Data>::pointer> values(fds.size());
     *
     * fdmap.getData(fds.begin(), fds.end(), values.begin());
     * @endcode
     * @param[in] first Begin iterator for a collection of file descriptors
     * @param[in] last End iterator for a collection of file descriptors
     * @param[out] dstFirst Begin iterator for a collection of smart pointers to receive the values
     * of file descriptors
     * @note The size and validation of destination collection is guaranteed by the user.
     */
    template<class ForwardIter, class OutputIter>
    void getData(ForwardIter first, ForwardIter last, OutputIter dstFirst) const{
        guard_type g(lock_);
        for(int fd = -1;first != last;++first, ++dstFirst){
            fd = *first;
            if(fd >= 0)
                *dstFirst = map_[fd];
        }
    }
    /**
     * @brief Remove value for an fd.
     * @param[in] fd A file descriptor
     * @param[out] old A smart pointer object to obtain old value for @c fd, or @c NULL if not
     * interested
     */
    void clearData(int fd, pointer * old = NULL){
        if(fd < 0)
            return;
        guard_type g(lock_);
        setAux(fd, pointer(), old);
    }
    /**
     * @brief Remove values for some file descriptors.
     * @param[in] first Begin iterator for a collection of file descriptors
     * @param[in] last End iterator for a collection of file descriptors
     */
    template<class ForwardIter>
    void clearData(ForwardIter first, ForwardIter last){
        guard_type g(lock_);
        for(int fd = -1;first != last;++first){
            fd = *first;
            if(fd >= 0)
                setAux(fd, pointer(), NULL);
        }
    }
    /**
     * @brief Remove values for some file descriptors.
     * @param[in] first Begin iterator for a collection of file descriptors
     * @param[in] last End iterator for a collection of file descriptors
     * @param[out] dstFirst Begin iterator for a collection of smart pointers to receive old values
     * @note The size and validation of destination collection is guaranteed by the user.
     */
    template<class ForwardIter, class OutputIter>
    void clearData(ForwardIter first, ForwardIter last, OutputIter dstFirst){
        guard_type g(lock_);
        for(int fd = -1;first != last;++first, ++dstFirst){
            fd = *first;
            if(fd >= 0)
                setAux(fd, pointer(), &*dstFirst);
        }
    }
    /**
     * @brief Clear all key/value pairs.
     */
    void clear(){
        guard_type g(lock_);
        map_.clear();
        sz_ = 0;
    }
private:
    //set *old = map_[fd], then set map_[fd] = data
    //Note: old and &data may be equal
    void setAux(int fd, const pointer & data, pointer * old){
        assert(fd >= 0);
        if(fd < int(map_.capacity())){
            pointer & cur = map_[fd];
            sz_ += (data ? 1 : 0) - (cur ? 1 : 0);
            if(NULL != old)
                old->swap(cur);
            if(&data != old)
                cur = data;
        }else{
            if(data){
                map_[fd] = data;
                ++sz_;
            }
            if(NULL != old)
                old->reset();
        }
    }
    //fields:
    __Map map_;
    lock_type lock_;
    volatile size_t sz_;
};

NS_SERVER_END

#endif
