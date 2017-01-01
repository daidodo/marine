#ifndef DOZERG_THREADS_H_20130222
#define DOZERG_THREADS_H_20130222

/*
    POSIX线程封装
        CThreadAttr     线程的属性
        CThread         单个线程
        CThreadPool     固定数目的线程池
        CThreadManager  自动伸缩的线程池
//*/

#include <pthread.h>
#include <vector>
#include <list>
#include <cassert>
#include "lock_int.hh"
#include "lock_queue.hh"
#include "logger.hh"
#include "atomic_sync.hh"
#include "impl/threads_impl.hh"

NS_SERVER_BEGIN

class CThread;

class CThreadAttr
{
    friend class CThread;
    typedef CThreadAttr __Myt;
public:
    CThreadAttr() throw(std::runtime_error){
        THROW_RT_IF_FAIL(::pthread_attr_init(&a_));
    }
    ~CThreadAttr(){
        ::pthread_attr_destroy(&a_);
    }
    //设置/获取栈大小（字节）
    void stackSize(size_t sz) throw(std::runtime_error){
        THROW_RT_IF_FAIL(::pthread_attr_setstacksize(&a_, sz));
    }
    size_t stackSize() const throw(std::runtime_error){
        size_t sz = 0;
        THROW_RT_IF_FAIL(::pthread_attr_getstacksize(&a_, &sz));
        return sz;
    }
    //设置/获取栈保护空间大小（字节）
    void guardSize(size_t sz) throw(std::runtime_error){
        THROW_RT_IF_FAIL(::pthread_attr_setguardsize(&a_, sz));
    }
    size_t guardSize() const throw(std::runtime_error){
        size_t sz = 0;
        THROW_RT_IF_FAIL(::pthread_attr_getguardsize(&a_, &sz));
        return sz;
    }
    //设置/获取线程状态
    void detach(bool on) throw(std::runtime_error){
        THROW_RT_IF_FAIL(::pthread_attr_setdetachstate(&a_
                    , (on ? PTHREAD_CREATE_DETACHED : PTHREAD_CREATE_JOINABLE)));
    }
    bool detach() const throw(std::runtime_error){
        int state = 0;
        THROW_RT_IF_FAIL(::pthread_attr_getdetachstate(&a_, &state));
        return (PTHREAD_CREATE_DETACHED == state);
    }
#ifdef __API_HAS_PTHREAD_ATTR_SETSTACK
    //设置/获取栈地址和大小
    void stack(void * addr, size_t sz) throw(std::runtime_error){
        THROW_RT_IF_FAIL(::pthread_attr_setstack(&a_, addr, sz));
    }
    void * stack(size_t * sz) const throw(std::runtime_error){
        void * addr = NULL;
        size_t s = 0;
        THROW_RT_IF_FAIL(::pthread_attr_getstack(&a_, &addr, &s));
        if(sz)
            *sz = s;
        return addr;
    }
#endif
private:
    CThreadAttr(const __Myt &); //disable copy and assignment
    __Myt & operator =(const __Myt &);
    pthread_attr_t * a(){return &a_;}
    const pthread_attr_t * a() const{return &a_;}
    //fields
    pthread_attr_t a_;
};

class CThread
{
    typedef CThread __Myt;
    typedef void * (*__ThreadProc)(void *);
public:
    CThread():ret_(NULL), id_(0){}
    //启动线程
    //proc: 线程函数
    //arg: 线程函数的参数
    //attr: 线程属性
    //注意:
    //  重复调用start()，会导致之前的线程id等信息丢失
    bool start(__ThreadProc proc, void * arg = NULL){
        return (0 == ::pthread_create(&id_, NULL, proc, arg));
    }
    bool start(__ThreadProc proc, void * arg, const CThreadAttr & attr){
        return (0 == ::pthread_create(&id_, attr.a(), proc, arg));
    }
    //通知线程停止
    bool stop(){
        return (0 == ::pthread_cancel(id_));
    }
    //等待线程停止，并存储返回值
    //仅对joinable状态的线程有效
    bool join(){
        return (0 == ::pthread_join(id_, &ret_));
    }
    //获取线程返回值
    //仅对joinable状态的线程有效
    void * retval() const{return ret_;}
    //detach线程
    //仅对joinable状态的线程有效
    bool detach(){
        return (0 == ::pthread_detach(id_));
    }
    //给线程发送信号
    bool signal(int sig) const{
        return (0 == ::pthread_kill(id_, sig));
    }
    //判断线程id是否相等
    bool operator ==(const __Myt & t) const{
        return operator ==(t.id_);
    }
    bool operator ==(pthread_t t) const{
        return (0 != ::pthread_equal(id_, t));
    }
    bool operator !=(const __Myt & t) const{
        return !operator ==(t);
    }
    bool operator !=(pthread_t t) const{
        return !operator ==(t);
    }
    //转化成可读字符串
    std::string toString() const{
        CToString oss;
        oss<<"{id_="<<id_
            <<", ret_="<<ret_
            <<'}';
        return oss.str();
    }
private:
    //fields
    void * ret_;
    pthread_t id_;
};

inline bool operator ==(pthread_t t1, const CThread & t2)
{
    return (t2.operator ==(t1));
}

inline bool operator !=(pthread_t t1, const CThread & t2)
{
    return (t2.operator !=(t1));
}

class CThreadPool
{
    typedef CThreadPool                 __Myt;
    typedef CAtomicSync<unsigned int>   __Count;
    typedef std::vector<CThread>        __Threads;
public:
    CThreadPool(){}
    virtual ~CThreadPool(){}
    //启动所有线程
    //子类可以重载，但最后应该调用CThreadPool::startThreads()
    //threadCount: 线程数
    //attr: 线程属性
    //return:
    //  <0      出错，所有线程都无法启动
    //  其他    实际启动的线程数
    virtual int startThreads(unsigned int threadCount = 1){
        if(!threads_.empty())
            return -1;  //re-start
        threads_.reserve(threadCount);
        while(threadCount-- > 0){
            CThread t;
            if(t.start(threadProc, this))
                threads_.push_back(t);
        }
        return threads_.size();
    }
    virtual int startThreads(unsigned int threadCount, const CThreadAttr & attr){
        if(!threads_.empty())
            return -1;  //re-start
        threads_.reserve(threadCount);
        while(threadCount-- > 0){
            CThread t;
            if(t.start(threadProc, this, attr))
                threads_.push_back(t);
        }
        return threads_.size();
    }
    //获取运行的线程数
    unsigned int runningCount() const volatile{return cnt_;}
    //判断所有线程是否都已停止
    bool stopped() const volatile{return (0 == runningCount());}
    //通知所有线程停止
    //子类可以重载，但最后应该调用CThreadPool::stopThreads()
    virtual void stopThreads(){
        for(__Threads::iterator i = threads_.begin();i != threads_.end();++i)
            i->stop();
    }
    //等待所有线程退出，回收资源
    //仅对joinable状态的线程有效
    //子类可以重载，但最后应该调用CThreadPool::joinThreads()
    virtual void joinThreads(){
        for(__Threads::iterator i = threads_.begin();i != threads_.end();++i)
            i->join();
    }
    //detach所有线程
    //仅对joinable状态的线程有效
    //子类可以重载，但最后应该调用CThreadPool::detachThreads()
    virtual void detachThreads(){
        for(__Threads::iterator i = threads_.begin();i != threads_.end();++i)
            i->detach();
    }
    //给所有线程发送信号
    void signalThreads(int sig) const{
        for(__Threads::const_iterator i = threads_.begin();i != threads_.end();++i)
            i->signal(sig);
    }
    //转换成可读字符串
    std::string toString() const{
        CToString oss;
        oss<<"{cnt_="<<cnt_
            <<", threads_=("<<threads_.size()<<"){";
        for(size_t i = 0;i < threads_.size();++i){
            if(i)
                oss<<", ";
            oss<<'['<<i<<"]="<<threads_[i].toString();
        }
        oss<<"}}";
        return oss.str();
    }
protected:
    //工作函数，子类必须重载
    //注意：函数退出后，线程也会结束
    virtual void run() = 0;
private:
    CThreadPool(const __Myt &); //disable copy and assignment
    __Myt & operator =(const __Myt &);
    static void * threadProc(void * arg){
        typedef NS_IMPL::CActive<__Count> __Active;
        LOGGER_CRASH_HANDLER();
        assert(arg);
        __Myt & self = *static_cast<__Myt *>(arg);
        __Active a(self.cnt_);
        self.run();
        return NULL;
    }
    //fields
    __Threads threads_;
    __Count cnt_;
};

template<class Task>
class CThreadManager
{
    typedef CThreadManager<Task>            __Myt;
    typedef CLockInt<unsigned int>          __Count;
    typedef CLockIntMax<unsigned int>       __CountMax;
    typedef std::list<CThread>              __Workers;
    typedef CMutex                          __WorkLock;
    typedef CGuard<__WorkLock>              __WorkGuard;
protected:
    typedef Task                    __Task;
    typedef CLockQueue<__Task>      __Queue;
    static const int kScheduleInterval = 500;  //ms, 默认调度间隔
public:
    CThreadManager()
        : inputQue_(NULL)
        , threadCountMin_(1)
        , threadCountMax_(0)
        , interval_(kScheduleInterval)
    {}
    virtual ~CThreadManager(){}
    //设置/获取最少线程数
    void threadCountMin(unsigned int count){threadCountMin_ = count;}
    unsigned int threadCountMin() const{return threadCountMin_;}
    //设置/获取最多线程数
    void threadCountMax(unsigned int count){threadCountMax_ = count;}
    unsigned int threadCountMax() const{return threadCountMax_;}
    //设置/获取调度线程的处理间隔时间(毫秒)
    void scheduleInterval(int timeMs){interval_ = timeMs;}
    int scheduleInterval() const{return interval_;}
    //启动线程
    //子类可以重载，但最后应该调用CThreadManager::startThreads()
    //inputQue: 任务队列
    //initCount: 初始线程数目
    //  <threadCountMin_    启动threadCountMin_个线程
    //  >threadCountMax_    启动threadCountMax_个线程
    //  其他                启动initCount个线程
    //stackSz: 每个线程的栈大小
    //  0       系统默认
    //  其他    指定字节大小
    //return:
    //  <0      出错
    //  其他    实际启动的worker线程数
    virtual int startThreads(__Queue & inputQue, unsigned int initCount = 1, size_t stackSz = 0){
        //check
        if(NULL != inputQue_)
            return -1;  //re-start
        //attr
        attr_.detach(true);
        if(stackSz)
            attr_.stackSize(stackSz);
        //schedule
        if(!sched_.start(threadSchedule, this, attr_))
            return -1;
        inputQue_ = &inputQue;
        //worker
        return addThreads(adjustThreadCount(initCount));
    }
    //通知所有线程停止
    //子类可以重载，但最后应该调用CThreadManager::stopThreads()
    virtual void stopThreads(){
        sched_.stop();
        __Workers::iterator it;
        __WorkGuard g(workLock_);
        for(it = workers_.begin();it != workers_.end();++it)
            it->stop();
        workers_.clear();
    }
    //获取worker线程数统计
    //countMax:
    //  NULL    忽略
    //  其他    返回最大worker线程数目，且重新开始统计
    //return: 当前worker线程数目
    unsigned int runningCount(unsigned int * countMax = NULL) volatile{
        if(NULL != countMax)
            *countMax = workerCount_.resetMax();
        return workerCount_.load();
    }
    //获取活跃线程统计
    //countMax:
    //  NULL    忽略
    //  其他    返回最大活跃线程数目，且重新开始统计
    //return: 当前活跃线程数目
    unsigned int activeCount(unsigned int * countMax = NULL) volatile{
        if(NULL != countMax)
            *countMax = activeCount_.resetMax();
        return activeCount_.load();
    }
    //检查所有线程是否停止
    bool stopped() const volatile{return (0 == workerCount_.load());}
    //转换成可读字符串
    std::string toString() const{
        CToString oss;
        oss<<"{sched_="<<sched_.toString()
            <<", workerCount_="<<workerCount_.load()
            <<", activeCount_="<<activeCount_.load()
            <<", deleteCount_="<<deleteCount_.load()
            <<", inputQue_="<<inputQue_
            <<", threadCountMin_="<<threadCountMin_
            <<", threadCountMax_="<<threadCountMax_
            <<", interval_="<<interval_
            <<'}';
        return oss.str();
    }
protected:
    //工作函数，负责处理一个任务，完成后返回
    //子类必须重载
    //task: 从任务队列中取出的任务
    virtual void run(__Task & task) = 0;
private:
    //schedule thread
    static void * threadSchedule(void * arg){
        LOGGER_CRASH_HANDLER();
        assert(arg);
        __Myt & self = *static_cast<__Myt *>(arg);
        ::sleep(1);
        for(unsigned int expect = 0;;){
            ::usleep(self.interval_ * 1000);
            const int kStrategy = 1;    //策略选择
            const unsigned int count = self.workerCount_.load();
            const unsigned int active = self.activeCount_.load();
            int add = 0;
            if(0 == kStrategy){
                expect = self.adjustThreadCount(active << 1);
                if(count > expect + 4 && count > expect + (expect >> 1)){
                    add = expect - count;
                }else if(count < expect)    //need more threads
                    add = expect - count;
            }else if(1 == kStrategy){
                expect = (expect +  1) >> 1;
                unsigned int max = self.adjustThreadCount(count);
                if(count > max){
                    add = max - count;
                }else{
                    expect = self.adjustThreadCount(expect + active);
                    if(count > expect + 4 && count > expect + (expect >> 1)){
                        add = expect - count;
                    }else if(count < expect)    //need more threads
                        add = expect - count;
                }
            }
            if(add > 0)
                self.addThreads(add);
            else if(add < 0)
                self.deleteCount_ = -add;
            //FATAL("stats={"<<count<<", "<<active<<", "<<expect<<", "<<add<<"}");
        }
        return NULL;
    }
    //worker thread
    static void * threadWorker(void * arg){
        typedef NS_IMPL::CActive<__CountMax> __Active;
        LOGGER_CRASH_HANDLER();
        assert(arg);
        __Myt & self = *static_cast<__Myt *>(arg);
        assert(self.inputQue_);
        __Active a(self.workerCount_);
        for(__Task task;self.querySurvive();){
            if(self.inputQue_->pop(task)){
                __Active b(self.activeCount_);
                ::pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
                self.run(task);
                ::pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
            }
        }
        self.rmWorker(::pthread_self());
        return NULL;
    }
    unsigned int adjustThreadCount(unsigned int count) const{
        const unsigned int t_min = threadCountMin_;
        const unsigned int t_max = threadCountMax_;
        if(count < t_min || t_max <= t_min)
            return t_min;
        if(count > t_max)
            return t_max;
        return count;
    }
    bool querySurvive(){
        typedef NS_IMPL::IsPositive<__CountMax::value_type> __Pred;
        return !deleteCount_.test_sub(__Pred(), 1, NULL, NULL);
    }
    int addThreads(unsigned int count){
        if(0 == count)
            return 0;
        int ret = 0;
        while(count-- > 0){
            CThread t;
            __WorkGuard g(workLock_);
            //避免其他线程调用stopThreads()后，schedule线程继续创建worker线程
            ::pthread_testcancel();
            if(t.start(threadWorker, this, attr_)){
                workers_.push_back(t);
                ++ret;
            }
        }
        return ret;
    }
    void rmWorker(pthread_t t){
        __Workers::iterator it;
        __WorkGuard g(workLock_);
        for(it = workers_.begin();it != workers_.end();++it)
            if(*it == t){
                workers_.erase(it);
                break;
            }
    }
    CThreadManager(const __Myt &);  //disable copy and assignment
    __Myt & operator =(const __Myt &);
    //fields
    CThread sched_;
    __Workers workers_;
    __WorkLock workLock_;
    CThreadAttr attr_;
    __CountMax workerCount_;
    __CountMax activeCount_;
    __Count deleteCount_;   //thread count to be deleted
    __Queue * inputQue_;
    volatile unsigned int threadCountMin_;
    volatile unsigned int threadCountMax_;
    volatile int interval_; //ms, schedule interval
};

NS_SERVER_END

#endif

