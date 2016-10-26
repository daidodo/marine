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
    //ɾ��ָ�����ļ�
    //shmname: �����ļ�����Ӧ����'/'��ͷ������"/shmname"��ʵ��λ�ÿ�����"/dev/shm/shmname"
    static bool Unlink(const std::string & shmname){
        if(shmname.empty())
            return false;
        return (0 == ::shm_unlink(shmname.c_str()));
    }
    //����/����/����
    //shmname: �����ļ�����Ӧ����'/'��ͷ������"/shmname"��ʵ��λ�ÿ�����"/dev/shm/shmname"
    //flags: �ο�shm_open(2)��flags����
    //mode: �ο�shm_open(2)��mode����
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
    //����fd���ͺ�����
    int fdType() const{return kFdType;}
    const char * fdTypeName() const{return "CPosixShmFile";}
    //��ȡ�����ļ���
    std::string shmname() const{return shmname_;}
    //���ļ�
    //shmname: �����ļ�����Ӧ����'/'��ͷ������"/shmname"��ʵ��λ�ÿ�����"/dev/shm/shmname"
    //flags: �ο�shm_open(2)��flags����
    //mode: �ο�shm_open(2)��mode����
    //return:
    //  true    �ɹ�
    //  false   ʧ��
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
    //ɾ���ļ�
    bool unlink(){
        if(!valid())
            return false;
        return Unlink(shmname());
    }
    //�ڲ�״̬��������Ҫ����log
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

