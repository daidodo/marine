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

/*
    提供fd到对象的映射
    以fd作为索引，主要进行边界检查和自动伸缩
        CFdDataMap  以fd为索引的容器，线程安全
                    内部存储CSharedPtr<T>，而不是数据指针:w
    History
        20080604    CFdSockMap增加对CSharedPtr的支持，主要是putSock的处理不同
        20080910    CFdSockMap增加sz_字段，Size()函数的意义改为在线fd的个数
                    GetSock()函数内的null_ptr对象类型加上const限定
        20080911    CFdSockMap增加CloseSock()函数，避免关闭大量连接时频繁加锁
                    CFdSockMap增加SetSock()函数的重载，通过一次操作既更换连接对象，又得到旧的连接对象
                    CFdSockMap修改原SetSock()函数，增加del参数，分离“关闭连接”与“释放对象”操作
        20080917    给CFdMap::operator [] const增加DEFAULT变量
                    CFdMap和CFdSockMap增加模板参数Container，决定内部容器类型
                    CFdMap增加reserve()函数
        20080920    CFdSockMap使用模板参数决定锁类型
        20081010    CFdSockMap::GetSock()函数返回值改为__SockPtr对象，因为返回const引用会造成无锁访问map_[fd]
                    增加CFdSockMap::GetSock(int, __SockPtr &) const重载，减少一个临时对象
        20081013    增加CFdSockMap::GetSock(ForwardIter, ForwardIter, OutputIter) const重载，批量获取连接对象
        20130228    重构CFdSockMap，不限于存储Socket对象，并改名为CFdDataMap
//*/

#include <vector>
#include <cassert>
#include "mutex.hh"
#include "shared_ptr.hh"
#include "fd_map.hh"

NS_SERVER_BEGIN

/**
 * @brief
 * @tparam T
 * @tparam LockT
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
    //ctor
    //capacity: 预留(fd -> data)个数，可减少内存分配次数
    explicit CFdDataMap(size_t capacity = 100)
        : map_(capacity)
        , sz_(0)
    {}
    //获取当前有效的(fd -> data)个数
    size_t size() const{return sz_;}
    //修改/获取当前可容纳的(fd -> data)个数
    size_t capacity() const{
        guard_type g(lock_);
        return map_.capacity();
    }
    void capacity(size_t c){
        guard_type g(lock_);
        map_.capacity(c);
    }
    //设置fd对应的数据
    //old: 返回fd之前对应的数据
    void setData(int fd, const pointer & data, pointer * old = NULL){
        if(fd < 0)
            return;
        guard_type g(lock_);
        setAux(fd, data, old);
    }
    //获取fd对应的数据
    pointer getData(int fd) const{
        if(fd < 0)
            return pointer();
        guard_type g(lock_);
        return map_[fd];
    }
    //获取fd对应的数据，优化版
    void getData(int fd, pointer * data) const{
        if(NULL == data || fd < 0)
            return;
        guard_type g(lock_);
        *data = map_[fd];
    }
    //批量获取数据
    //first: fd序列的起始迭代器
    //last: fd序列的结束迭代器
    //dstFirst: 输出数据序列的起始迭代器
    //注意: 调用者负责保证目的地dstFirst的有效性和范围
    template<class ForwardIter, class OutputIter>
    void getData(ForwardIter first, ForwardIter last, OutputIter dstFirst) const{
        guard_type g(lock_);
        for(int fd = -1;first != last;++first, ++dstFirst){
            fd = *first;
            if(fd >= 0)
                *dstFirst = map_[fd];
        }
    }
    //移除fd对应的数据
    //old: 返回fd之前对应的数据
    void clearData(int fd, pointer * old = NULL){
        if(fd < 0)
            return;
        guard_type g(lock_);
        setAux(fd, pointer(), old);
    }
    //批量移除数据
    //first: fd序列的起始迭代器
    //last: fd序列的结束迭代器
    //oldFirst: 数据序列的起始迭代器，返回之前的数据
    //注意: 调用者负责保证目的地dstFirst的有效性和范围
    template<class ForwardIter>
    void clearData(ForwardIter first, ForwardIter last){
        guard_type g(lock_);
        for(int fd = -1;first != last;++first){
            fd = *first;
            if(fd >= 0)
                setAux(fd, pointer(), NULL);
        }
    }
    template<class ForwardIter, class OutputIter>
    void clearData(ForwardIter first, ForwardIter last, OutputIter dstFirst){
        guard_type g(lock_);
        for(int fd = -1;first != last;++first, ++dstFirst){
            fd = *first;
            if(fd >= 0)
                setAux(fd, pointer(), &*dstFirst);
        }
    }
    //移除所有数据
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
