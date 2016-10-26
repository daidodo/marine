#ifndef DOZERG_EPOLL_H_20130506
#define DOZERG_EPOLL_H_20130506

#include <sys/epoll.h>
#include <vector>
#include "file.hh"
#include "tools/debug.hh"   //tools::ToStringBits
#include "template.hh"      //ARRAY_SIZE

NS_SERVER_BEGIN

class CEpoll;

struct CEpollEvent : private epoll_event
{
    friend class CEpoll;
    //�¼���������Ҫ����log
    static std::string EventsName(uint32_t ev){
        const char * const kName[] = {
            "EPOLLIN",
            "EPOLLPRI",
            "EPOLLOUT",
            "EPOLLERR",
            "EPOLLHUP",
            NULL,
            "EPOLLRDNORM",
            "EPOLLRDBAND",
            "EPOLLWRNORM",
            "EPOLLWRBAND",
            "EPOLLMSG"
        };
        return marine::tools::ToStringBits(ev, kName, ARRAY_SIZE(kName));
    }
    //��ȡfd
    int fd() const{return data.fd;}
    //fd�Ƿ���Ч
    bool valid() const{return data.fd >= 0;}
    //�Ƿ��ж��¼�
    bool canInput() const{return events & EPOLLIN;}
    //�Ƿ���д�¼�
    bool canOutput() const{return events & EPOLLOUT;}
    //�Ƿ����
    bool error() const{return (events & EPOLLERR) || (events & EPOLLHUP);}
    //�ڲ�״̬��������Ҫ����log
    std::string toString() const{
        CToString oss;
        oss<<"{fd="<<data.fd
            <<", events="<<EventsName(events)
            <<"}";
        return oss.str();
    }
};

class CEpoll : public IFileDesc
{
public:
    static const int kFdType = 6;
    //����fd���ͺ�����
    int fdType() const{return kFdType;}
    const char * fdTypeName() const{return "CEpoll";}
    //����epoll
    bool create(){
        if(!valid())
            fd_ = ::epoll_create(1000);
        return valid();
    }
    //����fd��flags
    //flags: ���man epoll_ctl����Ҫ��
    //  EPOLLIN     �ȴ����¼�
    //  EPOLLOUT    �ȴ�д�¼�
    //mod:
    //  true    ���fd����epoll����޸�flags
    //  false   ���fd����epoll��򷵻�ʧ��
    bool addFd(int fd, uint32_t flags, bool mod = true){
        return (valid()
                && (ctrl(fd, flags, EPOLL_CTL_ADD)
                    || (mod && ctrl(fd, flags, EPOLL_CTL_MOD))));
    }
    //�޸�fd��flags
    //flags: ���man epoll_ctl����Ҫ��
    //  EPOLLIN     �ȴ����¼�
    //  EPOLLOUT    �ȴ�д�¼�
    //add:
    //  true    ���fdû��epoll�������fd��flags
    //  false   ���fdû��epoll��򷵻�ʧ��
    bool modFd(int fd, uint32_t flags, bool add = true){
        return (valid()
                && (ctrl(fd, flags, EPOLL_CTL_MOD)
                    || (add && ctrl(fd, flags, EPOLL_CTL_ADD))));
    }
    //ɾ��fd
    bool delFd(int fd){return (valid() && ctrl(fd, 0, EPOLL_CTL_DEL));}
    //�ȴ�epoll�¼�
    //timeoutMs:
    //  0       ���ȴ���ֱ�ӷ���
    //  ����    ���õȴ�
    //  ����    �ȴ�timeoutMs����
    bool wait(int timeoutMs = -1){
        if(!valid())
            return false;
        revents_.resize(128);
        int n = ::epoll_wait(fd(), &revents_[0], revents_.size(), timeoutMs);
        if(n < 0)
            return false;
        assert(size_t(n) <= revents_.size());
        revents_.resize(n);
        return true;
    }
    //�ж�д�¼���fd����
    size_t size() const{return revents_.size();}
    //��ȡfd���¼�
    const CEpollEvent & operator [](size_t i) const{return revents_[i];}
    //�ڲ�״̬��������Ҫ����log
    std::string toString() const{
        const size_t kMaxPrint = 4;
        CToString oss;
        oss<<"{IFileDesc="<<IFileDesc::toString()
            <<", revents_={";
        for(size_t i = 0;i < revents_.size() && i < kMaxPrint;++i){
            if(i)
                oss<<", ";
            oss<<'['<<i<<"]="<<revents_[i].toString();
        }
        if(revents_.size() > kMaxPrint)
            oss<<", ...";
        oss<<"}}";
        return oss.str();
    }
private:
    bool ctrl(int fd, uint32_t flags, int op){
        assert(valid());
        struct epoll_event ev;
        memset(&ev, 0, sizeof ev);
        ev.events = flags | EPOLLET;
        ev.data.fd = fd;
        return (0 == ::epoll_ctl(this->fd(), op, fd, &ev));
    }
    //field
    std::vector<CEpollEvent> revents_;
};

NS_SERVER_END

#endif

