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
 * @brief A convenient interface for epoll(7).
 * @author Zhao DAI
 */

#ifndef DOZERG_EPOLL_H_20130506
#define DOZERG_EPOLL_H_20130506

#include <sys/epoll.h>
#include <vector>
#include "file.hh"
#include "tools/debug.hh"   //tools::ToStringBits
#include "template.hh"      //ARRAY_SIZE

NS_SERVER_BEGIN

class CEpoll;

/**
 * @brief Representation of @c epoll_event.
 */
struct CEpollEvent : private epoll_event
{
    friend class CEpoll;
    /**
     * @brief Get events description.
     * @param ev A bit mask representing events
     * @return Events description
     */
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
    /**
     * @brief Get fd (file descriptor).
     * @return fd of self
     */
    int fd() const{return data.fd;}
    /**
     * @brief Test if @ref fd is valid.
     * @return @c true if @ref fd is valid; @c false otherwise
     */
    bool valid() const{return data.fd >= 0;}
    /**
     * @brief Test if there are read events.
     * @return @c true if there are read events; @c false otherwise
     */
    bool canInput() const{return events & EPOLLIN;}
    /**
     * @brief Test if there are write events.
     * @return @c true if there are write events; @c false otherwise
     */
    bool canOutput() const{return events & EPOLLOUT;}
    /**
     * @brief Test if there are errors.
     * @return @c true if there are errors; @c false otherwise
     */
    bool error() const{return (events & EPOLLERR) || (events & EPOLLHUP);}
    /**
     * @brief Get readable description.
     * @return Readable description
     */
    std::string toString() const{
        CToString oss;
        oss<<"{fd="<<data.fd
            <<", events="<<EventsName(events)
            <<"}";
        return oss.str();
    }
};

/**
 * @brief Representation of epoll(7).
 */
class CEpoll : public IFileDesc
{
public:
    static const int kFdType = 6;
    /**
     * @brief Get fd (file descriptor) type identifier.
     * @return @c 6
     */
    int fdType() const{return kFdType;}
    /**
     * @brief Get fd (file identifier) type name.
     * @return @c "CEpoll"
     */
    const char * fdTypeName() const{return "CEpoll";}
    /**
     * @brief Initialize epoll.
     * @return @c true if succeeded; @c false otherwise
     */
    bool create(){
        if(!valid())
            fd_ = ::epoll_create(1000);
        return valid();
    }
    /**
     * @brief Add fd (file descriptor) to epoll.
     * @param fd File descriptor, a non-negative number
     * @param flags A bit mask of events to monitor, e.g. @c EPOLLIN, @c EPOLLOUT
     * @param mod
     *   @li @c true: If @c fd is already in epoll, modify its @c flags
     *   @li @c false: If @c fd is already in epoll, this operation will fail
     * @return @c true if succeeded; @c false otherwise
     */
    bool addFd(int fd, uint32_t flags, bool mod = true){
        return (valid()
                && (ctrl(fd, flags, EPOLL_CTL_ADD)
                    || (mod && ctrl(fd, flags, EPOLL_CTL_MOD))));
    }
    /**
     * @brief Modify flags of an fd (file descriptor).
     * @param fd File descriptor, a non-negative number
     * @param flags A bit mask of events to monitor, e.g. @c EPOLLIN, @c EPOLLOUT
     * @param add
     *   @li @c true: If @c fd is not in epoll, add it
     *   @li @c false: If @c fd is not in epoll, this operation will fail
     * @return
     */
    bool modFd(int fd, uint32_t flags, bool add = true){
        return (valid()
                && (ctrl(fd, flags, EPOLL_CTL_MOD)
                    || (add && ctrl(fd, flags, EPOLL_CTL_ADD))));
    }
    /**
     * @brief Remove an fd (file descriptor) from epoll.
     * @param fd File descriptor, a non-negative number
     * @return @c true if succeeded; @c false otherwise
     */
    bool delFd(int fd){return (valid() && ctrl(fd, 0, EPOLL_CTL_DEL));}
    /**
     * @brief Wait for epoll events.
     * This function checks if there are pending events, and blocks current thread for an amount of
     * time if necessary.
     * @param timeoutMs
     *   @li @c -1: Wait forever if there is no events
     *   @li @c 0: Return immediately no matter there are events or not
     *   @li Positive Number: Milliseconds to wait if there is no events
     * @return @c true if there are events; @c false if time is out and there is no event
     */
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
    /**
     * @brief Get number of file descriptors that have pending events.
     * @return Number of ready file descriptors
     */
    size_t size() const{return revents_.size();}
    /**
     * @brief Get events of a ready file descriptor.
     * @param i Index of ready file descriptors list
     * @return Readonly events object for the file descriptor
     */
    const CEpollEvent & operator [](size_t i) const{return revents_[i];}
    /**
     * @brief Get readable description.
     * @return Readable description of this object
     */
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

