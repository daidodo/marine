#ifndef DOZERG_POSIX_SHM_IMPL_H_20130305
#define DOZERG_POSIX_SHM_IMPL_H_20130305

#include <string>
#include <sys/mman.h>

NS_IMPL_BEGIN

template<class F>
class CPosixShmImpl
{
    typedef CPosixShmImpl<F>    __Myt;
    typedef F                   __File;
public:
    CPosixShmImpl()
        : addr_(NULL)
        , sz_(0)
    {}
    ~CPosixShmImpl(){uninit();}
    //共享内存是否已初始化
    bool valid() const{return (NULL != addr_);}
    //TODO: sz = 0
    //初始化共享内存，如果没有filename，则为匿名映射
    //如果不存在且(readonly==false)，则会创建
    //filename: 文件名
    //sz:
    //  0       映射文件实际大小
    //  其他    映射内存字节大小，如果文件长度不足sz，文件会被扩展
    //readonly:
    //  true    需要只读权限
    //  false   需要读写权限
    //address: 建议映射到的虚拟地址
    //mode: 共享内存文件本身的权限
    //return:
    //  <0      出错
    //  0       shm已存在，映射成功
    //  >0      shm不存在，创建并映射成功
    int init(const std::string & filename, size_t sz, bool readonly, void * address, int mode){
        if(valid() || filename.empty())
            return -1;
        //open
        __File f;
        bool create = false;
        int flags = (readonly ? O_RDONLY : O_RDWR);
        assert(!filename.empty());
        if(!f.open(filename, flags)){
            if(readonly)
                return -1;
            if(!f.open(filename, flags | O_CREAT, mode))
                return -1;
            create = true;
        }
        //extend file size
        off_t realSz = f.length();
        if(realSz < 0)
            return -1;
        if(0 == sz)
            sz = realSz;
        if(sz > size_t(realSz) && !f.truncate(sz))
            return -1;
        //map
        int prot = PROT_READ;
        if(!readonly)
            prot |= PROT_WRITE;
        if(MAP_FAILED == (addr_ = ::mmap(address, sz, prot, mapFlags(false, sz), f.fd(), 0))){
            if(create)
                f.unlink();
            addr_ = NULL;
            return -1;
        }
        sz_ = sz;
        filename_ = filename;
        return (create ? 1 : 0);
    }
    int init(size_t sz, void * address){
        if(valid() || 0 == sz)
            return -1;
        if(MAP_FAILED == (addr_ = ::mmap(address, sz, PROT_READ | PROT_WRITE, mapFlags(true, sz), -1, 0))){
            addr_ = NULL;
            return -1;
        }
        sz_ = sz;
        return 1;
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
        return (on ?
                (0 == ::mlock(addr_, sz_)) :
                (0 == ::munlock(addr_, sz_)));
    }
    //标记共享内存文件可删除
    //注意: 当没有进程使用共享内存时，系统才会真正删除共享内存
    //return:
    //  true    成功
    //  false   失败
    bool setDeleted(){
        if(!valid() || filename_.empty())
            return false;
        return __File::Unlink(filename_);
    }
    //解除（但未删除）共享内存
    //注意: 解除后，对应内存地址可能不可访问
    void uninit(){
        if(valid()){
            ::munmap(addr_, sz_);
            filename_.clear();
            addr_ = NULL;
            sz_ = 0;
        }
    }
    //访问共享内存
    void * addr(){return addr_;}
    const void * addr() const{return addr_;}
    size_t size() const{return sz_;}
private:
    static int mapFlags(bool anony, size_t sz){
        int flags = (anony ? MAP_ANONYMOUS | MAP_PRIVATE : MAP_SHARED);
#ifdef MAP_HUGETLB
        if(sz >= (16 << 20))
            flags |= MAP_HUGETLB;
#endif
        return flags;
    }
    CPosixShmImpl(const CPosixShmImpl &);     //disable copy and assignment
    CPosixShmImpl & operator =(const CPosixShmImpl &);
    //fields
    std::string filename_;
    void * addr_;
    size_t sz_;
};

NS_IMPL_END

#endif

