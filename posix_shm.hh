#ifndef DOZERG_POSIX_SHM_H_20130325
#define DOZERG_POSIX_SHM_H_20130325

/*
    ��POSIX�����ڴ�ķ�װ
        CPosixShm
//*/

#include <errno.h>
#include "template.hh"      //CTypeSelector
#include "posix_shm_file.hh"
#include "tools/system.hh"
#include "impl/posix_shm_impl.hh"

NS_SERVER_BEGIN

//T: �����ڴ����Ķ�������
//RO: �����ڴ��Ƿ�ֻ��
//PF:
//  true    ʹ��POSIX shm�ļ�
//  false   ʹ����ͨ�ļ�
template<typename T, bool RO = false, bool PF = true>
class CPosixShm;

//ֻ�������ڴ�
template<typename T, bool PF>
class CPosixShm<T, true, PF>
{
    typedef typename CTypeSelector<CPosixShmFile, CFile, PF>::result_type __File;
    typedef NS_IMPL::CPosixShmImpl<__File> __Shm;
public:
    typedef T           value_type;
    typedef const T &   const_reference;
    typedef const T *   const_pointer;
    static const int kModeDefault = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;  //�����ڴ��Ӧ�ļ���Ĭ�϶�дȨ��
    //�����ϴ�ϵͳ��������
    static std::string ErrMsg(){return tools::ErrorMsg(errno);}
    //���캯��
    CPosixShm():sz_(0),readonly_(true){}
    //�����ڴ��Ƿ���ӳ��
    bool valid() const{return shm_.valid();}
    //TODO: sz = 0
    //������ӳ�乲���ڴ�
    //filename:
    //  if(PF==false)   ��ʾ�����ڴ��Ӧ����ͨ�ļ�·��
    //  if(PF==true)    ��ʾ�����ڴ��Ӧ��POSIX shm�ļ�����ʵ���ļ�·��������"/dev/shm/filename"
    //sz:
    //  0       �����ļ�ʵ�ʴ�Сӳ��
    //  ����    �������
    //address: ����ӳ�䵽�ĵ�ַ
    //mode: �ļ�����ķ���Ȩ��
    //return:
    //  <0      ����
    //  0       shm�Ѵ��ڣ�ӳ��ɹ�
    //  >0      shm�����ڣ�������ӳ��ɹ�
    int init(const std::string & filename, size_t sz = 0, void * address = NULL, int mode = kModeDefault){
        const int ret = shm_.init(filename, sizeof(value_type) * sz, readonly_, address, mode);
        if(ret < 0)
            return ret;
        sz_ = (sz ? sz : shm_.size() / sizeof(value_type));
        return ret;
    }
    //���������ڴ�
    int init(size_t sz, void * address = NULL){
        sz_ = sz;
        return shm_.init(sizeof(value_type) * sz_, address);
    }
    //�����Ƿ�פ�����ڴ�
    //on:
    //  true    ��פ�ڴ�
    //  false   ����פ�ڴ�
    //return:
    //  true    ���óɹ�
    //  false   ����ʧ��
    bool setResident(bool on){return shm_.setResident(on);}
    //��ǹ����ڴ��Ӧ���ļ���ɾ��
    //ע��: ��û�н���ӳ�䵽��Ӧ���ļ�ʱ��ϵͳ�Ż�����ɾ�������ڴ�
    //return:
    //  true    �ɹ�
    //  false   ʧ��
    bool setDeleted(){return shm_.setDeleted();}
    //�ͷŹ����ڴ��ַ�ռ䣬��ɾ��
    //ע��: �ͷź󣬶�Ӧ�ڴ��ַ���ܲ��ɷ���
    void uninit(){return shm_.uninit();}
    //��ȡ��������Ԫ�ظ���
    size_t size() const{return sz_;}
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
    explicit CPosixShm(bool readonly):readonly_(readonly){}
    //fields
    __Shm shm_;
    size_t sz_;
    bool readonly_;
};

//�ɶ�д�����ڴ�
template<typename T, bool PF>
class CPosixShm<T, false, PF> : public CPosixShm<T, true, PF>
{
    typedef CPosixShm<T, true, PF> __MyBase;
public:
    typedef T & reference;
    typedef T * pointer;
    CPosixShm():__MyBase(false){}
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

