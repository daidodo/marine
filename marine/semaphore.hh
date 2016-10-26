#ifndef DOZERG_SEMAPHORE_H_20090318
#define DOZERG_SEMAPHORE_H_20090318

/*
    对POSIX信号量进行简单的封装
    方便使用,隐藏底层实现,便于移植
        CSemaphore          POSIX有名信号量
        CUnnamedSemaphore   POSIX无名信号量
        CXsiSemaphoreSet    XSI信号量集
//*/

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <fcntl.h>
#include <semaphore.h>
#include <errno.h>
#include <cassert>
#include <vector>
#include "tools/time.hh"

NS_SERVER_BEGIN

class CUnnamedSemaphore
{
    CUnnamedSemaphore(const CUnnamedSemaphore &);     //disable copy and assignment
    CUnnamedSemaphore & operator =(const CUnnamedSemaphore &);
public:
    CUnnamedSemaphore(){}
    //init unnamed semaphore
    //init_val: init value of Semaphore
    //pshared: shared in process(true) or threads(false)
    //注意：
    //如果pshared=true，那么对象应该位于shm中，否则无法实现进程间共享；
    bool init(unsigned int init_val, bool pshared){
        return (0 == ::sem_init(&sem_, pshared, init_val));
    }
    //destroy unnamed semaphore
    //注意：
    //本函数可由任何进程/线程调用，但需要与init调用匹配，否则可能造成不确定结果
    bool destroy(){
        return (0 == ::sem_destroy(&sem_));
    }
    //unlock semaphore
    bool post(){return (0 == ::sem_post(&sem_));}
    //lock semaphore
    bool wait(){return (0 == ::sem_wait(&sem_));}
    //try to lock semaphore
    bool tryWait(){return (0 == ::sem_trywait(&sem_));}
#ifdef __API_HAS_SEM_TIMEWAIT
    //在指定的timeMs毫秒内如果不能获取信号量,返回false
    bool timeWait(uint32_t timeMs){
        struct timespec ts;
        return (tools::GetAbsTimespec(timeMs, &ts)
                && 0 == ::sem_timedwait(&sem_, &ts));
    }
#endif
    //获取信号量的当前值；返回-1表示获取失败
    int getVal() const{
        int ret = -1;
        if(0 == ::sem_getvalue(&sem_, &ret) && ret < 0)
            ret = 0;
        return ret;
    }
private:
    mutable sem_t sem_;
};

class CSemaphore
{
    CSemaphore(const CSemaphore &);     //disable copy and assignment
    CSemaphore & operator =(const CSemaphore &);
    static const int kModeDefault = S_IRUSR | S_IWUSR;// | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
public:
    //remove named semaphore
    //TODO: unit test
    static bool Unlink(const char * name){
        assert(name);
        return (0 == sem_unlink(name));
    }
    //init named semaphore
    //name: file name of Semaphore
    //init_val: init value of Semaphore
    //oflag: for sem_open()
    //mode: for sem_open()
    explicit CSemaphore(const char * name
            , unsigned int init_val = 0
            , int oflag = O_CREAT
            , mode_t mode = kModeDefault)
        : semp_(NULL)
    {
        //注意：
        //如果遇到sem_open()失败，而错误为"No such file or directory"失败，原因可能是name里有目录
        //因为sem_open试图创建/dev/shm/sem.name文件，如果name里有目录，则会因为目录不存在而失败
        sem_t * s = sem_open(name, oflag, mode, init_val);
        if(SEM_FAILED != s)
            semp_ = s;
    }
    ~CSemaphore(){
        if(semp_)
            sem_close(semp_);
        semp_ = NULL;
    }
    bool valid() const{return (semp_ != NULL);}
    //unlock semaphore(++sem_val)
    bool post(){return (valid() && 0 == sem_post(semp_));}
    //lock semaphore(--sem_val)
    bool wait(){return (valid() && 0 == sem_wait(semp_));}
    //try to lock semaphore
    bool tryWait(){return (valid() && 0 == sem_trywait(semp_));}
#ifdef __API_HAS_SEM_TIMEWAIT
    //在指定的timeMs毫秒内如果不能获取信号量,返回false
    bool timeWait(uint32_t timeMs){
        struct timespec ts;
        return (valid()
                && tools::GetAbsTimespec(timeMs, &ts)
                && 0 == sem_timedwait(semp_, &ts));
    }
#endif
    //获取信号量的当前值；返回-1表示获取失败
    int getVal() const{
        int ret = -1;
        if(valid() && 0 == sem_getvalue(semp_, &ret) && ret < 0)
            ret = 0;
        return ret;
    }
private:
    sem_t * semp_;
};

class CXsiSemaphoreSet
{
    union semun {
        int              val;    // Value for SETVAL
        struct semid_ds *buf;    // Buffer for IPC_STAT, IPC_SET
        unsigned short  *array;  // Array for GETALL, SETALL
        struct seminfo  *__buf;  // Buffer for IPC_INFO (Linux-specific)
    };
public:
    static const int kFlagDefault = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
    //generate an operation
    static sembuf GenOp(int index, int op, bool wait, bool undo){
        sembuf sb;
        sb.sem_num = index;
        sb.sem_op = op;
        sb.sem_flg = 0;
        if(!wait)
            sb.sem_flg |= IPC_NOWAIT;
        if(undo)
            sb.sem_flg |= SEM_UNDO;
        return sb;
    }
    CXsiSemaphoreSet():semid_(-1){}
    //init semaphore set
    //key: key for semaphore set
    //nsems:
    //  0       ignored
    //  other   number of semaphores in set
    //semflg: access flags for semaphores in set
    explicit CXsiSemaphoreSet(key_t key, int nsems = 0, int semflg = kFlagDefault)
        : semid_(-1)
    {
        init(key, nsems, semflg);
    }
    //pathname: file pathname for semaphore set, must be existing and accessible
    //project: project identifier for semaphore set
    CXsiSemaphoreSet(const char * pathname, int project, int nsems = 0, int semflg = kFlagDefault)
        : semid_(-1)
    {
        init(pathname, project, nsems, semflg);
    }
    bool init(key_t key, int nsems, int semflg){
        if(valid())
            return false;
        semid_ = ::semget(key, nsems, semflg);
        return valid();
    }
    bool init(const char * pathname, int project, int nsems, int semflg){
        if(valid())
            return false;
        key_t key = ::ftok(pathname, project);
        if(-1 == key)
            return false;
        return init(key, nsems, semflg);
    }
    bool valid() const{return (semid_ >= 0);}
    //get number of semaphores in set
    size_t size() const{
        if(!valid())
            return 0;
        struct semid_ds sem;
        union semun arg;
        arg.buf = &sem;
        if(0 > ::semctl(semid_, 0, IPC_STAT, arg))
            return 0;
        return sem.sem_nsems;
    }
    //get all semvals in semaphore set
    bool getAll(std::vector<unsigned short> & results) const{
        if(!valid())
            return false;
        const size_t sz = size();
        if(!sz)
            return false;
        results.resize(sz);
        union semun arg;
        arg.array = &results[0];
        return (0 == ::semctl(semid_, 0, GETALL, arg));
    }
    //set semval of all semaphores in set
    bool setAll(const std::vector<unsigned short> & values){
        if(!valid())
            return false;
        const size_t sz = size();
        if(!sz || values.size() < sz)
            return false;
        union semun arg;
        arg.array = const_cast<unsigned short *>(&values[0]);
        return (0 == ::semctl(semid_, 0, SETALL, arg));
    }
    //set semval of one semaphone in set
    bool setVal(int index, int val){
        if(!valid())
            return false;
        union semun arg;
        arg.val = val;
        return (0 == ::semctl(semid_, index, SETVAL, arg));
    }
    //get semval of one semaphone in set
    //return:
    //  <0      failed
    //  others  semval
    int getVal(int index) const{
        if(!valid())
            return -1;
        return ::semctl(semid_, index, GETVAL);
    }
    //apply operation(s) on semaphore(s) in set
    //index: index of semaphore in set
    //op: increment or decrement for semval; if ZERO, then wait for semval=0
    //wait: wait or not if operaton suspended
    //undo: undo or not when process exit
    bool apply(int index, int op, bool wait, bool undo){
        if(!valid())
            return false;
        sembuf sb(GenOp(index, op, wait, undo));
        return (0 == ::semop(semid_, &sb, 1));
    }
    //ops: operations array
    bool apply(const std::vector<sembuf> & ops){
        if(!valid())
            return false;
        if(ops.empty())
            return true;
        return (0 == ::semop(semid_, const_cast<sembuf *>(&ops[0]), ops.size()));
    }
#ifdef __API_HAS_SEMTIMEDOP
    //timeMs: if suspended, wait for timeMs milli-seconds
    bool timeApply(int index, int op, bool wait, bool undo, uint32_t timeMs){
        if(!valid())
            return false;
        sembuf sb(GenOp(index, op, wait, undo));
        struct timespec ts;
        tools::GetRelativeTimespec(timeMs, &ts);
        return (0 == ::semtimedop(semid_, &sb, 1, &ts));
    }
    bool timeApply(const std::vector<sembuf> & ops, uint32_t timeMs){
        if(!valid())
            return false;
        if(ops.empty())
            return true;
        struct timespec ts;
        tools::GetRelativeTimespec(timeMs, &ts);
        return (0 == ::semtimedop(semid_, const_cast<sembuf *>(&ops[0]), ops.size(), &ts));
    }
#endif
    //destroy semaphore set from system
    void destroy(){
        if(valid()){
            ::semctl(semid_, 0, IPC_RMID);
            semid_ = -1;
        }
    }
private:
    int semid_;
};

NS_SERVER_END

#endif

