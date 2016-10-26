#ifndef DOZERG_SYSV_SHM_H_20130301
#define DOZERG_SYSV_SHM_H_20130301

/*
    ��SysV�����ڴ�ķ�װ
        CSysvShm
//*/

#include <errno.h>
#include <cassert>
#include "tools/system.hh"
#include "impl/sysv_shm_impl.hh"

NS_SERVER_BEGIN

//T: �����ڴ����Ķ�������
//N: �����ڴ�����Ķ������
//RO: �����ڴ��Ƿ�ֻ��
template<typename T, size_t N = 1, bool RO = false>
class CSysvShm;

//ֻ�������ڴ�
template<typename T, size_t N>
class CSysvShm<T, N, true>
{
    typedef NS_IMPL::CSysvShmImpl __Shm;
public:
    typedef T           value_type;
    typedef const T &   const_reference;
    typedef const T *   const_pointer;
    static const int kModeDefault = SHM_R | SHM_W;  //�����ڴ��Ĭ�϶�дȨ��
    static const size_t kSize = N;  //���������Ԫ�ظ���
    //�����ϴ�ϵͳ��������
    static std::string ErrMsg(){return tools::ErrorMsg(errno);}
    //���캯��
    CSysvShm():readonly_(true){}
    //�����ڴ��Ƿ���attach
    bool valid() const{return shm_.valid();}
    //������attach�����ڴ�
    //pathname: ����key���ļ���
    //projId: ����key����Ŀid
    //key: �����ڴ��key
    //address: ����attach���ĵ�ַ
    //mode: �����ڴ汾���Ȩ��
    //return:
    //  <0      ����
    //  0       shm�Ѵ��ڣ�attach�ɹ�
    //  >0      shm�����ڣ�������attach�ɹ�
    int init(const std::string & pathname, int projId, void * address = NULL, int mode = kModeDefault){
        if(pathname.empty())
            return -1;
        key_t key = ::ftok(pathname.c_str(), projId);
        if(key < 0)
            return -1;
        return init(key, address, mode);
    }
    int init(key_t key = IPC_PRIVATE, void * address = NULL, int mode = kModeDefault){
        return shm_.init(key, sizeof(value_type) * kSize, readonly_, address, mode);
    }
    //�����Ƿ�פ�����ڴ�
    //on:
    //  true    ��פ�ڴ�
    //  false   ����פ�ڴ�
    //return:
    //  true    ���óɹ�
    //  false   ����ʧ��
    bool setResident(bool on){return shm_.setResident(on);}
    //��ǹ����ڴ��ɾ��
    //ע��: ��û�н���attach�������ڴ�ʱ��ϵͳ�Ż�����ɾ�������ڴ�
    //return:
    //  true    �ɹ�
    //  false   ʧ��
    bool setDeleted(){return shm_.setDeleted();}
    //detach�����ڴ�
    //ע��: detach�󣬶�Ӧ�ڴ��ַ���ܲ��ɷ���
    void uninit(){return shm_.uninit();}
    //��ȡ��������Ԫ�ظ���
    size_t size() const{return kSize;}
    //���ʹ����ڴ����Ķ�������
    const_pointer ptr() const{
        return reinterpret_cast<const_pointer>(shm_.addr());
    }
    //ͨ���������ʶ���
    //index: ����
    const_reference operator [](size_t index) const{return ptr()[index];}
    //���ʵ�һ������
    const_reference front() const{
        assert(valid());
        return ptr()[0];
    }
protected:
    explicit CSysvShm(bool readonly):readonly_(readonly){}
    //fields
    __Shm shm_;
    bool readonly_;
};

//�ɶ�д�����ڴ�
template<typename T, size_t N>
class CSysvShm<T, N, false> : public CSysvShm<T, N, true>
{
    typedef CSysvShm<T, N, true> __MyBase;
public:
    typedef T & reference;
    typedef T * pointer;
    CSysvShm():__MyBase(false){}
    //���ʹ����ڴ����Ķ�������
    using __MyBase::ptr;
    pointer ptr(){
        return reinterpret_cast<pointer>(__MyBase::shm_.addr());
    }
    //ͨ���������ʶ���
    //index: ����
    using __MyBase::operator [];
    reference operator [](size_t index){return ptr()[index];}
    //���ʵ�һ������
    using __MyBase::front;
    reference front(){
        assert(__MyBase::valid());
        return ptr()[0];
    }
};

NS_SERVER_END

#endif

