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
    //�����ڴ��Ƿ��ѳ�ʼ��
    bool valid() const{return (-1 != shmid_ && NULL != addr_);}
    //��ʼ�������ڴ�
    //�����������(readonly==false)����ᴴ��
    //key: �����ڴ��key
    //sz: �����ڴ��ֽڴ�С
    //readonly:
    //  true    ��Ҫֻ��Ȩ��
    //  false   ��Ҫ��дȨ��
    //address: ����ӳ�䵽�������ַ
    //mode: �����ڴ汾���Ȩ��
    //return:
    //  <0      ����
    //  0       shm�Ѵ��ڣ�ӳ��ɹ�
    //  >0      shm�����ڣ�������ӳ��ɹ�
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
    //�����Ƿ�פ�����ڴ�
    //on:
    //  true    ��פ�ڴ�
    //  false   ����פ�ڴ�
    //return:
    //  true    �ɹ�
    //  false   ʧ��
    bool setResident(bool on){
        if(!valid())
            return false;
        return (0 == ::shmctl(shmid_, (on ? SHM_LOCK : SHM_UNLOCK), NULL));
    }
    //��ǹ����ڴ��ɾ��
    //ע��: ��û�н���ʹ�ù����ڴ�ʱ��ϵͳ�Ż�����ɾ�������ڴ�
    //return:
    //  true    �ɹ�
    //  false   ʧ��
    bool setDeleted(){
        if(!valid())
            return false;
        return (0 == ::shmctl(shmid_, IPC_RMID, NULL));
    }
    //�������δɾ���������ڴ�
    //ע��: ����󣬶�Ӧ�ڴ��ַ���ܲ��ɷ���
    void uninit(){
        if(valid()){
            ::shmdt(addr_);
            shmid_ = -1;
            addr_ = NULL;
        }
    }
    //���ʹ����ڴ�
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

