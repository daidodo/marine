#ifndef DOZERG_SINGLE_LIST_IMPL_H_20080226
#define DOZERG_SINGLE_LIST_IMPL_H_20080226

/*
    CSingleList的内部实现
        __single_list_node
        __slist_const_iterator
        __slist_iterator
//*/

#include <cstddef>      //std::ptrdiff_t
#include <iterator>     //std::forward_iterator_tag

NS_SERVER_BEGIN

template<class T,class Alloc>
class CSingleList;

NS_SERVER_END

NS_IMPL_BEGIN

template<class T>
struct __single_list_node
{
    typedef __single_list_node<T>   __Myt;
    __Myt *     next_;
    union{
        T *     pdata_;
        __Myt * tail_;    //头节点的这个域指向最后一个元素节点(不是end())
    };
};

//const_iterator
template<class T>
class __slist_const_iterator
{
    typedef __slist_const_iterator<T>   __Myt;
protected:
    typedef __single_list_node<T>       __node;
    typedef __node *                    __node_ptr;
public:
    typedef std::forward_iterator_tag   iterator_category;
    typedef T                           value_type;
    typedef const T *                   pointer;
    typedef const T &                   reference;
    typedef std::ptrdiff_t              difference_type;
public:
    __slist_const_iterator(__node_ptr p = 0):ptr_(p){}
    reference operator *() const{return *ptr_->pdata_;}
    pointer operator ->() const{return &operator *();}
    __Myt & operator ++(){
        ptr_ = ptr_->next_;
        return *this;
    }
    __Myt operator ++(int){
        __Myt ret(*this);
        operator ++();
        return ret;
    }
    bool operator ==(const __Myt & other) const{return ptr_ == other.ptr_;}
    bool operator !=(const __Myt & other) const{return !operator ==(other);}
protected:
    __node_ptr ptr_;
};

//iterator
template<class T,class Alloc>
class __slist_iterator:public __slist_const_iterator<T>
{
    friend class CSingleList<T,Alloc>;
    typedef __slist_const_iterator<T>       __MyBase;
    typedef __slist_iterator<T,Alloc>       __Myt;
    typedef typename __MyBase::__node       __node;
    typedef typename __MyBase::__node_ptr   __node_ptr;
public:
    typedef std::forward_iterator_tag           iterator_category;
    typedef T                                   value_type;
    typedef T *                                 pointer;
    typedef T &                                 reference;
    typedef typename __MyBase::difference_type  difference_type;
    __slist_iterator(__node_ptr p = 0):__MyBase(p){}
    reference operator *() const{return *__MyBase::ptr_->pdata_;}
    pointer operator ->() const{return &operator *();}
    __Myt & operator ++(){
        __MyBase::operator ++();
        return *this;
    }
    __Myt operator ++(int){
        __Myt ret(*this);
        operator ++();
        return ret;
    }
};

NS_IMPL_END

#endif
