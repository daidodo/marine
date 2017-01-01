#ifndef DOZERG_FD_MAP_H_20071228
#define DOZERG_FD_MAP_H_20071228

/*
    �ṩfd�������ӳ��
    ��fd��Ϊ��������Ҫ���б߽�����Զ�����
        CFdMap      ��fdΪ�������������̲߳���ȫ
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
    //capacity: Ԥ��Ԫ�ظ������ɼ����ڴ�������
    explicit CFdMap(size_t capacity = 100){map_.reserve(capacity);}
    //����/��ȡԤ��Ԫ�ظ���
    size_t capacity() const{return map_.capacity();}
    void capacity(size_t sz){map_.reserve(sz);}
    //����/��ȡfd��Ӧ��Ԫ��
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
    //�������Ԫ��
    void clear(){map_.clear();}
private:
    container_type map_;
};

NS_SERVER_END

#endif
