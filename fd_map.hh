#ifndef DOZERG_FD_MAP_H_20071228
#define DOZERG_FD_MAP_H_20071228

/*
    提供fd到对象的映射
    以fd作为索引，主要进行边界检查和自动伸缩
        CFdMap      以fd为索引的容器，线程不安全
//*/

#include <cstddef>
#include <vector>
#include <cassert>
#include "impl/environment.hh"

NS_SERVER_BEGIN

template<class T, class Container = std::vector<T> >
struct CFdMap
{
    typedef Container   container_type;
    typedef T           value_type;
    typedef T &         reference;
    typedef const T &   const_reference;
    //ctor
    //capacity: 预留元素个数，可减少内存分配次数
    explicit CFdMap(size_t capacity = 100){map_.reserve(capacity);}
    //设置/获取预留元素个数
    size_t capacity() const{return map_.capacity();}
    void capacity(size_t sz){map_.reserve(sz);}
    //设置/获取fd对应的元素
    reference operator [](int fd){
        assert(fd >= 0);
        if(size_t(fd) >= map_.size())
            map_.resize(fd + 1);
        return map_[fd];
    }
    const_reference operator [](int fd) const{
        static const value_type kDef = value_type();    //use value_type() to fix compile error for POD
        if(fd >= 0 && size_t(fd) < map_.size())
            return map_[fd];
        return kDef;
    }
    //清空所有元素
    void clear(){map_.clear();}
private:
    container_type map_;
};

NS_SERVER_END

#endif
