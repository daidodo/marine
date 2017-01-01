#ifndef DOZERG_FILE_H_20120119
#define DOZERG_FILE_H_20120119

/*
    对fd和文件读写进行封装
        IFileDesc       fd接口封装，所有fd形式对象的基类
        CFile           文件对象封装
//*/

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <cstdio>
#include <string>
#include <vector>
#include "tools/system.hh"  //ErrorMsg

NS_SERVER_BEGIN

class IFileDesc
{
public:
    static const int kInvalidFd = -1;
    //获取上次出错信息
    static std::string ErrMsg(){return tools::ErrorMsg(errno);}
    //构造/析构
    IFileDesc():fd_(kInvalidFd){}
    virtual ~IFileDesc(){this->close();}
    //是否有效
    bool valid() const{return (fd_ >= 0);}
    //获取fd
    int fd() const{return fd_;}
    //返回fd类型和名称，由子类实现
    //return:
    //  CFile::kFdType(1)
    //  CTcpConnSocket::kFdType(2)
    //  CListenSocket::kFdType(3)
    //  CUdpSocket::kFdType(4)
    //  CPosixShmFile::kFdType(5)
    //  CEpoll::kFdType(6)
    virtual int fdType() const = 0;
    virtual const char * fdTypeName() const = 0;
    //返回打开的文件名
    std::string filename() const{return tools::GetFilenameByFd(fd_);}
    //获取文件字节大小
    //return:
    //  -1      出错
    //  其他    文件字节大小
    off_t length() const{
        if(!valid())
            return -1;
        struct stat st;
        if(0 != ::fstat(fd(), &st))
            return -1;
        return st.st_size;
    }
    //文件是否已经被删除
    //return:
    //  false   未被删除，或者出错
    //  true    已被删除
    bool deleted() const{
        if(!valid())
            return false;
        struct stat st;
        if(0 != ::fstat(fd(), &st))
            return false;
        return (0 == st.st_nlink);
    }
    //设置是否阻塞
    //return:
    //  true    设置成功
    //  false   设置失败
    bool block(bool on){
        if(!valid())
            return false;
        const int oldflag = ::fcntl(fd_, F_GETFL);
        if(-1 == oldflag)
            return false;
        const int newflag = (on ? oldflag & ~O_NONBLOCK : oldflag | O_NONBLOCK);
        if(oldflag == newflag)
            return true;
        if(::fcntl(fd_, F_SETFL, newflag) < 0)
            return false;
        return true;
    }
    //关闭fd
    void close(){
        if(valid()){
            ::close(fd_);
            fd_ = kInvalidFd;
        }
    }
    //内部状态描述，主要用于log
    virtual std::string toString() const{
        CToString oss;
        oss<<"{fd_="<<fd_<<"("<<fdTypeName()<<")"
            <<"}";
        return oss.str();
    }
private:
    //NOTE: 子类可以自定义复制行为
    IFileDesc(const IFileDesc & other);
    IFileDesc & operator =(const IFileDesc & other);
protected:
    //member
    int fd_;
};

class CFile : public IFileDesc
{
    typedef IFileDesc __MyBase;
protected:
    static const int kFlagsDefault = O_RDONLY;
    static const int kModeDefault = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
public:
    static const int kFdType = 1;
    //删除指定的文件
    //pathname: 文件路径
    static bool Unlink(const std::string & pathname){
        if(pathname.empty())
            return false;
        return (0 == ::unlink(pathname.c_str()));
    }
    //重命名指定的文件
    //oldfile: 旧的文件名
    //newfile: 新的文件名
    static bool Rename(const std::string & oldfile, const std::string & newfile){
        if(oldfile.empty() || newfile.empty())
            return false;
        return (0 == ::rename(oldfile.c_str(), newfile.c_str()));
    }
    //构造/复制/析构
    //pathname: 文件名
    //flags: 参考open(2)的flags参数
    //mode: 参考open(2)的mode参数
    CFile(){}
    //TODO:unit test
    explicit CFile(const char * pathname, int flags = kFlagsDefault, mode_t mode = kModeDefault){
        this->open(pathname, flags, mode);
    }
    explicit CFile(const std::string & pathname, int flags = kFlagsDefault, mode_t mode = kModeDefault){
        this->open(pathname, flags, mode);
    }
    CFile(const CFile & other){copyFrom(other);}
    CFile & operator =(const CFile & other){
        copyFrom(other);
        return *this;
    }
    //返回fd类型和名称
    int fdType() const{return kFdType;}
    const char * fdTypeName() const{return "CFile";}
    //打开文件
    //pathname: 文件名
    //flags: 参考open(2)的flags参数
    //mode: 参考open(2)的mode参数
    //return:
    //  true    成功
    //  false   失败
    //TODO: unit test
    virtual bool open(const char * pathname, int flags = kFlagsDefault, mode_t mode = kModeDefault){
        if(NULL == pathname)
            return false;
        if(valid())
            this->close();
        fd_ = ::open(pathname, flags, mode);
        return valid();
    }
    virtual bool open(const std::string & pathname, int flags = kFlagsDefault, mode_t mode = kModeDefault){
        return this->open(pathname.c_str(), flags, mode);
    }
    //从文件最多读取size字节数据放到buf里
    //return:
    //  -1  出错
    //  +n  实际读出的字节数
    ssize_t read(char * buf, size_t size){
        if(!valid())
            return -1;
        return ::read(fd(), buf, size);
    }
    //从文件最多读取size字节数据放到buf里
    //append: true-追加到已有数据末尾; false-覆盖已有数据
    //return:
    //  true    成功
    //  false   失败
    bool read(std::vector<char> & buf, size_t size, bool append){
        return readData(buf, size, append);
    }
    bool read(std::string & buf, size_t size, bool append){
        return readData(buf, size, append);
    }
    //将buf数据写入文件
    //return:
    //  -1  出错
    //  +n  实际写入的字节数
    ssize_t write(const char * buf, size_t size){
        if(!valid())
            return -1;
        return ::write(fd(), buf, size);
    }
    ssize_t write(const std::vector<char> & buf){
        return this->write(&buf[0], buf.size());
    }
    ssize_t write(const std::string & buf){
        return this->write(&buf[0], buf.size());
    }
    //设置读写位置
    //whence:
    //  SEEK_SET    相对于文件起点偏移offset
    //  SEEK_CUR    相对于当前位置偏移offset
    //  SEEK_END    相对于文件末尾偏移offset
    //return:
    //  true    成功
    //  false   失败
    bool seek(off_t offset, int whence){
        if(!valid())
            return false;
        return ((off_t)-1 != ::lseek(fd(), offset, whence));
    }
#ifdef __HAS_LSEEK64
    bool seek64(off64_t offset, int whence){
        if(!valid())
            return false;
        return ((off_t)-1 != ::lseek64(fd(), offset, whence));
    }
#endif
    //获取读写位置（相对于文件起点）
    //return:
    //  -1  错误
    //  >=0 读写位置
    off_t tell() const{
        if(!valid())
            return -1;
        return ::lseek(fd(), 0, SEEK_CUR);
    }
#ifdef __HAS_LSEEK64
    off64_t tell64() const{
        if(!valid())
            return -1;
        return ::lseek64(fd(), 0, SEEK_CUR);
    }
#endif
#ifdef __HAS_FTRUNCATE
    //将文件截断或扩展到指定字节长度
    //length: 最后的字节长度
    //注意：文件必须可写
    bool truncate(off_t length){
        if(!valid())
            return false;
        return (0 == ::ftruncate(fd(), length));
    }
#endif
    //删除文件
    virtual bool unlink(){
        if(!valid())
            return false;
        return Unlink(__MyBase::filename());
    }
    //重命名文件
    bool rename(const std::string & newfile){
        if(!valid() || newfile.empty())
            return false;
        std::string fname = __MyBase::filename();
        if(fname.empty())
            return false;
        return Rename(fname, newfile);
    }
    //内部状态描述，主要用于log
    std::string toString() const{
        CToString oss;
        oss<<"{IFileDesc="<<IFileDesc::toString()
            <<", filename="<<filename()
            <<"}";
        return oss.str();
    }
protected:
    void copyFrom(const CFile & other){
        if(!other.valid()){
            this->close();
        }else if(!valid()){
            fd_ = ::dup(other.fd());
        }else
            fd_ = ::dup2(other.fd(), fd_);
    }
private:
    template<class Buf>
    bool readData(Buf & buf, size_t size, bool append){
        if(!valid())
            return false;
        const size_t from = (append ? buf.size() : 0);
        buf.resize(from + size);
        const ssize_t ret = this->read(&buf[from], size);
        buf.resize(from + (ret > 0 ? ret : 0));
        return (ret >= 0);
    }
};

NS_SERVER_END

#endif

