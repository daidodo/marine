#ifndef DOZERG_RING_BUFFER_H_20090218
#define DOZERG_RING_BUFFER_H_20090218

/*
    环形缓冲区，可作为“单生产者-单消费者”模型中的消息队列，无需加锁
        CRingBuf
//*/

#include <cassert>
#include <cstddef>  //std::ptrdiff_t
#include "tools/memory.hh"

NS_SERVER_BEGIN

template<class T, class Alloc = std::allocator<T> >
class CRingBuf
{
    //typedefs:
    typedef CRingBuf<T>     __Myt;
public:
    typedef T               value_type;
    typedef T &             reference;
    typedef const T &       const_reference;
    typedef T *             pointer;
    typedef const T *       const_pointer;
    typedef size_t          size_type;
    typedef std::ptrdiff_t  difference_type;
    typedef typename Alloc::
        template rebind<T>::other   allocator_type;
    //functions:
    explicit CRingBuf(size_type capacity)
        : buf_(NULL)
        , capa_(capacity + 1)
        , head_(0)
        , tail_(0)
    {
        init();
    }
    ~CRingBuf(){uninit();}
    size_type capacity() const{return capa_ - 1;}
    bool empty() const{return head_ == tail_;}
    size_type size() const{return (tail_ + capa_ - head_) % capa_;}
    bool push(const_reference v){
        assert(head_ < capa_ && tail_ < capa_);
        const size_type n = next(tail_);
        if(n == head_)
            return false;    //full
        tools::Construct(buf_ + tail_, v);
        tail_ = n;
        return true;
    }
    bool pop(pointer v){
        assert(head_ < capa_ && tail_ < capa_);
        if(empty())
            return false;
        const size_type n = next(head_);
        if(v)
            *v = buf_[head_];
        tools::Destroy(buf_ + head_);
        head_ = n;
        return true;
    }
private:
    CRingBuf(const __Myt &);    //disable copy and assignment
    __Myt & operator =(const __Myt &);
    void init(){
        assert(!buf_);
        assert(capa_ > 1);
        buf_ = tools::Allocate<value_type>(allocator_type(), capa_);
    }
    void uninit(){
        assert(buf_);
        clear();
        tools::Deallocate(buf_, capa_, allocator_type());
    }
    void clear(){
        while(!empty())
            pop(NULL);
    }
    size_type next(size_type cur) const{
        if(++cur >= capa_)
            cur -= capa_;
        return cur;
    }
    //fields:
    pointer             buf_;
    const size_type     capa_;      //实际容量+1(因为留了一个位置来区分空和满)
    volatile size_type  head_;
    volatile size_type  tail_;
};

NS_SERVER_END

#endif
