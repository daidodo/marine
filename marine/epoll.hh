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
    //事件描述，主要用于log
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
    //获取fd
    int fd() const{return data.fd;}
    //fd是否有效
    bool valid() const{return data.fd >= 0;}
    //是否有读事件
    bool canInput() const{return events & EPOLLIN;}
    //是否有写事件
    bool canOutput() const{return events & EPOLLOUT;}
    //是否出错
    bool error() const{return (events & EPOLLERR) || (events & EPOLLHUP);}
    //内部状态描述，主要用于log
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
    //返回fd类型和名称
    int fdType() const{return kFdType;}
    const char * fdTypeName() const{return "CEpoll";}
    //创建epoll
    bool create(){
        if(!valid())
            fd_ = ::epoll_create(1000);
        return valid();
    }
    //增加fd和flags
    //flags: 详见man epoll_ctl，主要有
    //  EPOLLIN     等待读事件
    //  EPOLLOUT    等待写事件
    //mod:
    //  true    如果fd已在epoll里，则修改flags
    //  false   如果fd已在epoll里，则返回失败
    bool addFd(int fd, uint32_t flags, bool mod = true){
        return (valid()
                && (ctrl(fd, flags, EPOLL_CTL_ADD)
                    || (mod && ctrl(fd, flags, EPOLL_CTL_MOD))));
    }
    //修改fd的flags
    //flags: 详见man epoll_ctl，主要有
    //  EPOLLIN     等待读事件
    //  EPOLLOUT    等待写事件
    //add:
    //  true    如果fd没在epoll里，则增加fd和flags
    //  false   如果fd没在epoll里，则返回失败
    bool modFd(int fd, uint32_t flags, bool add = true){
        return (valid()
                && (ctrl(fd, flags, EPOLL_CTL_MOD)
                    || (add && ctrl(fd, flags, EPOLL_CTL_ADD))));
    }
    //删除fd
    bool delFd(int fd){return (valid() && ctrl(fd, 0, EPOLL_CTL_DEL));}
    //等待epoll事件
    //timeoutMs:
    //  0       不等待，直接返回
    //  负数    永久等待
    //  正数    等待timeoutMs毫秒
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
    //有读写事件的fd个数
    size_t size() const{return revents_.size();}
    //获取fd和事件
    const CEpollEvent & operator [](size_t i) const{return revents_[i];}
    //内部状态描述，主要用于log
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

