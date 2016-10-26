#ifndef DOZERG_LOCKED_QUEUE_H_20070901
#define DOZERG_LOCKED_QUEUE_H_20070901

/*
    加锁保护的FIFO消息队列,适合线程间传递数据
        CLockQueue
    History
        20070925    把pthread_cond_t和pthread_mutex_t替换成CCondMutex
        20071025    加入capacity_,并把CCondMutex分成CMutex和CCondition
        20080128    加入pushFront(),把元素放到队列前面
        20080203    加入Mutex(),统一接口
        20080903    增加popAll(),避免频繁pop()
        20080911    增加pushAll(),避免频繁push()
        20080912    增加_append(),对适用pushAll()的类型进行接口统一
        20080920    增加lock_type，adapter_type和guard_type，修改getLock(),lock()和unlock()
        20111204    去掉对CList的支持，将Container改成成员变量
                    修改waitNotEmpty()和waitNotFull()，严格检验size
        20130603    把broadcast()修改成signal()
                    去掉getLock(),lock(),unlock()
        20131016    去掉adapter_type
//*/

#include "mutex.hh"
#include "single_list.hh"

NS_SERVER_BEGIN

template<class T, class Container = CSingleList<T> >
class CLockQueue
{
    typedef CLockQueue<T, Container> __Myt;
public:
    typedef CMutex              lock_type;
    typedef CGuard<lock_type>   guard_type;
    typedef Container           container_type;
    typedef typename container_type::value_type       value_type;
    typedef typename container_type::size_type        size_type;
    typedef typename container_type::reference        reference;
    typedef typename container_type::iterator         iterator;
    typedef typename container_type::const_reference  const_reference;
    typedef typename container_type::const_iterator   const_iterator;
private:
    static const size_t kCapacityDefault = 10000;   //默认容量
    template<class E, class A>
    static void _append(CSingleList<E, A> & to, CSingleList<E, A> & from){
        to.append(from);
    }
public:
    explicit CLockQueue(size_t capacity = kCapacityDefault)
        : capacity_(capacity)
        , top_size_(0)
    {}
    explicit CLockQueue(const container_type & con, size_t capacity = kCapacityDefault)
        : con_(con)
        , capacity_(capacity)
        , top_size_(con.size())
    {}
    size_type capacity() const{return capacity_;}
    void capacity(size_type c){capacity_ = c;}
    bool empty() const volatile{
        guard_type g(lock_);
        return c().empty();
    }
    size_type size() const volatile{
        guard_type g(lock_);
        return c().size();
    }
    //timeMs < 0,阻塞式;timeMs >= 0,等待timeMs毫秒
    bool push(const_reference v, int32_t timeMs = -1){
        guard_type g(lock_);
        const int sig = waitNotFull(timeMs, 1);
        if(sig < 0)
            return false;
        con_.push_back(v);
        const size_t sz = con_.size();
        if(sz > top_size_)  //for statistic
            top_size_ = sz;
        not_empty_.signal();
        return true;
    }
    //timeMs < 0,阻塞式;timeMs >= 0,等待timeMs毫秒
    bool pushFront(const_reference v, int32_t timeMs = -1){
        guard_type g(lock_);
        const int sig = waitNotFull(timeMs, 1);
        if(sig < 0)
            return false;
        con_.push_front(v);
        const size_t sz = con_.size();
        if(sz > top_size_)  //for statistic
            top_size_ = sz;
        not_empty_.signal();
        return true;
    }
    //把con的所有元素加到队列尾
    //timeMs < 0,阻塞式;timeMs >= 0,等待timeMs毫秒
    //只有重载了CLockQueue::_append函数的类型，才能使用此操作
    bool pushAll(container_type & con, int32_t timeMs = -1){
        if(con.empty())
            return true;
        guard_type g(lock_);
        const int sig = waitNotFull(timeMs, con.size());
        if(sig < 0)
            return false;
        _append(con_, con);
        const size_t sz = con_.size();
        if(sz > top_size_)  //for statistic
            top_size_ = sz;
        not_empty_.broadcast();
        return true;
    }
    //timeMs < 0,阻塞式;timeMs >= 0,等待timeMs毫秒
    bool pop(reference v, int32_t timeMs = -1){
        guard_type g(lock_);
        const int sig = waitNotEmpty(timeMs, 1);
        if(sig < 0)
            return false;
        v = con_.front();
        con_.pop_front();
        not_full_.signal();
        return true;
    }
    //把队列的所有元素转移到con里
    //timeMs < 0,阻塞式;timeMs >= 0,等待timeMs毫秒
    bool popAll(container_type & con, int32_t timeMs = -1){
        con.clear();
        guard_type g(lock_);
        const int sig = waitNotEmpty(timeMs, (con_.empty() ? capacity_ : con_.size()));
        if(sig < 0)
            return false;
        con_.swap(con);
        not_full_.broadcast();
        return true;
    }
    size_t topSize() const volatile{    //队列长度的峰值
        guard_type g(lock_);
        return top_size_;
    }
    size_t resetTopSize() volatile{
        guard_type g(lock_);
        size_t ret = top_size_;
        top_size_ = c().size();
        return ret;
    }
private:
    /*
    Return Value:
        -1  failed
        0   not full
        1   full
    //*/
    int waitNotEmpty(int32_t timeMs, size_t need){
        if(need > capacity_)
            return -1;
        while(con_.empty()){
            if(timeMs < 0){
                not_empty_.wait(lock_);
            }else if(!timeMs || !not_empty_.timeWait(lock_, timeMs))
                return -1;
        }
        need = (con_.size() <= need ? 0 : con_.size() - need); //size after pop
        return (con_.size() >= capacity_ && need < capacity_ ? 1 : 0);
    }
    /*
    Return Value:
        -1  failed
        0   not empty
        1   empty
    //*/
    int waitNotFull(int32_t timeMs, size_t need){
        assert(need > 0);
        if(need > capacity_)
            return -1;
        while(con_.size() + need > capacity_){
            if(timeMs < 0){
                not_full_.wait(lock_);
            }else if(!timeMs || !not_full_.timeWait(lock_, timeMs))
                return -1;
        }
        return (con_.empty() ? 1 : 0);
    }
    container_type & c() volatile{return const_cast<container_type &>(con_);}
    const container_type & c() const volatile{return const_cast<const container_type &>(con_);}
    CLockQueue(const __Myt &);  //disable copy and assignment
    __Myt & operator =(const __Myt &);
    //fields
    container_type  con_;
    size_type   capacity_;      //队列最大长度,达到capacity_后push会阻塞
    size_type   top_size_;      //con_.size()的峰值,统计用
    lock_type   lock_;
    CCondition  not_empty_, not_full_;
};

NS_SERVER_END

#endif

