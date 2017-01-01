#ifndef DOZERG_MUTEX_H_20070924
#define DOZERG_MUTEX_H_20070924

/*
    对POSIX锁机制进行简单的封装
    方便使用,隐藏底层实现
        CMutexAttr      互斥锁的属性
        CMutex          互斥锁
        CCondAttr       条件变量的属性
        CCondition      条件变量
        CRWLockAttr     读写锁的属性
        CRWLock         读写锁
        CSpinLock       自旋锁
        CFileLock       文件锁
        CLockAdapter    锁适配器
        CGuard          锁守卫
        COnceGuard      确保某些函数只运行一次
        CThreadData     线程本地数据
        CBarrierAttr    栅栏的属性
        CBarrier        栅栏，用于多线程对齐执行点
//*/

#include <sys/file.h>       //flock
#include <pthread.h>
#include <stdexcept>        //std::runtime_error
#include "scoped_ptr.hh"
#include "file.hh"
#include "tools/time.hh"    //GetAbsTimespec, MonoTimeUs
#include "tools/system.hh"  //ErrorMsg

NS_SERVER_BEGIN

class CMutex;

class CMutexAttr
{
    friend class CMutex;
    typedef CMutexAttr __Myt;
public:
    CMutexAttr() throw(std::runtime_error){
        THROW_RT_IF_FAIL(::pthread_mutexattr_init(&attr_));
    }
    ~CMutexAttr(){
        ::pthread_mutexattr_destroy(&attr_);
    }
    void pshared(bool on) throw(std::runtime_error){
        THROW_RT_IF_FAIL(::pthread_mutexattr_setpshared(&attr_, (on ? PTHREAD_PROCESS_SHARED : PTHREAD_PROCESS_PRIVATE)));
    }
    bool pshared() const throw(std::runtime_error){
        int on = 0;
        THROW_RT_IF_FAIL(::pthread_mutexattr_getpshared(&attr_, &on));
        return (PTHREAD_PROCESS_SHARED == on);
    }
    void type(int t) throw(std::runtime_error){
        THROW_RT_IF_FAIL(::pthread_mutexattr_settype(&attr_, t));
    }
    int type() const throw(std::runtime_error){
        int t = 0;
        THROW_RT_IF_FAIL(::pthread_mutexattr_gettype(&attr_, &t));
        return t;
    }
private:
    const pthread_mutexattr_t * a() const{return &attr_;}
    CMutexAttr(const __Myt &);  //disable copy and assignment
    __Myt & operator =(const __Myt &);
    //field
    pthread_mutexattr_t attr_;
};

class CCondition;

//lock types
class CMutex
{
    friend class CCondition;
    typedef CMutex __Myt;
public:
    CMutex() throw(std::runtime_error){
#ifdef NDEBUG
        THROW_RT_IF_FAIL(::pthread_mutex_init(&mutex_, NULL));
#else   //check dead lock
        CMutexAttr attr;
        attr.type(PTHREAD_MUTEX_ERRORCHECK);
        THROW_RT_IF_FAIL(::pthread_mutex_init(&mutex_, attr.a()));
#endif
    }
    explicit CMutex(const CMutexAttr & attr) throw(std::runtime_error){
        THROW_RT_IF_FAIL(::pthread_mutex_init(&mutex_, attr.a()));
    }
    virtual ~CMutex(){
        ::pthread_mutex_destroy(&mutex_);
    }
    void lock() volatile throw(std::runtime_error){
        THROW_RT_IF_FAIL(::pthread_mutex_lock(m()));
    }
    bool tryLock() volatile{
        return (0 == ::pthread_mutex_trylock(m()));
    }
    //在指定的timeMs毫秒内如果不能lock,返回false
    bool timeLock(uint32_t timeMs) volatile{
        timespec ts;
        if(!tools::GetAbsTimespec(timeMs, &ts))
            return false;
        return (0 == ::pthread_mutex_timedlock(m(), &ts));
    }
    void unlock() volatile const throw(std::runtime_error){
        THROW_RT_IF_FAIL(::pthread_mutex_unlock(m()));
    }
private:
    pthread_mutex_t * m() const volatile{
        return const_cast<pthread_mutex_t *>(&mutex_);
    }
    CMutex(const __Myt &);     //disable copy and assignment
    __Myt & operator =(const __Myt &);
    //field
    mutable pthread_mutex_t mutex_;
};

class CCondAttr
{
    friend class CCondition;
    typedef CCondAttr __Myt;
public:
    CCondAttr() throw(std::runtime_error){
        THROW_RT_IF_FAIL(::pthread_condattr_init(&attr_));
    }
    ~CCondAttr(){
        ::pthread_condattr_destroy(&attr_);
    }
    void pshared(bool on) throw(std::runtime_error){
        THROW_RT_IF_FAIL(::pthread_condattr_setpshared(&attr_, (on ? PTHREAD_PROCESS_SHARED : PTHREAD_PROCESS_PRIVATE)));
    }
    bool pshared() const throw(std::runtime_error){
        int on = 0;
        THROW_RT_IF_FAIL(::pthread_condattr_getpshared(&attr_, &on));
        return (PTHREAD_PROCESS_SHARED == on);
    }
private:
    const pthread_condattr_t * a() const{return &attr_;}
    CCondAttr(const __Myt &);   //disable copy and assignment
    __Myt operator =(const __Myt &);
    //field
    pthread_condattr_t attr_;
};

class CCondition
{
    typedef CCondition __Myt;
public:
    CCondition() throw(std::runtime_error){
        THROW_RT_IF_FAIL(::pthread_cond_init(&cond_, NULL));
    }
    explicit CCondition(const CCondAttr & attr) throw(std::runtime_error){
        THROW_RT_IF_FAIL(::pthread_cond_init(&cond_, attr.a()));
    }
    ~CCondition(){
        ::pthread_cond_destroy(&cond_);
    }
    void signal() throw(std::runtime_error){
        THROW_RT_IF_FAIL(::pthread_cond_signal(&cond_));
    }
    void broadcast() throw(std::runtime_error){
        THROW_RT_IF_FAIL(::pthread_cond_broadcast(&cond_));
    }
    void wait(volatile CMutex & m) throw(std::runtime_error){
        THROW_RT_IF_FAIL(::pthread_cond_wait(&cond_, m.m()));
    }
    //等待指定的timeMs毫秒
    bool timeWait(volatile CMutex & m, uint32_t timeMs){
        timespec ts;
        if(!tools::GetAbsTimespec(timeMs, &ts))
            return false;
        return (0 == ::pthread_cond_timedwait(&cond_, m.m(), &ts));
    }
private:
    CCondition(const __Myt &);     //disable copy and assignment
    __Myt & operator =(const __Myt &);
    //field
    pthread_cond_t cond_;
};

class CRWLock;

class CRWLockAttr
{
    friend class CRWLock;
    typedef CRWLockAttr __Myt;
public:
    CRWLockAttr() throw(std::runtime_error){
        THROW_RT_IF_FAIL(::pthread_rwlockattr_init(&attr_));
    }
    ~CRWLockAttr(){
        ::pthread_rwlockattr_destroy(&attr_);
    }
    void pshared(bool on) throw(std::runtime_error){
        THROW_RT_IF_FAIL(::pthread_rwlockattr_setpshared(&attr_, (on ? PTHREAD_PROCESS_SHARED : PTHREAD_PROCESS_PRIVATE)));
    }
    bool pshared() const throw(std::runtime_error){
        int on = 0;
        THROW_RT_IF_FAIL(::pthread_rwlockattr_getpshared(&attr_, &on));
        return (PTHREAD_PROCESS_SHARED == on);
    }
private:
    const pthread_rwlockattr_t * a() const{return &attr_;}
    CRWLockAttr(const __Myt &);   //disable copy and assignment
    __Myt operator =(const __Myt &);
    //field
    pthread_rwlockattr_t attr_;
};

class CRWLock
{
    typedef CRWLock __Myt;
public:
    CRWLock() throw(std::runtime_error){
        THROW_RT_IF_FAIL(::pthread_rwlock_init(&lock_, NULL));
    }
    explicit CRWLock(const CRWLockAttr & attr) throw(std::runtime_error){
        THROW_RT_IF_FAIL(::pthread_rwlock_init(&lock_, attr.a()));
    }
    ~CRWLock(){
        ::pthread_rwlock_destroy(&lock_);
    }
    void readLock() const volatile throw(std::runtime_error){
        THROW_RT_IF_FAIL(::pthread_rwlock_rdlock(l()));
    }
    bool tryReadLock() const volatile{
        return (0 == ::pthread_rwlock_tryrdlock(l()));
    }
    //在指定的timeMs毫秒内如果不能rdlock,返回false
    bool timeReadLock(uint32_t timeMs) const volatile{
        timespec ts;
        if(!tools::GetAbsTimespec(timeMs, &ts))
            return false;
        return (0 == ::pthread_rwlock_timedrdlock(l(), &ts));
    }
    void writeLock() volatile throw(std::runtime_error){
        THROW_RT_IF_FAIL(::pthread_rwlock_wrlock(l()));
    }
    bool tryWriteLock() volatile{
        return (0 == ::pthread_rwlock_trywrlock(l()));
    }
    //在指定的timeMs毫秒内如果不能wrlock,返回false
    bool timeWriteLock(uint32_t timeMs) volatile{
        timespec ts;
        if(!tools::GetAbsTimespec(timeMs, &ts))
            return false;
        return (0 == ::pthread_rwlock_timedwrlock(l(), &ts));
    }
    void unlock() const volatile throw(std::runtime_error){
        THROW_RT_IF_FAIL(::pthread_rwlock_unlock(l()));
    }
private:
    pthread_rwlock_t * l() const volatile{
        return const_cast<pthread_rwlock_t *>(&lock_);
    }
    CRWLock(const __Myt &);   //disable copy and assignment
    __Myt & operator =(const __Myt &);
    //field
    mutable pthread_rwlock_t lock_;
};

class CSpinLock
{
    typedef CSpinLock __Myt;
public:
    explicit CSpinLock(int pshared = false) throw(std::runtime_error){
        THROW_RT_IF_FAIL(::pthread_spin_init(&lock_, (pshared ? PTHREAD_PROCESS_SHARED : PTHREAD_PROCESS_PRIVATE)));
    }
    ~CSpinLock(){
        ::pthread_spin_destroy(&lock_);
    }
    void lock() volatile throw(std::runtime_error){
        THROW_RT_IF_FAIL(::pthread_spin_lock(l()));
    }
    bool tryLock() volatile{
        return (0 == ::pthread_spin_trylock(l()));
    }
    void unlock() const volatile throw(std::runtime_error){
        THROW_RT_IF_FAIL(::pthread_spin_unlock(l()));
    }
private:
    pthread_spinlock_t * l() const volatile{
        return const_cast<pthread_spinlock_t *>(&lock_);
    }
    CSpinLock(const __Myt &);   //disable copy and assignment
    __Myt & operator =(const __Myt &);
    //field
    mutable pthread_spinlock_t lock_;
};

class CFileLock
{
    typedef CFileLock __Myt;
public:
    explicit CFileLock(const char * filename)
        : file_(filename)
    {
        THROW_RT_IF_FAIL(file_.valid() ? 0 : errno);
    }
    explicit CFileLock(const std::string & filename)
        : file_(filename)
    {
        THROW_RT_IF_FAIL(file_.valid() ? 0 : errno);
    }
    void readLock() const volatile throw(std::runtime_error){
        THROW_RT_IF_FAIL((0 == ::flock(fd(), LOCK_SH)) ? 0 : errno);
    }
    bool tryReadLock() const volatile{
        return (0 == ::flock(fd(), LOCK_SH | LOCK_NB));
    }
    void writeLock() volatile throw(std::runtime_error){
        THROW_RT_IF_FAIL((0 == ::flock(fd(), LOCK_EX)) ? 0 : errno);
    }
    bool tryWriteLock() volatile{
        return (0 == ::flock(fd(), LOCK_EX | LOCK_NB));
    }
    void unlock() const volatile throw(std::runtime_error){
        THROW_RT_IF_FAIL((0 == ::flock(fd(), LOCK_UN)) ? 0 : errno);
    }
private:
    int fd() const volatile{
        return const_cast<const CFile &>(file_).fd();
    }
    CFileLock(const __Myt &);   //disable copy and assignment
    __Myt & operator =(const __Myt &);
    //filed
    CFile file_;
};

//adapters for lock types
template<class T>struct CLockAdapter{};

template<>
struct CLockAdapter<CMutex>
{
    typedef CMutex lock_type;
    void unlock(const volatile lock_type & m) const{m.unlock();}
    void readLock(const volatile lock_type & m) const{
        const_cast<volatile lock_type &>(m).lock();
    }
    bool tryReadLock(const volatile lock_type & m) const{
        return const_cast<volatile lock_type &>(m).tryLock();
    }
    bool timeReadLock(const volatile lock_type & m, uint32_t timeMs) const{
        return const_cast<volatile lock_type &>(m).timeLock(timeMs);
    }
    void writeLock(volatile lock_type & m) const{m.lock();}
    bool tryWriteLock(volatile lock_type & m) const{return m.tryLock();}
    bool timeWriteLock(volatile lock_type & m, uint32_t timeMs) const{
        return m.timeLock(timeMs);
    }
};

template<>
struct CLockAdapter<CRWLock>
{
    typedef CRWLock lock_type;
    void unlock(const volatile lock_type & m) const{m.unlock();}
    void readLock(const volatile lock_type & m) const{m.readLock();}
    bool tryReadLock(const volatile lock_type & m) const{return m.tryReadLock();}
    bool timeReadLock(const volatile lock_type & m, uint32_t timeMs) const{
        return m.timeReadLock(timeMs);
    }
    void writeLock(volatile lock_type & m) const{m.writeLock();}
    bool tryWriteLock(volatile lock_type & m) const{return m.tryWriteLock();}
    bool timeWriteLock(volatile lock_type & m, uint32_t timeMs) const{
        return m.timeWriteLock(timeMs);
    }
};

template<>
struct CLockAdapter<CSpinLock>
{
    typedef CSpinLock lock_type;
    void unlock(const volatile lock_type & m) const{m.unlock();}
    void readLock(const volatile lock_type & m) const{const_cast<volatile lock_type &>(m).lock();}
    bool tryReadLock(const volatile lock_type & m) const{
        return const_cast<volatile lock_type &>(m).tryLock();
    }
    bool timeReadLock(const volatile lock_type & m, uint32_t timeMs) const{
        const int64_t end = tools::MonoTimeUs(NULL, true) + timeMs * 1000;
        do{
            if(tryReadLock(m))
                return true;
        }while(end > tools::MonoTimeUs(NULL, true));
        return false;
    }
    void writeLock(volatile lock_type & m) const{m.lock();}
    bool tryWriteLock(volatile lock_type & m) const{return m.tryLock();}
    bool timeWriteLock(volatile lock_type & m, uint32_t timeMs) const{
        const int64_t end = tools::MonoTimeUs(NULL, true) + timeMs * 1000;
        do{
            if(tryWriteLock(m))
                return true;
        }while(end > tools::MonoTimeUs(NULL, true));
        return false;
    }
};

template<>
struct CLockAdapter<CFileLock>
{
    typedef CFileLock lock_type;
    void unlock(const volatile lock_type & m) const{m.unlock();}
    void readLock(const volatile lock_type & m) const{m.readLock();}
    bool tryReadLock(const volatile lock_type & m) const{return m.tryReadLock();}
    bool timeReadLock(const volatile lock_type & m, uint32_t timeMs) const{
        const int64_t end = tools::MonoTimeUs(NULL, true) + timeMs * 1000;
        do{
            if(tryReadLock(m))
                return true;
        }while(end > tools::MonoTimeUs(NULL, true));
        return false;
    }
    void writeLock(volatile lock_type & m) const{m.writeLock();}
    bool tryWriteLock(volatile lock_type & m) const{return m.tryWriteLock();}
    bool timeWriteLock(volatile lock_type & m, uint32_t timeMs) const{
        const int64_t end = tools::MonoTimeUs(NULL, true) + timeMs * 1000;
        do{
            if(tryWriteLock(m))
                return true;
        }while(end > tools::MonoTimeUs(NULL, true));
        return false;
    }
};

//guard for all lock types
template<class LockT>
class CGuard
{
    //typedefs
    typedef CGuard __Myt;
public:
    typedef typename COmitCV<LockT>::result_type    lock_type;
    typedef CLockAdapter<lock_type>                 adapter_type;
    //functions
    explicit CGuard(volatile lock_type & r, bool bWrite = true)
        : lock_(&r)
    {
        bWrite ? adapter_type().writeLock(r) : adapter_type().readLock(r);
    }
    explicit CGuard(const volatile lock_type & r)
        : lock_(&r)
    {
        adapter_type().readLock(r);
    }
    explicit CGuard(volatile lock_type * p, bool bWrite = true)
        : lock_(p)
    {
        if(lock_)
            bWrite ? adapter_type().writeLock(*p) : adapter_type().readLock(*p);
    }
    explicit CGuard(const volatile lock_type * p)
        : lock_(p)
    {
        adapter_type().readLock(*p);
    }
    ~CGuard(){
        if(lock_)
            adapter_type().unlock(*lock_);
    }
private:
    CGuard(const __Myt &);  //disable copy and assignment
    __Myt & operator =(const __Myt &);
    //field
    const volatile lock_type * const lock_;
};

/*
template<class T, class LockT>
class CGuardObj : public CGuard<LockT>
{
    typedef CGuardObj<T, LockT> __Myt;
    typedef CGuard<LockT>                       guard_type;
public:
    typedef typename guard_type::lock_type      lock_type;
    typedef typename COmitCV<T>::result_type    value_type;
    typedef value_type &                        reference;
    typedef const value_type &                  const_reference;
    CGuardObj(reference obj, volatile lock_type & r)
        : g_(r)
        , obj_(obj)
    {}
    CGuardObj(const_reference obj, const volatile lock_type & r)
        : g_(r)
        , obj_(obj)
    {}
    CGuardObj(reference obj, volatile lock_type * p)
        : g_(p)
        , obj_(obj)
    {}
    CGuardObj(const_reference obj, const volatile lock_type * p)
        : g_(p)
        , obj_(obj)
    {}
    T & obj() const{return obj_;}
private:
    guard_type g_;
    T & obj_;
};

template<class T, class LockT>
inline CGuardObj<T, LockT> lockObj(T & obj, LockT & lock)
{
    return CGuardObj<T, LockT>(obj, lock);
}

template<class T, class LockT>
inline CGuardObj<T, LockT> lockObj(T & obj, LockT * lock)
{
    return CGuardObj<T, LockT>(obj, lock);
}
*/

struct COnceGuard
{
    typedef void (*__Function)(void);
    explicit COnceGuard(__Function func = NULL)
        : func_(func)
        , once_(PTHREAD_ONCE_INIT)
    {}
    void runOnce(__Function func = NULL){
        ::pthread_once(&once_, (func ? func : func_));
    }
private:
    __Function      func_;
    pthread_once_t  once_;
};

template<class T>
class CThreadData
{
    typedef CThreadData<T>  __Myt;
public:
    typedef T                   value_type;
    typedef value_type &        reference;
    typedef const value_type &  const_reference;
    typedef value_type *        pointer;
    typedef const value_type *  const_pointer;
private:
    typedef CScopedPtr<value_type>  __Ptr;
    static void Dtor(void * p){__Ptr sp(static_cast<pointer>(p));}
public:
    CThreadData()
        : key_(PTHREAD_KEYS_MAX)
    {
        ::pthread_key_create(&key_, &Dtor);
    }
    bool valid() const{return (key_ != PTHREAD_KEYS_MAX);}
    pointer ptr(){return static_cast<pointer>(getValue());}
    const_pointer ptr() const{return static_cast<const_pointer>(getValue());}
    reference ref(){return *ptr();}
    const_reference ref() const{return *ptr();}
    std::string toString() const{
        CToString oss;
        oss<<"{key_="<<key_<<'}';
        return oss.str();
    }
private:
    void * getValue() const{
        if(!valid())
            return NULL;
        void * p = ::pthread_getspecific(key_);
        if(NULL == p){
            __Ptr sp(tools::New<value_type>());
            if(0 == ::pthread_setspecific(key_, sp.get()))
                p = sp.release();
        }
        return p;
    }
    //field
    pthread_key_t key_;
};

class CBarrier;

class CBarrierAttr
{
    friend class CBarrier;
    typedef CBarrierAttr __Myt;
public:
    CBarrierAttr() throw(std::runtime_error){
        THROW_RT_IF_FAIL(::pthread_barrierattr_init(&attr_));
    }
    ~CBarrierAttr(){
        ::pthread_barrierattr_destroy(&attr_);
    }
    void pshared(bool on) throw(std::runtime_error){
        THROW_RT_IF_FAIL(::pthread_barrierattr_setpshared(&attr_, (on ? PTHREAD_PROCESS_SHARED : PTHREAD_PROCESS_PRIVATE)));
    }
    bool pshared() const throw(std::runtime_error){
        int on = 0;
        THROW_RT_IF_FAIL(::pthread_barrierattr_getpshared(&attr_, &on));
        return (PTHREAD_PROCESS_SHARED == on);
    }
private:
    const pthread_barrierattr_t * a() const{return &attr_;}
    CBarrierAttr(const __Myt &);  //disable copy and assignment
    __Myt & operator =(const __Myt &);
    pthread_barrierattr_t attr_;
};

class CBarrier
{
    typedef CBarrier __Myt;
public:
    explicit CBarrier(size_t count) throw(std::runtime_error){
        THROW_RT_IF_FAIL(::pthread_barrier_init(&barrier_, NULL, count));
    }
    CBarrier(size_t count, const CBarrierAttr & attr) throw(std::runtime_error){
        THROW_RT_IF_FAIL(::pthread_barrier_init(&barrier_, attr.a(), count));
    }
    ~CBarrier(){
        ::pthread_barrier_destroy(&barrier_);
    }
    void wait() throw(std::runtime_error){
        int err = ::pthread_barrier_wait(&barrier_);
        if(0 != err && PTHREAD_BARRIER_SERIAL_THREAD != err)
            throw std::runtime_error(tools::ErrorMsg(err).c_str());
    }
private:
    CBarrier(const __Myt &);     //disable copy and assignment
    __Myt & operator =(const __Myt &);
    //field
    pthread_barrier_t barrier_;
};

NS_SERVER_END

#endif
