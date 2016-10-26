#ifndef DOZERG_THREADS_H_20130222
#define DOZERG_THREADS_H_20130222

/*
    POSIX�̷߳�װ
        CThreadAttr     �̵߳�����
        CThread         �����߳�
        CThreadPool     �̶���Ŀ���̳߳�
        CThreadManager  �Զ��������̳߳�
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
    //����/��ȡջ��С���ֽڣ�
    void stackSize(size_t sz) throw(std::runtime_error){
        THROW_RT_IF_FAIL(::pthread_attr_setstacksize(&a_, sz));
    }
    size_t stackSize() const throw(std::runtime_error){
        size_t sz = 0;
        THROW_RT_IF_FAIL(::pthread_attr_getstacksize(&a_, &sz));
        return sz;
    }
    //����/��ȡջ�����ռ��С���ֽڣ�
    void guardSize(size_t sz) throw(std::runtime_error){
        THROW_RT_IF_FAIL(::pthread_attr_setguardsize(&a_, sz));
    }
    size_t guardSize() const throw(std::runtime_error){
        size_t sz = 0;
        THROW_RT_IF_FAIL(::pthread_attr_getguardsize(&a_, &sz));
        return sz;
    }
    //����/��ȡ�߳�״̬
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
    //����/��ȡջ��ַ�ʹ�С
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
    //�����߳�
    //proc: �̺߳���
    //arg: �̺߳����Ĳ���
    //attr: �߳�����
    //ע��:
    //  �ظ�����start()���ᵼ��֮ǰ���߳�id����Ϣ��ʧ
    bool start(__ThreadProc proc, void * arg = NULL){
        return (0 == ::pthread_create(&id_, NULL, proc, arg));
    }
    bool start(__ThreadProc proc, void * arg, const CThreadAttr & attr){
        return (0 == ::pthread_create(&id_, attr.a(), proc, arg));
    }
    //֪ͨ�߳�ֹͣ
    bool stop(){
        return (0 == ::pthread_cancel(id_));
    }
    //�ȴ��߳�ֹͣ�����洢����ֵ
    //����joinable״̬���߳���Ч
    bool join(){
        return (0 == ::pthread_join(id_, &ret_));
    }
    //��ȡ�̷߳���ֵ
    //����joinable״̬���߳���Ч
    void * retval() const{return ret_;}
    //detach�߳�
    //����joinable״̬���߳���Ч
    bool detach(){
        return (0 == ::pthread_detach(id_));
    }
    //���̷߳����ź�
    bool signal(int sig) const{
        return (0 == ::pthread_kill(id_, sig));
    }
    //�ж��߳�id�Ƿ����
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
    //ת���ɿɶ��ַ���
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
    //���������߳�
    //����������أ������Ӧ�õ���CThreadPool::startThreads()
    //threadCount: �߳���
    //attr: �߳�����
    //return:
    //  <0      ���������̶߳��޷�����
    //  ����    ʵ���������߳���
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
    //��ȡ���е��߳���
    unsigned int runningCount() const volatile{return cnt_;}
    //�ж������߳��Ƿ���ֹͣ
    bool stopped() const volatile{return (0 == runningCount());}
    //֪ͨ�����߳�ֹͣ
    //����������أ������Ӧ�õ���CThreadPool::stopThreads()
    virtual void stopThreads(){
        for(__Threads::iterator i = threads_.begin();i != threads_.end();++i)
            i->stop();
    }
    //�ȴ������߳��˳���������Դ
    //����joinable״̬���߳���Ч
    //����������أ������Ӧ�õ���CThreadPool::joinThreads()
    virtual void joinThreads(){
        for(__Threads::iterator i = threads_.begin();i != threads_.end();++i)
            i->join();
    }
    //detach�����߳�
    //����joinable״̬���߳���Ч
    //����������أ������Ӧ�õ���CThreadPool::detachThreads()
    virtual void detachThreads(){
        for(__Threads::iterator i = threads_.begin();i != threads_.end();++i)
            i->detach();
    }
    //�������̷߳����ź�
    void signalThreads(int sig) const{
        for(__Threads::const_iterator i = threads_.begin();i != threads_.end();++i)
            i->signal(sig);
    }
    //ת���ɿɶ��ַ���
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
    //���������������������
    //ע�⣺�����˳����߳�Ҳ�����
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
    static const int kScheduleInterval = 500;  //ms, Ĭ�ϵ��ȼ��
public:
    CThreadManager()
        : inputQue_(NULL)
        , threadCountMin_(1)
        , threadCountMax_(0)
        , interval_(kScheduleInterval)
    {}
    virtual ~CThreadManager(){}
    //����/��ȡ�����߳���
    void threadCountMin(unsigned int count){threadCountMin_ = count;}
    unsigned int threadCountMin() const{return threadCountMin_;}
    //����/��ȡ����߳���
    void threadCountMax(unsigned int count){threadCountMax_ = count;}
    unsigned int threadCountMax() const{return threadCountMax_;}
    //����/��ȡ�����̵߳Ĵ�����ʱ��(����)
    void scheduleInterval(int timeMs){interval_ = timeMs;}
    int scheduleInterval() const{return interval_;}
    //�����߳�
    //����������أ������Ӧ�õ���CThreadManager::startThreads()
    //inputQue: �������
    //initCount: ��ʼ�߳���Ŀ
    //  <threadCountMin_    ����threadCountMin_���߳�
    //  >threadCountMax_    ����threadCountMax_���߳�
    //  ����                ����initCount���߳�
    //stackSz: ÿ���̵߳�ջ��С
    //  0       ϵͳĬ��
    //  ����    ָ���ֽڴ�С
    //return:
    //  <0      ����
    //  ����    ʵ��������worker�߳���
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
    //֪ͨ�����߳�ֹͣ
    //����������أ������Ӧ�õ���CThreadManager::stopThreads()
    virtual void stopThreads(){
        sched_.stop();
        __Workers::iterator it;
        __WorkGuard g(workLock_);
        for(it = workers_.begin();it != workers_.end();++it)
            it->stop();
        workers_.clear();
    }
    //��ȡworker�߳���ͳ��
    //countMax:
    //  NULL    ����
    //  ����    �������worker�߳���Ŀ�������¿�ʼͳ��
    //return: ��ǰworker�߳���Ŀ
    unsigned int runningCount(unsigned int * countMax = NULL) volatile{
        if(NULL != countMax)
            *countMax = workerCount_.resetMax();
        return workerCount_.load();
    }
    //��ȡ��Ծ�߳�ͳ��
    //countMax:
    //  NULL    ����
    //  ����    ��������Ծ�߳���Ŀ�������¿�ʼͳ��
    //return: ��ǰ��Ծ�߳���Ŀ
    unsigned int activeCount(unsigned int * countMax = NULL) volatile{
        if(NULL != countMax)
            *countMax = activeCount_.resetMax();
        return activeCount_.load();
    }
    //��������߳��Ƿ�ֹͣ
    bool stopped() const volatile{return (0 == workerCount_.load());}
    //ת���ɿɶ��ַ���
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
    //����������������һ��������ɺ󷵻�
    //�����������
    //task: �����������ȡ��������
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
            const int kStrategy = 1;    //����ѡ��
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
            //���������̵߳���stopThreads()��schedule�̼߳�������worker�߳�
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

