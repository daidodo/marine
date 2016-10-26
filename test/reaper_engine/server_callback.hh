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
    //socket�������socket()ϵͳ���óɹ�֮�����
    //socket: ������tcp listen, tcp client��udp socket
    //return:
    //  true    �ص��ɹ�
    //  false   �ر�����
    virtual bool onSocket(__Socket & socket){return true;}
    //socket�������bind()ϵͳ���óɹ�֮�����
    //socket: ������tcp listen, tcp client��udp socket
    //sendList: ����Ҫ���͵����ݣ�����tcp listen socket�����
    //return:
    //  true    �ص��ɹ�
    //  false   �ر�����
    virtual bool onBind(__Socket & socket, __SendList & sendList){return true;}
    //socket�������connect()ϵͳ���óɹ�֮�����
    //socket: ������tcp client��udp socket
    //sendList: ����Ҫ���͵�����
    //return:
    //  true    �ص��ɹ�
    //  false   �ر�����
    virtual bool onConnect(__Socket & socket, __SendList & sendList){return true;}
    //listen�������listen()ϵͳ���óɹ�֮�����
    //listen: ����socket
    //return:
    //  true    �ص��ɹ�
    //  false   �ر�����
    virtual bool onListen(__Listen & listen){return true;}
    //listen����accept������֮�����
    //listen: ����socket
    //client: �¿ͻ���socket
    //clientCallback: ����client�Ļص�����
    //      ��� *clientCallback = NULL����client�޻ص�����
    //      ��� *clientCallback = this����client����listen�Ļص�����
    //      �����������clientʹ��ָ���Ļص�����
    //ע��: �ص�������������ڣ��ɿ����߹���
    //return:
    //  true    �ص��ɹ�
    //  false   �ܾ�client���ر�client����
    virtual bool onAccept(__Listen & listen, __TcpConn & client, CServerCallback ** clientCallback){
        return true;
    }
    //socket�����յ�����֮���ڽ����߳�����ã�������������Ժ���ȷ��
    //socket: ������tcp��udp socket
    //buf: ��ǰ���յ�������
    //sz: buf���ֽڳ���
    //from: ������Դ��ַ
    //return:
    //  <0  ���ݸ�ʽ����
    //  0   ���ݲ���������Ҫ��������
    //  >0  ��ʹ�õ������ֽڳ���
    //  >sz ���ݸ�ʽ����
    //ע��: ���ý�����buf��ʧЧ
    virtual ssize_t onRecv(__Socket & socket, const char * buf, size_t sz, const __SockAddr & from){
        return 0;
    }
    //socket�����յ�����֮���ڴ����߳�����ã���������
    //socket: ������tcp��udp socket
    //buf: ��ǰ���յ�������
    //sz: buf���ֽڳ���
    //from: ������Դ��ַ
    //sendList: ����Ҫ���͵�����
    //return:
    //  true    �������
    //  false   ����ʧ�ܣ��ر�socket
    //ע��: ���ý�����buf��ʧЧ
    virtual bool onProcess(__Socket & socket, const char * buf, size_t sz, const __SockAddr & from, __SendList & sendList){
        return true;
    }
    //socket����������֮�����
    //socket: ������tcp��udp socket
    //buf: �Ѿ����͵�����
    //sz: buf���ֽڳ���
    //to: ����Ŀ�ĵ�ַ
    //ע��: ���ý�����buf��ʧЧ
    virtual void onSend(__Socket & socket, const char * buf, size_t sz, const __SockAddr & to){}
    //socket����close֮ǰ����
    //socket: ������tcp listen, tcp client��udp socket
    //return:
    //  true    �رպ��Զ�����socket��������ܵĻ���
    //  false   ���ر�socket
    virtual bool onClose(__Socket & socket){return true;}
    //Engine�����ϱ�Attrͳ��֮ǰ����
    //������ͳ���Զ��������
    virtual void onAttrReport(){}
    //Engine�����ϱ�Attrͳ��
    //attr: ͳ�����Ե�id
    //value: ͳ�����Ե�ֵ
    virtual void reportAttr(int attr, uint64_t value){}
    //����ɶ��ַ���
    virtual std::string toString() const{return "DEFAULT";}
};

NS_HSHA_END

#endif

