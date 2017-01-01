#ifndef DOZERG_POSIX_SHM_FILE_H_20130305
#define DOZERG_POSIX_SHM_FILE_H_20130305

#include <sys/mman.h>
#include <sys/stat.h>
#include "file.hh"

NS_SERVER_BEGIN

class CPosixShmFile : public CFile
{
    typedef CFile __MyBase;
public:
    static const int kFdType = 5;
    //删除指定的文件
    //shmname: 共享文件名，应该以'/'开头，例如"/shmname"，实际位置可能是"/dev/shm/shmname"
    static bool Unlink(const std::string & shmname){
        if(shmname.empty())
            return false;
        return (0 == ::shm_unlink(shmname.c_str()));
    }
    //构造/复制/析构
    //shmname: 共享文件名，应该以'/'开头，例如"/shmname"，实际位置可能是"/dev/shm/shmname"
    //flags: 参考shm_open(2)的flags参数
    //mode: 参考shm_open(2)的mode参数
    CPosixShmFile(){}
    //TODO:unit test
    explicit CPosixShmFile(const char * shmname,
            int flags = __MyBase::kFlagsDefault,
            mode_t mode = __MyBase::kModeDefault){
        this->open(shmname, flags, mode);
    }
    explicit CPosixShmFile(const std::string & shmname,
            int flags = __MyBase::kFlagsDefault,
            mode_t mode = __MyBase::kModeDefault){
        this->open(shmname, flags, mode);
    }
    CPosixShmFile(const CPosixShmFile & other){__MyBase::copyFrom(other);}
    CPosixShmFile & operator =(const CPosixShmFile & other){
        __MyBase::copyFrom(other);
        return *this;
    }
    //返回fd类型和名称
    int fdType() const{return kFdType;}
    const char * fdTypeName() const{return "CPosixShmFile";}
    //获取共享文件名
    std::string shmname() const{return shmname_;}
    //打开文件
    //shmname: 共享文件名，应该以'/'开头，例如"/shmname"，实际位置可能是"/dev/shm/shmname"
    //flags: 参考shm_open(2)的flags参数
    //mode: 参考shm_open(2)的mode参数
    //return:
    //  true    成功
    //  false   失败
    //TODO:unit test
    bool open(const char * shmname, int flags = kFlagsDefault, mode_t mode = kModeDefault){
        if(NULL == shmname)
            return false;
        if(valid())
            this->close();
        fd_ = ::shm_open(shmname, flags, mode);
        shmname_ = shmname;
        return valid();
    }
    bool open(const std::string & shmname, int flags = kFlagsDefault, mode_t mode = kModeDefault){
        return this->open(shmname.c_str(), flags, mode);
    }
    //删除文件
    bool unlink(){
        if(!valid())
            return false;
        return Unlink(shmname());
    }
    //内部状态描述，主要用于log
    std::string toString() const{
        CToString oss;
        oss<<"{CFile="<<CFile::toString()
            <<", shmname_="<<shmname_
            <<"}";
        return oss.str();
    }
private:
    std::string shmname_;
};

NS_SERVER_END

#endif

