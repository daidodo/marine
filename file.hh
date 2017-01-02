/*
 * Copyright (c) 2016 Zhao DAI <daidodo@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or any
 * later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see accompanying file LICENSE.txt
 * or <http://www.gnu.org/licenses/>.
 */

/**
 * @file
 * @brief Encapsulation for fd (file descriptor) and regular files.
 * @author Zhao DAI
 */

#ifndef DOZERG_FILE_H_20120119
#define DOZERG_FILE_H_20120119

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

/**
 * @brief Abstract interface of fd (file descriptor).
 */
class IFileDesc
{
public:
    static const int kInvalidFd = -1;
    /**
     * @brief Get latest @c errno and error message.
     * @return Description of latest error
     */
    static std::string ErrMsg(){return tools::ErrorMsg(errno);}
    /**
     * @brief Construct a default object.
     */
    IFileDesc():fd_(kInvalidFd){}
    /**
     * @brief Destroy this object.
     * Release any resources if necessary.
     */
    virtual ~IFileDesc(){this->close();}
    /**
     * @brief Test if this object is a valid file descriptor.
     * @return @c true if it's valid; @c false otherwise
     */
    bool valid() const{return (fd_ >= 0);}
    /**
     * @brief Get fd (file descriptor).
     * @return fd
     */
    int fd() const{return fd_;}
    /**
     * @brief Get type id of this object.
     * Any concrete class derived from IFileDesc shall define a distinct type id named @c kFdType,
     * so it's possible to check this id and determine what the object really is.
     * @n Currently following `kFdType`s are defined:
     * | Type Id | Value |
     * | ---: | --- |
     * | @ref @c CFile::kFdType | 1 |
     * | @ref @c CTcpConnSocket::kFdType | 2 |
     * | @ref @c CListenSocket::kFdType | 3 |
     * | @ref @c CUdpSocket::kFdType | 4 |
     * | @ref @c CPosixShmFile::kFdType | 5 |
     * | @ref @c CEpoll::kFdType | 6 |
     * @return Type id of this object
     * @sa kFdType
     */
    virtual int fdType() const = 0;
    /**
     * @brief Get type name of this object.
     * Any concrete class derived from IFileDesc must give a readable name to identify itself.
     * @return Readable type name
     */
    virtual const char * fdTypeName() const = 0;
    /**
     * @brief Get file name opened by this object.
     * @return A file name, or an empty string if any error occurs
     * @sa ErrMsg
     */
    std::string filename() const{return tools::GetFilenameByFd(fd_);}
    /**
     * @brief Get byte size of file opened by this object.
     * @return
     *   @li @c -1: Failed
     *   @li others: Byte size of file
     * @sa ErrMsg
     */
    off_t length() const{
        if(!valid())
            return -1;
        struct stat st;
        if(0 != ::fstat(fd(), &st))
            return -1;
        return st.st_size;
    }
    /**
     * @brief Test if file is deleted.
     * A file could be removed even if you're reading or writing it. So it's necessary to test this
     * status, or you may lost all changes to the file.
     * @return @c true if file is deleted; @c false otherwise
     */
    bool deleted() const{
        if(!valid())
            return false;
        struct stat st;
        if(0 != ::fstat(fd(), &st))
            return false;
        return (0 == st.st_nlink);
    }
    /**
     * @brief Set block/non-block for operations.
     * @param on
     *   @li @c true: all IO operations will be @em BLOCK by default
     *   @li @c false: all IO operations will be @em NONBLOCK by default
     * @return @c true if succeeded; @c false otherwise
     * @sa ErrMsg
     */
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
    /**
     * @brief Close this fd.
     */
    void close(){
        if(valid()){
            ::close(fd_);
            fd_ = kInvalidFd;
        }
    }
    /**
     * @brief Get readable description of this object.
     * @return Description string
     */
    virtual std::string toString() const{
        CToString oss;
        oss<<"{fd_="<<fd_<<"("<<fdTypeName()<<")"
            <<"}";
        return oss.str();
    }
private:
    //NOTE: Subclasses could implement its own copy and assignment operations
    IFileDesc(const IFileDesc & other);
    IFileDesc & operator =(const IFileDesc & other);
protected:
    //member
    int fd_;
};

/**
 * @brief Regular file operations.
 */
class CFile : public IFileDesc
{
    typedef IFileDesc __MyBase;
protected:
    static const int kFlagsDefault = O_RDONLY;
    static const int kModeDefault = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
public:
    static const int kFdType = 1; /**< Type id */
    /**
     * @brief Remove a file.
     * @param pathname File pathname
     * @return @c true if succeeded; @c false otherwise
     * @sa ErrMsg
     */
    static bool Unlink(const std::string & pathname){
        if(pathname.empty())
            return false;
        return (0 == ::unlink(pathname.c_str()));
    }
    /**
     * @brief Rename a file.
     * @param oldfile Old pathname
     * @param newfile New pathname
     * @return @c true if succeeded; @c false otherwise
     * @sa ErrMsg
     */
    static bool Rename(const std::string & oldfile, const std::string & newfile){
        if(oldfile.empty() || newfile.empty())
            return false;
        return (0 == ::rename(oldfile.c_str(), newfile.c_str()));
    }
    /**
     * @brief Default constructor.
     */
    CFile(){}
    /**
     * @name Open or create a file
     * @details @c mode is useful only when a new file is created.
     * @param pathname File pathname
     * @param flags Flags for open(2), e.g. @c O_RDONLY, @c O_WRONLY, or @c O_RDWR
     * @param mode Mode for open(2) when new file is created, e.g. @c S_IRUSR
     * @sa ErrMsg
     * @{ */
    explicit CFile(const char * pathname, int flags = kFlagsDefault, mode_t mode = kModeDefault){
        this->open(pathname, flags, mode);
    }
    explicit CFile(const std::string & pathname, int flags = kFlagsDefault, mode_t mode = kModeDefault){
        this->open(pathname, flags, mode);
    }
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
    /**  @} */
    /**
     * @name Duplicate file descriptor
     * @details These functions do @em NOT copy file content, it only duplicates fd using dup(2).
     * @param other Another file object
     * @{ */
    CFile(const CFile & other){copyFrom(other);}
    CFile & operator =(const CFile & other){
        copyFrom(other);
        return *this;
    }
    /**  @} */
    int fdType() const{return kFdType;}
    /**
     * @brief Get readable description of this object.
     * @return @c "CFile"
     */
    const char * fdTypeName() const{return "CFile";}
    /**
     * @name Read from file
     * @details These functions read at most @c size bytes from file.
     * @param[out] buf A buffer to receive the data
     * @param[in] size Size of bytes to read at most
     * @param[in] append
     *   @li @c true: New data will append to @c buf
     *   @li @c false: New data will overwrite old ones in @c buf
     * @{ */
    /**
     * @return
     *   @li @c -1: Failed
     *   @li @c 0: The end of file is reached
     *   @li Positive number: Size of bytes actually read
     */
    ssize_t read(char * buf, size_t size){
        if(!valid())
            return -1;
        return ::read(fd(), buf, size);
    }
    /**
     * @return @c true if succeeded; @c false otherwise
     */
    bool read(std::vector<char> & buf, size_t size, bool append){
        return readData(buf, size, append);
    }
    /**
     * @return @c true if succeeded; @c false otherwise
     */
    bool read(std::string & buf, size_t size, bool append){
        return readData(buf, size, append);
    }
    /**  @} */
    /**
     * @name Write to file
     * @param buf A buffer to write
     * @param size Bytes size of @c buf
     * @return
     *   @li @c -1: Failed
     *   @li Non-negative number: Size of bytes actually written
     * @{ */
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
    /**  @} */
    /**
     * @name Set cursor.
     * @details See lseek(2) for more information.
     * @param offset Offset to move the cursor
     * @param whence Directive for @c offset, showing as follows:
     * | Directive | Explanation |
     * | --- | --- |
     * | SEEK_SET | The offset is set to @c offset bytes |
     * | SEEK_CUR | The offset is set to its current location plus @c offset bytes |
     * | SEEK_END | The offset is set to the size of the file plus @c offset bytes |
     * @return @c true if succeeded; @c false otherwise
     * @{ */
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
    /**  @} */
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

