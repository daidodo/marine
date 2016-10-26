#ifndef DOZERG_TYPES_H_20130606
#define DOZERG_TYPES_H_20130606

#include <sockets.hh>
#include "impl/base.hh"

NS_HSHA_BEGIN

typedef marine::CSockAddr       __SockAddr;
typedef marine::CSocket         __Socket;
typedef marine::CListenSocket   __ListenSocket;
typedef marine::CTcpConnSocket  __TcpConnSocket;
typedef marine::CUdpSocket      __UdpSocket;

//[1, 999]是保留的属性ID
enum EReaperAttr
{
    ATTR_NONE,
    ATTR_ACCEPT_CLIENT,         //Reaper: Accept新客户端
    ATTR_CLOSE_FD,              //Reaper: 关闭socket
    ATTR_EVENT_INPUT,           //Reaper: 通知读事件
    ATTR_EVENT_OUTPUT,          //Reaper: 通知写事件
    ATTR_TCP_RECV,              //Reaper: Tcp Recv成功
    ATTR_TCP_SEND,              //Reaper: Tcp Send成功
    ATTR_UDP_RECV,              //Reaper: Udp Recv成功
    ATTR_UDP_SEND,              //Reaper: Udp Send成功
    ATTR_PUSH_EVENT_QUE,        //Reaper: Push EventQue成功
    ATTR_POP_EVENT_QUE,         //Reaper: Pop EventQue成功
    ATTR_PUSH_EVENT_QUE_ERR,    //Reaper: Push EventQue失败,warning
    ATTR_PUSH_ADDING_QUE,       //Reaper: Push AddingQue成功
    ATTR_POP_ADDING_QUE,        //Reaper: Pop AddingQue成功
    ATTR_PUSH_ADDING_QUE_ERR,   //Reaper: Push AddingQue失败,warning
    ATTR_PUSH_PKG_QUE,          //Reaper: Push PkgQue成功
    ATTR_POP_PKG_QUE,           //Reaper: Pop PkgQue成功
    ATTR_PUSH_PKG_QUE_ERR,      //Reaper: Push PkgQue失败,warning
    ATTR_EPOLL_ADD,             //Reaper: 向epoll中增加或修改fd
    ATTR_EPOLL_ADD_ERR,         //Reaper: 向epoll中增加或修改fd失败,warning
    ATTR_EPOLL_WAIT_ERR,        //Reaper: epoll wait出错,warning
    ATTR_UNKNOWN_FD_TYPE,       //Reaper: 未知的Fd Type,warning

    ATTR_MAX
};

NS_HSHA_END

#endif

