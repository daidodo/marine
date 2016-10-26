#ifndef DOZERG_SERVER_CALLBACK_H_20130509
#define DOZERG_SERVER_CALLBACK_H_20130509

#include <deque>
#include <utility>      //std::pair
#include <sockets.hh>
#include "impl/base.hh"

NS_HSHA_BEGIN

class CServerCallback
{
public:
    //typedefs
    typedef marine::CSockAddr                   __SockAddr;
    typedef marine::CSocket                     __Socket;
    typedef marine::CListenSocket               __Listen;
    typedef marine::CTcpConnSocket              __TcpConn;
    typedef std::pair<std::string, __SockAddr>  __SendElem;
    typedef std::deque<__SendElem>              __SendList;
    //functions
    virtual ~CServerCallback(){}
    //socket对象完成socket()系统调用成功之后调用
    //socket: 可能是tcp listen, tcp client或udp socket
    //return:
    //  true    回调成功
    //  false   关闭连接
    virtual bool onSocket(__Socket & socket){return true;}
    //socket对象完成bind()系统调用成功之后调用
    //socket: 可能是tcp listen, tcp client或udp socket
    //sendList: 返回要发送的数据，对于tcp listen socket会忽略
    //return:
    //  true    回调成功
    //  false   关闭连接
    virtual bool onBind(__Socket & socket, __SendList & sendList){return true;}
    //socket对象完成connect()系统调用成功之后调用
    //socket: 可能是tcp client或udp socket
    //sendList: 返回要发送的数据
    //return:
    //  true    回调成功
    //  false   关闭连接
    virtual bool onConnect(__Socket & socket, __SendList & sendList){return true;}
    //listen对象完成listen()系统调用成功之后调用
    //listen: 监听socket
    //return:
    //  true    回调成功
    //  false   关闭连接
    virtual bool onListen(__Listen & listen){return true;}
    //listen对象accept新连接之后调用
    //listen: 监听socket
    //client: 新客户端socket
    //clientCallback: 返回client的回调对象
    //      如果 *clientCallback = NULL，则client无回调对象
    //      如果 *clientCallback = this，则client复用listen的回调对象
    //      其他情况，则client使用指定的回调对象
    //注意: 回调对象的生命周期，由开发者管理
    //return:
    //  true    回调成功
    //  false   拒绝client，关闭client连接
    virtual bool onAccept(__Listen & listen, __TcpConn & client, CServerCallback ** clientCallback){
        return true;
    }
    //socket对象收到数据之后，在接收线程里调用，检查数据完整性和正确性
    //socket: 可能是tcp或udp socket
    //buf: 当前已收到的数据
    //sz: buf的字节长度
    //from: 数据来源地址
    //return:
    //  <0  数据格式错误
    //  0   数据不完整，需要继续接收
    //  >0  已使用的数据字节长度
    //  >sz 数据格式错误
    //注意: 调用结束后，buf会失效
    virtual ssize_t onRecv(__Socket & socket, const char * buf, size_t sz, const __SockAddr & from){
        return 0;
    }
    //socket对象收到数据之后，在处理线程里调用，处理数据
    //socket: 可能是tcp或udp socket
    //buf: 当前已收到的数据
    //sz: buf的字节长度
    //from: 数据来源地址
    //sendList: 返回要发送的数据
    //return:
    //  true    处理完成
    //  false   处理失败，关闭socket
    //注意: 调用结束后，buf会失效
    virtual bool onProcess(__Socket & socket, const char * buf, size_t sz, const __SockAddr & from, __SendList & sendList){
        return true;
    }
    //socket对象发送数据之后调用
    //socket: 可能是tcp或udp socket
    //buf: 已经发送的数据
    //sz: buf的字节长度
    //to: 数据目的地址
    //注意: 调用结束后，buf会失效
    virtual void onSend(__Socket & socket, const char * buf, size_t sz, const __SockAddr & to){}
    //socket对象close之前调用
    //socket: 可能是tcp listen, tcp client或udp socket
    //return:
    //  true    关闭后自动重启socket（如果可能的话）
    //  false   仅关闭socket
    virtual bool onClose(__Socket & socket){return true;}
    //Engine对象上报Attr统计之前调用
    //可用来统计自定义的数据
    virtual void onAttrReport(){}
    //Engine对象上报Attr统计
    //attr: 统计属性的id
    //value: 统计属性的值
    virtual void reportAttr(int attr, uint64_t value){}
    //输出可读字符串
    virtual std::string toString() const{return "DEFAULT";}
};

NS_HSHA_END

#endif

