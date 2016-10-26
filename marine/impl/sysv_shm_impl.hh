#ifndef DOZERG_SYSV_SHM_IMPL_H_20130304
#define DOZERG_SYSV_SHM_IMPL_H_20130304

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <cstddef>          //NULL
#include "environment.hh"

NS_IMPL_BEGIN

class CSysvShmImpl
{
public:
    CSysvShmImpl()
        : addr_(NULL)
        , sz_(0)
        , shmid_(-1)
    {}
    ~CSysvShmImpl(){uninit();}
    //共享内存是否已初始化
    bool valid() const{return (-1 != shmid_ && NULL != addr_);}
    //初始化共享内存
    //如果不存在且(readonly==false)，则会创建
    //key: 共享内存的key
    //sz: 共享内存字节大小
    //readonly:
    //  true    需要只读权限
    //  false   需要读写权限
    //address: 建议映射到的虚拟地址
    //mode: 共享内存本身的权限
    //return:
    //  <0      出错
    //  0       shm已存在，映射成功
    //  >0      shm不存在，创建并映射成功
    int init(key_t key, size_t sz, bool readonly, void * address, int mode){
        //check
        if(valid() || key < 0 || 0 == sz)
            return -1;
        //get
        int flags = 0;
#ifdef SHM_HUGETLB  //support huge tlb
        if(sz >= (16 << 20))
            flags |= SHM_HUGETLB;   //use huge tlb if sz >= 16MB
#endif
        bool created = (IPC_PRIVATE == key ? true : false);
        if(-1 == (shmid_ = ::shmget(key, sz, flags))){
            if(readonly)
                return -1;
            flags |= IPC_CREAT | (mode & 0x1FF);    //least significant 9 bits
            if(-1 == (shmid_ = ::shmget(key, sz, flags)))
                return -1;
            created = true;
        }
        //attach
        flags = 0;
        if(NULL != address)
            flags |= SHM_RND;
        if(readonly)
            flags |= SHM_RDONLY;
        if((void *)-1 == (addr_ = ::shmat(shmid_, address, flags))){
            if(created)
                setDeleted();
            shmid_ = -1;
            addr_ = NULL;
            return -1;
        }
        sz_ = sz;
        return (created ? 1 : 0);
    }
    //设置是否常驻物理内存
    //on:
    //  true    常驻内存
    //  false   不常驻内存
    //return:
    //  true    成功
    //  false   失败
    bool setResident(bool on){
        if(!valid())
            return false;
        return (0 == ::shmctl(shmid_, (on ? SHM_LOCK : SHM_UNLOCK), NULL));
    }
    //标记共享内存可删除
    //注意: 当没有进程使用共享内存时，系统才会真正删除共享内存
    //return:
    //  true    成功
    //  false   失败
    bool setDeleted(){
        if(!valid())
            return false;
        return (0 == ::shmctl(shmid_, IPC_RMID, NULL));
    }
    //解除（但未删除）共享内存
    //注意: 解除后，对应内存地址可能不可访问
    void uninit(){
        if(valid()){
            ::shmdt(addr_);
            shmid_ = -1;
            addr_ = NULL;
        }
    }
    //访问共享内存
    void * addr(){return addr_;}
    const void * addr() const{return addr_;}
    size_t size() const{return sz_;}
private:
    CSysvShmImpl(const CSysvShmImpl &);     //disable copy and assignment
    CSysvShmImpl & operator =(const CSysvShmImpl &);
    void * addr_;
    size_t sz_;
    int shmid_;
};

NS_IMPL_END

#endif

