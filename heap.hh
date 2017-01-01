#ifndef DOZERG_HEAP_H_20080102
#define DOZERG_HEAP_H_20080102

/*
    STL风格的堆
        CHeap
        CFixedHeap  固定大小堆
    History:
        20071006    为CHeap和CFixedHeap增加swap函数
                    去掉CFixedHeap的less_和equal_成员变量
                    CFixedHeap::push_unique函数里使用迭代器，而不是下标
//*/

#include <vector>
#include <algorithm>    //std::less, std::make_heap, ...
#include <functional>   //std::less, std::equal
#include "impl/environment.hh"

NS_SERVER_BEGIN

//CHeap
template<
    class T,
    class Container = std::vector<T>,
    class BinaryPredicate = std::less<T>
>class CHeap
{
    typedef CHeap<T, Container, BinaryPredicate>        __Myt;
public:
    typedef BinaryPredicate                             less_comp;
    typedef Container                                   container_type;
    typedef typename container_type::value_type         value_type;
    typedef typename container_type::size_type          size_type;
protected:
    typedef typename container_type::reference          reference;
    typedef typename container_type::const_reference    const_reference;
public:
    explicit CHeap(less_comp = less_comp()){}
    explicit CHeap(const container_type & cont, less_comp = less_comp())
        : cont_(cont)
    {
        std::make_heap(cont_.begin(), cont_.end(), less_comp());
    }
    template<class InputIterator>
    CHeap(InputIterator first, InputIterator last, less_comp = less_comp())
        : cont_(first, last)
    {
        std::make_heap(cont_.begin(), cont_.end(), less_comp());
    }
    template<class InputIterator>
    CHeap(const container_type & cont, InputIterator first, InputIterator last, less_comp = less_comp())
        : cont_(cont)
    {
        cont_.insert(cont_.end(), first, last);
        std::make_heap(cont_.begin(), cont_.end(), less_comp());
    }
    bool empty() const          {return cont_.empty();}
    size_type size() const      {return cont_.size();}
    reference top()             {return cont_.front();}
    const_reference top() const {return cont_.front();}
    void push(const_reference value){
        cont_.push_back(value);
        std::push_heap(cont_.begin(), cont_.end(), less_comp());
    }
    void pop(){
        std::pop_heap(cont_.begin(), cont_.end(), less_comp());
        cont_.pop_back();
    }
    void swap(__Myt & a) throw() {
        std::swap(cont_, a.cont_);
    }
private:
    container_type  cont_;
};

//CFixedHeap
template<
    class T,
    class Container = std::vector<T>,
    class BinaryPredicateLess = std::less<T>,       //小于比较算符
    class BinaryPredicateEqual = std::equal_to<T>   //等于比较算符
>class CFixedHeap
{
    typedef CFixedHeap<T, Container, BinaryPredicateLess, BinaryPredicateEqual> __Myt;
public:
    typedef BinaryPredicateLess                         less_comp;
    typedef BinaryPredicateEqual                        equal_comp;
    typedef Container                                   container_type;
    typedef typename container_type::value_type         value_type;
    typedef typename container_type::size_type          size_type;
protected:
    typedef typename container_type::reference          reference;
    typedef typename container_type::const_reference    const_reference;
public:
    explicit CFixedHeap(size_t max_size = 10, less_comp = less_comp(), equal_comp = equal_comp())
        : max_size_(max_size)
    {}
    CFixedHeap(size_t max_size, const container_type & cont, less_comp = less_comp(), equal_comp = equal_comp())
        : max_size_(max_size)
    {
        fromRange(cont_.begin(), cont_.end(), cont_.size());
    }
    template<class InputIterator>
    CFixedHeap(InputIterator first, InputIterator last, size_t max_size, less_comp = less_comp(), equal_comp = equal_comp())
        : max_size_(max_size)
    {
        fromRange(first, last, 0);
    }
    bool empty() const          {return cont_.empty();}
    size_type size() const      {return cont_.size();}
    size_type max_size() const  {return max_size_;}
    reference top()             {return cont_.front();}
    const_reference top() const {return cont_.front();}
    void set_max_size(size_t maxsz){max_size_ = maxsz;}
    void push(const_reference value){
        if(size() < max_size_){
            cont_.push_back(value);
            std::push_heap(cont_.begin(), cont_.end(), less_comp());
        }else if(max_size_ > 0 && less_comp()(value, top())){
            top() = value;
            std::make_heap(cont_.begin(), cont_.end(), less_comp());
        }
    }
    void pop(){
        std::pop_heap(cont_.begin(), cont_.end(), less_comp());
        cont_.pop_back();
    }
    //保证value不重复
    void push_unique(const_reference value){
        typedef typename container_type::const_iterator __Iter;
        equal_comp eq;
        __Iter i = cont_.begin();
        for(;i != cont_.end() && !eq(value, *i);++i);
        if(i != cont_.end())
            std::make_heap(cont_.begin(), cont_.end(), less_comp());
        else
            push(value);
    }
    void swap(__Myt & a) throw() {
        std::swap(max_size_, a.max_size_);
        std::swap(cont_, a.cont_);
    }
private:
    template<class InputIterator>
    void fromRange(InputIterator first, InputIterator last, size_t sz){
        if(sz && sz <= max_size_){
            cont_.assign(first, last);
            std::make_heap(cont_.first(), cont_.last(), less_comp());
        }else{
            for(;first != last;++first)
                push(*first);
        }
    }
    //members:
    size_t          max_size_;
    container_type  cont_;
};

template<class T, class C, class P>
inline void swap(CHeap<T, C, P> & a, CHeap<T, C, P> & b)
{
    a.swap(b);
}

template<class T, class C, class P1, class P2>
inline void swap(CFixedHeap<T, C, P1, P2> & a, CFixedHeap<T, C, P1, P2> & b)
{
    a.swap(b);
}

NS_SERVER_END

#endif
