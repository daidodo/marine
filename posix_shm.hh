#ifndef DOZERG_POSIX_SHM_H_20130325
#define DOZERG_POSIX_SHM_H_20130325

/*
    对POSIX共享内存的封装
        CPosixShm
//*/

#include <errno.h>
#include "template.hh"      //CTypeSelector
#include "posix_shm_file.hh"
#include "tools/system.hh"
#include "impl/posix_shm_impl.hh"

NS_SERVER_BEGIN

//T: 共享内存代表的对象类型
//RO: 共享内存是否只读
//PF:
//  true    使用POSIX shm文件
//  false   使用普通文件
template<typename T, bool RO = false, bool PF = true>
class CPosixShm;

//只读共享内存
template<typename T, bool PF>
class CPosixShm<T, true, PF>
{
    typedef typename CTypeSelector<CPosixShmFile, CFile, PF>::result_type __File;
    typedef NS_IMPL::CPosixShmImpl<__File> __Shm;
public:
    typedef T           value_type;
    typedef const T &   const_reference;
    typedef const T *   const_pointer;
    static const int kModeDefault = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;  //共享内存对应文件的默认读写权限
    //返回上次系统错误描述
    static std::string ErrMsg(){return tools::ErrorMsg(errno);}
    //构造函数
    CPosixShm():sz_(0),readonly_(true){}
    //共享内存是否已映射
    bool valid() const{return shm_.valid();}
    //TODO: sz = 0
    //创建并映射共享内存
    //filename:
    //  if(PF==false)   表示共享内存对应的普通文件路径
    //  if(PF==true)    表示共享内存对应的POSIX shm文件名，实际文件路径可能是"/dev/shm/filename"
    //sz:
    //  0       根据文件实际大小映射
    //  其他    对象个数
    //address: 建议映射到的地址
    //mode: 文件本身的访问权限
    //return:
    //  <0      出错
    //  0       shm已存在，映射成功
    //  >0      shm不存在，创建并映射成功
    int init(const std::string & filename, size_t sz = 0, void * address = NULL, int mode = kModeDefault){
        const int ret = shm_.init(filename, sizeof(value_type) * sz, readonly_, address, mode);
        if(ret < 0)
            return ret;
        sz_ = (sz ? sz : shm_.size() / sizeof(value_type));
        return ret;
    }
    //匿名共享内存
    int init(size_t sz, void * address = NULL){
        sz_ = sz;
        return shm_.init(sizeof(value_type) * sz_, address);
    }
    //设置是否常驻物理内存
    //on:
    //  true    常驻内存
    //  false   不常驻内存
    //return:
    //  true    设置成功
    //  false   设置失败
    bool setResident(bool on){return shm_.setResident(on);}
    //标记共享内存对应的文件可删除
    //注意: 当没有进程映射到对应的文件时，系统才会真正删除共享内存
    //return:
    //  true    成功
    //  false   失败
    bool setDeleted(){return shm_.setDeleted();}
    //释放共享内存地址空间，不删除
    //注意: 释放后，对应内存地址可能不可访问
    void uninit(){return shm_.uninit();}
    //获取对象数组元素个数
    size_t size() const{return sz_;}
    //访问共享内存代表的对象数组
    const_pointer ptr() const{
        return reinterpret_cast<const_pointer>(shm_.addr());
    }
    //通过索引访问对象
    //index: 索引
    const_reference operator [](size_t index) const{return ptr()[index];}
    //访问第一个对象
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

//可读写共享内存
template<typename T, bool PF>
class CPosixShm<T, false, PF> : public CPosixShm<T, true, PF>
{
    typedef CPosixShm<T, true, PF> __MyBase;
public:
    typedef T & reference;
    typedef T * pointer;
    CPosixShm():__MyBase(false){}
    //访问共享内存代表的对象数组
    using __MyBase::ptr;
    pointer ptr(){
        return reinterpret_cast<pointer>(__MyBase::shm_.addr());
    }
    //通过索引访问对象
    //index: 索引
    using __MyBase::operator [];
    reference operator [](size_t index){return ptr()[index];}
    //访问第一个对象
    using __MyBase::front;
    reference front(){
        assert(__MyBase::valid());
        return ptr()[0];
    }
};

NS_SERVER_END

#endif

