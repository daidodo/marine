#include <marine/mutex.hh>

#include <marine/threads.hh>
#include "../inc.h"

TEST(CBarrierAttr, all)
{
    CBarrierAttr attr;
    //pshared
    ASSERT_FALSE(attr.pshared());
    attr.pshared(true);
    ASSERT_TRUE(attr.pshared());
    attr.pshared(false);
    ASSERT_FALSE(attr.pshared());
}

static void * barrierTestProc(void * arg)
{
    assert(arg);
    CBarrier & b = *reinterpret_cast<CBarrier *>(arg);
    b.wait();
    return reinterpret_cast<void *>(::time(NULL));
}

TEST(CBarrier, all)
{
    CBarrierAttr a;
    CBarrier b(2), b2(2, a);
    CThread th;
    th.start(barrierTestProc, &b);
    ::sleep(1);
    b.wait();
    time_t t = time(NULL);
    th.join();
    time_t t1 = reinterpret_cast<time_t>(th.retval());
    ASSERT_EQ(t, t1);
}

TEST(CMutexAttr, all)
{
    CMutexAttr attr;
    //pshared
    ASSERT_FALSE(attr.pshared());
    attr.pshared(true);
    ASSERT_TRUE(attr.pshared());
    attr.pshared(false);
    ASSERT_FALSE(attr.pshared());
    //type
    ASSERT_EQ(PTHREAD_MUTEX_DEFAULT, attr.type());
    attr.type(PTHREAD_MUTEX_NORMAL);
    ASSERT_EQ(PTHREAD_MUTEX_NORMAL, attr.type());
    attr.type(PTHREAD_MUTEX_ERRORCHECK);
    ASSERT_EQ(PTHREAD_MUTEX_ERRORCHECK, attr.type());
    attr.type(PTHREAD_MUTEX_RECURSIVE);
    ASSERT_EQ(PTHREAD_MUTEX_RECURSIVE, attr.type());
    attr.type(PTHREAD_MUTEX_DEFAULT);
    ASSERT_EQ(PTHREAD_MUTEX_DEFAULT, attr.type());
}

TEST(CMutex, all)
{
    CMutexAttr attr;
    CMutex m, m1(attr);

    ASSERT_TRUE(m.tryLock());
    m.unlock();

    m.lock();
    m.unlock();

    ASSERT_TRUE(m.timeLock(1000));
    m.unlock();
}

TEST(CCondAttr, all)
{
    CCondAttr attr;
    //pshared
    ASSERT_FALSE(attr.pshared());
    attr.pshared(true);
    ASSERT_TRUE(attr.pshared());
    attr.pshared(false);
    ASSERT_FALSE(attr.pshared());
}

static void * condProc(void * arg)
{
    assert(arg);
    sleep(1);
    CCondition & c = *static_cast<CCondition *>(arg);
    c.signal();
    return NULL;
}

TEST(CCondition, all)
{
    CCondAttr attr;
    CCondition c, c1(attr);

    c.signal();
    c.broadcast();

    CMutex m;
    m.lock();
    time_t start = ::time(NULL);
    for(int i = 0;i < 10;++i)
        ASSERT_FALSE(c.timeWait(m, 100));
    time_t end = ::time(NULL);
    ASSERT_EQ(start + 1, end);

    CThread t;
    t.start(condProc, &c);
    c.wait(m);
    m.unlock();
    t.join();
}

TEST(CRWLockAttr, all)
{
    CRWLockAttr attr;
    //pshared
    ASSERT_FALSE(attr.pshared());
    attr.pshared(true);
    ASSERT_TRUE(attr.pshared());
    attr.pshared(false);
    ASSERT_FALSE(attr.pshared());
}

TEST(CRWLock, all)
{
    CRWLockAttr attr;
    CRWLock rw, rw1(attr);

    rw.readLock();
    rw.unlock();

    ASSERT_TRUE(rw.tryReadLock());
    rw.unlock();

    ASSERT_TRUE(rw.timeReadLock(1000));
    rw.unlock();

    rw.writeLock();
    rw.unlock();

    ASSERT_TRUE(rw.tryWriteLock());
    rw.unlock();

    ASSERT_TRUE(rw.timeWriteLock(1000));
    rw.unlock();
}

TEST(CSpinLock, all)
{
    CSpinLock s, s1(false), s2(true);

    s.lock();
    s.unlock();

    ASSERT_TRUE(s.tryLock());
    s.unlock();
}

TEST(CFileLock, all)
{
    CFileLock f("mutex_unittest.cc");

    f.readLock();
    f.unlock();

    ASSERT_TRUE(f.tryReadLock());
    f.unlock();

    f.writeLock();
    f.unlock();

    ASSERT_TRUE(f.tryWriteLock());
    f.unlock();
}

template<class LockT>
static bool testAdapter(LockT & l)
{
    typedef CLockAdapter<LockT>             __Adapter;

    __Adapter().readLock(l);
    __Adapter().unlock(l);

    if((!__Adapter().tryReadLock(l)))
        return false;
    __Adapter().unlock(l);

    if(!__Adapter().timeReadLock(l, 1000))
        return false;
    __Adapter().unlock(l);

    __Adapter().writeLock(l);
    __Adapter().unlock(l);

    if((!__Adapter().tryWriteLock(l)))
        return false;
    __Adapter().unlock(l);

    if(!__Adapter().timeWriteLock(l, 1000))
        return false;
    __Adapter().unlock(l);
    return true;
}

template<class LockT>
static bool testAdapter()
{
    typedef CLockAdapter<LockT>             __Adapter;
    typedef typename __Adapter::lock_type   __Lock;
    __Lock l;

    return testAdapter(l);
}

TEST(CLockAdapter, all)
{
    ASSERT_TRUE(testAdapter<CMutex>());
    ASSERT_TRUE(testAdapter<CRWLock>());
    ASSERT_TRUE(testAdapter<CSpinLock>());
    CFileLock f("mutex_unittest.cc");
    ASSERT_TRUE(testAdapter(f));
}

template<class LockT>
static void testGuardImp(LockT & lock)
{
    typedef CGuard<LockT>               __Guard;
    typedef LockT __Lock;
    {
        __Lock & l = lock;
        __Lock * p = &lock;
        {
            __Guard g(l);
        }{
            __Guard g(l, false);
        }{
            __Guard g(l, true);
        }{
            __Guard g(p);
        }{
            __Guard g(p, false);
        }{
            __Guard g(p, true);
        }
    }{
        const __Lock & l = lock;
        const __Lock * p = &lock;
        {
            __Guard g(l);
        }{
            __Guard g(p);
        }
    }
}

template<class LockT>
static void testGuardImp()
{
    LockT lock;
    testGuardImp(lock);
}

template<class LockT>
static void testGuard()
{
    testGuardImp<LockT>();
    testGuardImp<volatile LockT>();
}

static void testGuardFileLock()
{
    CFileLock f("mutex_unittest.cc");
    testGuardImp(f);
    testGuardImp<volatile CFileLock>(f);
}
TEST(CGuard, all)
{
    testGuard<CMutex>();
    testGuard<CRWLock>();
    testGuard<CSpinLock>();
    testGuardFileLock();
}

int once;

static void testOnce()
{
    ++once;
}

static void * onceProc1(void * arg)
{
    assert(arg);
    COnceGuard & o = *static_cast<COnceGuard *>(arg);
    o.runOnce(testOnce);
    return NULL;
}

static void * onceProc2(void * arg)
{
    assert(arg);
    COnceGuard & o = *static_cast<COnceGuard *>(arg);
    o.runOnce();
    return NULL;
}

TEST(COnceGuard, all)
{
    {
        once = 0;
        COnceGuard o;
        std::vector<CThread> th(5);
        for(size_t i = 0;i < th.size();++i)
            th[i].start(onceProc1, &o);
        for(size_t i = 0;i < th.size();++i)
            th[i].join();
        ASSERT_EQ(1, once);
    }{
        once = 0;
        COnceGuard o(testOnce);
        std::vector<CThread> th(5);
        for(size_t i = 0;i < th.size();++i)
            th[i].start(onceProc2, &o);
        for(size_t i = 0;i < th.size();++i)
            th[i].join();
        ASSERT_EQ(1, once);
    }
}
