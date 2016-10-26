#ifndef DOZERG_SYSV_SHM_H_20130301
#define DOZERG_SYSV_SHM_H_20130301

/*
    对SysV共享内存的封装
        CSysvShm
//*/

#include <errno.h>
#include <cassert>
#include "tools/system.hh"
#include "impl/sysv_shm_impl.hh"

NS_SERVER_BEGIN

//T: 共享内存代表的对象类型
//N: 共享内存包含的对象个数
//RO: 共享内存是否只读
template<typename T, size_t N = 1, bool RO = false>
class CSysvShm;

//只读共享内存
template<typename T, size_t N>
class CSysvShm<T, N, true>
{
    typedef NS_IMPL::CSysvShmImpl __Shm;
public:
    typedef T           value_type;
    typedef const T &   const_reference;
    typedef const T *   const_pointer;
    static const int kModeDefault = SHM_R | SHM_W;  //共享内存的默认读写权限
    static const size_t kSize = N;  //对象数组的元素个数
    //返回上次系统错误描述
    static std::string ErrMsg(){return tools::ErrorMsg(errno);}
    //构造函数
    CSysvShm():readonly_(true){}
    //共享内存是否已attach
    bool valid() const{return shm_.valid();}
    //创建并attach共享内存
    //pathname: 生成key的文件名
    //projId: 生成key的项目id
    //key: 共享内存的key
    //address: 建议attach到的地址
    //mode: 共享内存本身的权限
    //return:
    //  <0      出错
    //  0       shm已存在，attach成功
    //  >0      shm不存在，创建并attach成功
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
    //设置是否常驻物理内存
    //on:
    //  true    常驻内存
    //  false   不常驻内存
    //return:
    //  true    设置成功
    //  false   设置失败
    bool setResident(bool on){return shm_.setResident(on);}
    //标记共享内存可删除
    //注意: 当没有进程attach到共享内存时，系统才会真正删除共享内存
    //return:
    //  true    成功
    //  false   失败
    bool setDeleted(){return shm_.setDeleted();}
    //detach共享内存
    //注意: detach后，对应内存地址可能不可访问
    void uninit(){return shm_.uninit();}
    //获取对象数组元素个数
    size_t size() const{return kSize;}
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
    explicit CSysvShm(bool readonly):readonly_(readonly){}
    //fields
    __Shm shm_;
    bool readonly_;
};

//可读写共享内存
template<typename T, size_t N>
class CSysvShm<T, N, false> : public CSysvShm<T, N, true>
{
    typedef CSysvShm<T, N, true> __MyBase;
public:
    typedef T & reference;
    typedef T * pointer;
    CSysvShm():__MyBase(false){}
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

