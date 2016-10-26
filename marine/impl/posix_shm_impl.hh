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
    //�����ڴ��Ƿ��ѳ�ʼ��
    bool valid() const{return (NULL != addr_);}
    //TODO: sz = 0
    //��ʼ�������ڴ棬���û��filename����Ϊ����ӳ��
    //�����������(readonly==false)����ᴴ��
    //filename: �ļ���
    //sz:
    //  0       ӳ���ļ�ʵ�ʴ�С
    //  ����    ӳ���ڴ��ֽڴ�С������ļ����Ȳ���sz���ļ��ᱻ��չ
    //readonly:
    //  true    ��Ҫֻ��Ȩ��
    //  false   ��Ҫ��дȨ��
    //address: ����ӳ�䵽�������ַ
    //mode: �����ڴ��ļ������Ȩ��
    //return:
    //  <0      ����
    //  0       shm�Ѵ��ڣ�ӳ��ɹ�
    //  >0      shm�����ڣ�������ӳ��ɹ�
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
        return (on ?
                (0 == ::mlock(addr_, sz_)) :
                (0 == ::munlock(addr_, sz_)));
    }
    //��ǹ����ڴ��ļ���ɾ��
    //ע��: ��û�н���ʹ�ù����ڴ�ʱ��ϵͳ�Ż�����ɾ�������ڴ�
    //return:
    //  true    �ɹ�
    //  false   ʧ��
    bool setDeleted(){
        if(!valid() || filename_.empty())
            return false;
        return __File::Unlink(filename_);
    }
    //�������δɾ���������ڴ�
    //ע��: ����󣬶�Ӧ�ڴ��ַ���ܲ��ɷ���
    void uninit(){
        if(valid()){
            ::munmap(addr_, sz_);
            filename_.clear();
            addr_ = NULL;
            sz_ = 0;
        }
    }
    //���ʹ����ڴ�
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

