#include <marine/epoll.hh>

#include "../inc.h"

TEST(CEpollEvent, all)
{
    ASSERT_EQ("0", CEpollEvent::EventsName(0));
    ASSERT_EQ("4294967295(EPOLLIN | EPOLLPRI | EPOLLOUT | EPOLLERR | EPOLLHUP | EPOLLRDNORM | EPOLLRDBAND | EPOLLWRNORM | EPOLLWRBAND | EPOLLMSG | 0xFFFFF820)", CEpollEvent::EventsName(-1));
    ASSERT_EQ("4294965280(0xFFFFF820)", CEpollEvent::EventsName(0xFFFFF820));

    ASSERT_EQ(sizeof(struct epoll_event), sizeof(CEpollEvent));
    struct epoll_event ev;
    CEpollEvent ev2;
    memcpy(&ev2, &ev, sizeof ev);

    ASSERT_EQ(ev.data.fd, ev2.fd());
    ASSERT_EQ(ev.data.fd >= 0, ev2.valid());
    ASSERT_EQ(0 != (ev.events & EPOLLIN), ev2.canInput());
    ASSERT_EQ(0 != (ev.events & EPOLLOUT), ev2.canOutput());
    ASSERT_EQ(0 != (ev.events & EPOLLERR) || 0 != (ev.events & EPOLLHUP), ev2.error());
    std::ostringstream oss;
    oss<<"{fd="<<ev.data.fd<<", events="<<CEpollEvent::EventsName(ev.events)<<"}";
    ASSERT_EQ(oss.str(), ev2.toString());
}

TEST(CEpoll, fdType)
{
    CEpoll e;

    ASSERT_EQ(6, e.fdType());
    ASSERT_STREQ("CEpoll", e.fdTypeName());
}

TEST(CEpoll, create)
{
    CEpoll e;
    ASSERT_FALSE(e.valid());

    ASSERT_TRUE(e.create());
    ASSERT_TRUE(e.valid());
}

TEST(CEpoll, addFd)
{
    {
        CEpoll e;
        ASSERT_TRUE(e.create());
        ASSERT_TRUE(e.valid());

        ASSERT_TRUE(e.addFd(0, EPOLLIN, false));
        ASSERT_FALSE(e.addFd(0, EPOLLOUT, false));

        ASSERT_TRUE(e.addFd(1, EPOLLOUT, true));
        ASSERT_TRUE(e.addFd(1, EPOLLIN | EPOLLOUT, true));

        ASSERT_TRUE(e.addFd(2, EPOLLIN));
        ASSERT_TRUE(e.addFd(2, EPOLLIN | EPOLLOUT));
    }{
        CEpoll e;
        ASSERT_FALSE(e.valid());

        ASSERT_FALSE(e.addFd(0, EPOLLIN, false));
        ASSERT_FALSE(e.addFd(0, EPOLLOUT, false));

        ASSERT_FALSE(e.addFd(1, EPOLLOUT, true));
        ASSERT_FALSE(e.addFd(1, EPOLLIN | EPOLLOUT, true));

        ASSERT_FALSE(e.addFd(2, EPOLLIN));
        ASSERT_FALSE(e.addFd(2, EPOLLIN | EPOLLOUT));
    }{
        CEpoll e;
        ASSERT_TRUE(e.create());
        ASSERT_TRUE(e.valid());

        ASSERT_FALSE(e.addFd(-1, EPOLLIN, false));
        ASSERT_TRUE(e.addFd(0, 0, false));
    }
}

TEST(CEpoll, modFd)
{
    {
        CEpoll e;
        ASSERT_TRUE(e.create());
        ASSERT_TRUE(e.valid());

        ASSERT_FALSE(e.modFd(0, EPOLLIN, false));
        ASSERT_TRUE(e.modFd(0, EPOLLIN, true));
        ASSERT_TRUE(e.modFd(0, EPOLLOUT, false));
        ASSERT_TRUE(e.modFd(0, EPOLLIN | EPOLLOUT, true));

        ASSERT_FALSE(e.modFd(1, EPOLLIN, false));
        ASSERT_TRUE(e.modFd(1, EPOLLIN));
        ASSERT_TRUE(e.modFd(1, EPOLLOUT, false));
        ASSERT_TRUE(e.modFd(1, EPOLLIN | EPOLLOUT));
    }{
        CEpoll e;
        ASSERT_FALSE(e.valid());

        ASSERT_FALSE(e.modFd(0, EPOLLIN, false));
        ASSERT_FALSE(e.modFd(0, EPOLLIN, true));
        ASSERT_FALSE(e.modFd(0, EPOLLOUT, false));
        ASSERT_FALSE(e.modFd(0, EPOLLIN | EPOLLOUT, true));

        ASSERT_FALSE(e.modFd(1, EPOLLIN, false));
        ASSERT_FALSE(e.modFd(1, EPOLLIN));
        ASSERT_FALSE(e.modFd(1, EPOLLOUT, false));
        ASSERT_FALSE(e.modFd(1, EPOLLIN | EPOLLOUT));
    }{
        CEpoll e;
        ASSERT_TRUE(e.create());
        ASSERT_TRUE(e.valid());

        ASSERT_FALSE(e.modFd(-1, EPOLLIN, false));
        ASSERT_TRUE(e.addFd(0, EPOLLIN, false));
        ASSERT_TRUE(e.modFd(0, 0, false));
    }
}

TEST(CEpoll, delFd)
{
    {
        CEpoll e;
        ASSERT_TRUE(e.create());
        ASSERT_TRUE(e.valid());

        ASSERT_FALSE(e.delFd(0));

        ASSERT_TRUE(e.addFd(0, EPOLLIN));
        ASSERT_TRUE(e.delFd(0));
    }{
        CEpoll e;
        ASSERT_FALSE(e.valid());

        ASSERT_FALSE(e.delFd(0));

        ASSERT_FALSE(e.addFd(0, EPOLLIN));
        ASSERT_FALSE(e.delFd(0));
    }
}

TEST(CEpoll, wait)
{
    {
        CEpoll e;
        ASSERT_TRUE(e.create());
        ASSERT_TRUE(e.valid());

        ASSERT_TRUE(e.addFd(STDIN_FILENO, EPOLLIN));
        ASSERT_TRUE(e.addFd(STDOUT_FILENO, EPOLLOUT));
        ASSERT_TRUE(e.addFd(STDERR_FILENO, EPOLLIN | EPOLLOUT));

        ASSERT_TRUE(e.wait(1000));
        ASSERT_EQ(size_t(2), e.size()); //stdout, stderr

        for(size_t i = 0;i < e.size();++i){
            const CEpollEvent & ev = e[i];
            ASSERT_TRUE(ev.valid());
            switch(ev.fd()){
                case STDOUT_FILENO:
                    ASSERT_FALSE(ev.canInput());
                    ASSERT_TRUE(ev.canOutput());
                    ASSERT_FALSE(ev.error());
                    ASSERT_EQ("{fd=1, events=4(EPOLLOUT)}", ev.toString());
                    break;
                case STDERR_FILENO:
                    ASSERT_FALSE(ev.canInput());
                    ASSERT_TRUE(ev.canOutput());
                    ASSERT_FALSE(ev.error());
                    ASSERT_EQ("{fd=2, events=4(EPOLLOUT)}", ev.toString());
                    break;
                default:
                    FAIL()<<"ev="<<ev.toString();
            }
        }
    }{
        CEpoll e;
        ASSERT_FALSE(e.valid());

        ASSERT_FALSE(e.addFd(STDIN_FILENO, EPOLLIN));
        ASSERT_FALSE(e.addFd(STDOUT_FILENO, EPOLLOUT));
        ASSERT_FALSE(e.addFd(STDERR_FILENO, EPOLLIN | EPOLLOUT));

        ASSERT_FALSE(e.wait());
        ASSERT_EQ(size_t(0), e.size());
    }
}
