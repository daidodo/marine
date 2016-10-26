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

//[1, 999]�Ǳ���������ID
enum EReaperAttr
{
    ATTR_NONE,
    ATTR_ACCEPT_CLIENT,         //Reaper: Accept�¿ͻ���
    ATTR_CLOSE_FD,              //Reaper: �ر�socket
    ATTR_EVENT_INPUT,           //Reaper: ֪ͨ���¼�
    ATTR_EVENT_OUTPUT,          //Reaper: ֪ͨд�¼�
    ATTR_TCP_RECV,              //Reaper: Tcp Recv�ɹ�
    ATTR_TCP_SEND,              //Reaper: Tcp Send�ɹ�
    ATTR_UDP_RECV,              //Reaper: Udp Recv�ɹ�
    ATTR_UDP_SEND,              //Reaper: Udp Send�ɹ�
    ATTR_PUSH_EVENT_QUE,        //Reaper: Push EventQue�ɹ�
    ATTR_POP_EVENT_QUE,         //Reaper: Pop EventQue�ɹ�
    ATTR_PUSH_EVENT_QUE_ERR,    //Reaper: Push EventQueʧ��,warning
    ATTR_PUSH_ADDING_QUE,       //Reaper: Push AddingQue�ɹ�
    ATTR_POP_ADDING_QUE,        //Reaper: Pop AddingQue�ɹ�
    ATTR_PUSH_ADDING_QUE_ERR,   //Reaper: Push AddingQueʧ��,warning
    ATTR_PUSH_PKG_QUE,          //Reaper: Push PkgQue�ɹ�
    ATTR_POP_PKG_QUE,           //Reaper: Pop PkgQue�ɹ�
    ATTR_PUSH_PKG_QUE_ERR,      //Reaper: Push PkgQueʧ��,warning
    ATTR_EPOLL_ADD,             //Reaper: ��epoll�����ӻ��޸�fd
    ATTR_EPOLL_ADD_ERR,         //Reaper: ��epoll�����ӻ��޸�fdʧ��,warning
    ATTR_EPOLL_WAIT_ERR,        //Reaper: epoll wait����,warning
    ATTR_UNKNOWN_FD_TYPE,       //Reaper: δ֪��Fd Type,warning

    ATTR_MAX
};

NS_HSHA_END

#endif

