#include <marine/semaphore.hh>

#include "../inc.h"
#include <marine/tools/system.hh>

template<class S>
static bool testSemOp(S & sem)
{
    int v = sem.getVal();
    if(1 != v){
        cerr<<"1: sem.getVal()="<<v<<" is not 1\n";
        return false;
    }
    sem.wait();
    v = sem.getVal();
    if(0 != v){
        cerr<<"2: sem.getVal()="<<v<<" is not 0\n";
        return false;
    }
    if(sem.tryWait()){
        cerr<<"sem.tryWait() should fail but succ\n";
        return false;
    }
#ifdef __API_HAS_SEM_TIMEWAIT
    //在指定的timeMs毫秒内如果不能获取信号量,返回false
    if(sem.timeWait(1)){
        cerr<<"sem.timeWait(1) should fail but succ\n";
        return false;
    }
#endif
    //在指定的timeMs毫秒内如果不能获取信号量,返回false
    sem.post();
    v = sem.getVal();
    if(1 != v){
        cerr<<"3: sem.getVal()="<<v<<" is not 1\n";
        return false;
    }
    return true;
}

TEST(CUnnamedSemaphore, all)
{
    CUnnamedSemaphore sem;
    ASSERT_TRUE(sem.init(1, false));
    EXPECT_TRUE(testSemOp(sem));
    EXPECT_TRUE(sem.destroy());

    EXPECT_TRUE(sem.init(1, true));
    EXPECT_TRUE(testSemOp(sem));
    EXPECT_TRUE(sem.destroy());
}

TEST(CSemaphore, all)
{
    CSemaphore sem("sem_test", 1);
    ASSERT_TRUE(sem.valid())<<tools::ErrorMsg(errno);
    EXPECT_TRUE(testSemOp(sem));
    EXPECT_TRUE(CSemaphore::Unlink("sem_test"));
}

TEST(CXsiSemaphoreSet, GenOp)
{
    vector<sembuf> ops;
    ops.push_back(CXsiSemaphoreSet::GenOp(0, 1, false, false));
}

const char * fname = "semaphore_unittest.cc";
const key_t key = ::ftok(fname, 1);
const key_t undo_key = ::ftok(fname, 2);

struct CEnv : public ::testing::Environment
{
    void SetUp(){
        CXsiSemaphoreSet sem(key);
        sem.destroy();
    }
};

__ADD_GTEST_ENV(CEnv, CXsiSemaphoreSet);

TEST(CXsiSemaphoreSet, ctor)
{
    ASSERT_GT(key, -1);
    {
        CXsiSemaphoreSet sem;
        ASSERT_FALSE(sem.valid());
    }{
        CXsiSemaphoreSet sem(key);
        ASSERT_FALSE(sem.valid());
    }{
        CXsiSemaphoreSet sem(key, 1);
        ASSERT_FALSE(sem.valid());
    }{
        CXsiSemaphoreSet sem(key, 1, 0);
        ASSERT_FALSE(sem.valid());
    }{
        CXsiSemaphoreSet sem(key, 1, 0);
        ASSERT_FALSE(sem.valid());
    }{
        CXsiSemaphoreSet sem(key, 10, IPC_CREAT | IPC_EXCL | CXsiSemaphoreSet::kFlagDefault);
        ASSERT_TRUE(sem.valid());

        sem.destroy();
    }{
        CXsiSemaphoreSet sem(fname, 1);
        ASSERT_FALSE(sem.valid());
    }{
        CXsiSemaphoreSet sem(fname, 1, 1);
        ASSERT_FALSE(sem.valid());
    }{
        CXsiSemaphoreSet sem(fname, 1, 1, 0);
        ASSERT_FALSE(sem.valid());
    }{
        CXsiSemaphoreSet sem(fname, 1, 10, IPC_CREAT | IPC_EXCL | CXsiSemaphoreSet::kFlagDefault);
        ASSERT_TRUE(sem.valid());

        sem.destroy();
    }
}

TEST(CXsiSemaphoreSet, init)
{
    ASSERT_GT(key, -1);
    {
        CXsiSemaphoreSet sem;
        ASSERT_FALSE(sem.valid());

        ASSERT_TRUE(sem.init(key, 10, IPC_CREAT | IPC_EXCL | CXsiSemaphoreSet::kFlagDefault));
        ASSERT_TRUE(sem.valid());

        sem.destroy();
    }{
        CXsiSemaphoreSet sem(key);
        ASSERT_FALSE(sem.valid());

        ASSERT_TRUE(sem.init(fname, 1, 10, IPC_CREAT | IPC_EXCL | CXsiSemaphoreSet::kFlagDefault));
        ASSERT_TRUE(sem.valid());

        sem.destroy();
    }
}

TEST(CXsiSemaphoreSet, size)
{
    ASSERT_GT(key, -1);

    CXsiSemaphoreSet sem;
    ASSERT_FALSE(sem.valid());
    ASSERT_EQ(size_t(0), sem.size());

    ASSERT_TRUE(sem.init(key, 10, IPC_CREAT | IPC_EXCL | CXsiSemaphoreSet::kFlagDefault));
    ASSERT_TRUE(sem.valid());
    ASSERT_EQ(size_t(10), sem.size());

    sem.destroy();
}

TEST(CXsiSemaphoreSet, get_setVal)
{
    ASSERT_GT(key, -1);

    CXsiSemaphoreSet sem;
    ASSERT_FALSE(sem.valid());
    ASSERT_FALSE(sem.setVal(0, 1));
    ASSERT_EQ(-1, sem.getVal(0));

    ASSERT_TRUE(sem.init(key, 10, IPC_CREAT | IPC_EXCL | CXsiSemaphoreSet::kFlagDefault));
    ASSERT_TRUE(sem.valid());
    ASSERT_EQ(-1, sem.getVal(11));
    ASSERT_EQ(0, sem.getVal(0));
    ASSERT_TRUE(sem.setVal(0, 1));
    ASSERT_EQ(1, sem.getVal(0));

    sem.destroy();
}

TEST(CXsiSemaphoreSet, getAll_setAll)
{
    ASSERT_GT(key, -1);
    vector<unsigned short> vals, ret;
    for(int i = 0;i < 10;++i)
        vals.push_back(i);

    CXsiSemaphoreSet sem;
    ASSERT_FALSE(sem.valid());
    ASSERT_FALSE(sem.setAll(vals));
    ASSERT_FALSE(sem.getAll(ret));

    ASSERT_TRUE(sem.init(key, 10, IPC_CREAT | IPC_EXCL | CXsiSemaphoreSet::kFlagDefault));
    ASSERT_TRUE(sem.valid());
    ASSERT_TRUE(sem.getAll(ret));
    ASSERT_EQ(size_t(10), ret.size());
    for(int i = 0;i < 10;++i)
        ASSERT_EQ(0, ret[i]);

    ret.clear();
    ASSERT_TRUE(sem.setAll(vals));
    ASSERT_TRUE(sem.getAll(ret));
    ASSERT_EQ(vals, ret);

    sem.destroy();
}

TEST(CXsiSemaphoreSet, apply)
{
    ASSERT_GT(key, -1);
    vector<sembuf> ops;

    ops.push_back(CXsiSemaphoreSet::GenOp(0, 0, true, false));
    ops.push_back(CXsiSemaphoreSet::GenOp(0, 1, true, false));
    ops.push_back(CXsiSemaphoreSet::GenOp(0, 1, true, false));
    ops.push_back(CXsiSemaphoreSet::GenOp(0, -1, true, false));
    ops.push_back(CXsiSemaphoreSet::GenOp(0, -1, true, false));

    CXsiSemaphoreSet sem;
    ASSERT_FALSE(sem.valid());
    ASSERT_FALSE(sem.apply(0, 0, true, false));
    ASSERT_FALSE(sem.apply(ops));

    ASSERT_TRUE(sem.init(key, 10, IPC_CREAT | IPC_EXCL | CXsiSemaphoreSet::kFlagDefault));
    ASSERT_TRUE(sem.valid());
    ASSERT_TRUE(sem.apply(0, 0, true, false));
    ASSERT_TRUE(sem.apply(0, 1, true, false));
    ASSERT_TRUE(sem.apply(0, 1, true, false));
    ASSERT_TRUE(sem.apply(0, -1, true, false));
    ASSERT_TRUE(sem.apply(0, -1, true, false));
    ASSERT_FALSE(sem.apply(0, -1, false, false));

    ASSERT_TRUE(sem.apply(ops));
    ops.push_back(CXsiSemaphoreSet::GenOp(0, -1, false, false));
    ASSERT_FALSE(sem.apply(ops));

    sem.destroy();
}

TEST(CXsiSemaphoreSet, apply_undo)
{
    ASSERT_GT(undo_key, -1);
    //cout<<"undo_key = 0x"<<hex<<undo_key<<endl;
    vector<sembuf> ops;
    ops.push_back(CXsiSemaphoreSet::GenOp(0, -1, false, true));

    CXsiSemaphoreSet sem;
    if(!sem.init(undo_key, 1, CXsiSemaphoreSet::kFlagDefault)){
        ASSERT_TRUE(sem.init(undo_key, 1, IPC_CREAT | IPC_EXCL | CXsiSemaphoreSet::kFlagDefault));
        ASSERT_TRUE(sem.valid());
        ASSERT_TRUE(sem.setVal(0, 1));
    }
    ASSERT_TRUE(sem.valid());
    ASSERT_EQ(1, sem.getVal(0))<<"undo_key=0x"<<hex<<undo_key;

    ASSERT_TRUE(sem.apply(0, -1, true, true));
    ASSERT_EQ(0, sem.getVal(0));
}

TEST(CXsiSemaphoreSet, timeApply)
{
    ASSERT_GT(key, -1);
    vector<sembuf> ops;

    ops.push_back(CXsiSemaphoreSet::GenOp(0, 0, true, false));
    ops.push_back(CXsiSemaphoreSet::GenOp(0, 1, true, false));
    ops.push_back(CXsiSemaphoreSet::GenOp(0, 1, true, false));
    ops.push_back(CXsiSemaphoreSet::GenOp(0, -1, true, false));
    ops.push_back(CXsiSemaphoreSet::GenOp(0, -1, true, false));

    CXsiSemaphoreSet sem;
    ASSERT_FALSE(sem.valid());
    ASSERT_FALSE(sem.timeApply(0, 0, true, false, 100));
    ASSERT_FALSE(sem.timeApply(ops, 100));

    ASSERT_TRUE(sem.init(key, 10, IPC_CREAT | IPC_EXCL | CXsiSemaphoreSet::kFlagDefault));
    ASSERT_TRUE(sem.valid());
    ASSERT_TRUE(sem.timeApply(0, 0, true, false, 100));
    ASSERT_TRUE(sem.timeApply(0, 1, true, false, 100));
    ASSERT_TRUE(sem.timeApply(0, 1, true, false, 100));
    ASSERT_TRUE(sem.timeApply(0, -1, true, false, 100));
    ASSERT_TRUE(sem.timeApply(0, -1, true, false, 100));
    time_t now = time(NULL);
    ASSERT_FALSE(sem.timeApply(0, -1, true, false, 0));
    ASSERT_EQ(time(NULL), now);
    ASSERT_FALSE(sem.timeApply(0, -1, true, false, 1000));
    ASSERT_EQ(time(NULL), 1 + now);

    ASSERT_TRUE(sem.timeApply(ops, 100));
    ops.push_back(CXsiSemaphoreSet::GenOp(0, -1, true, false));
    now = time(NULL);
    ASSERT_FALSE(sem.timeApply(ops, 0));
    ASSERT_EQ(time(NULL), now);
    ASSERT_FALSE(sem.timeApply(ops, 1000));
    ASSERT_EQ(time(NULL), 1 + now);

    sem.destroy();
}
