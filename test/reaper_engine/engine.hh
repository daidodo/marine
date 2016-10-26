#ifndef DOZERG_ENGINE_H_20130606
#define DOZERG_ENGINE_H_20130606

#include "types.hh"

NS_HSHA_IMPL_BEGIN

class CHshaEngineImpl;

NS_HSHA_IMPL_END

NS_HSHA_BEGIN

class CServerCallback;

struct CReaperParams
{
    uint32_t maxFdNum_;         //�����ļ�������С��ϵͳĬ��ֵʱ����
    int epollTimeoutMs_;        //epoll��ʱʱ�䣬����
    size_t notifyStackSz_;      //notify�߳�ջ��С��0:ʹ��Ĭ��ֵ������:�Զ���
    size_t ioStackSz_;          //io�߳�ջ��С��0:ʹ��Ĭ��ֵ������:�Զ���
    size_t processStackSz_;     //�����߳�ջ��С��0:ʹ��Ĭ��ֵ������:�Զ���
    int processThreadCountMin_; //�����߳�����Сֵ��0:ʹ��Ĭ��ֵ(1)������:�Զ���
    int processThreadCountMax_; //�����߳������ֵ��0:ʹ��Ĭ��ֵ(���ݴ�������������)������:�Զ���
    CServerCallback * callback_;//ͳ�������ϱ��Ļص�����NULL:������ͳ���ϱ�������:����ͳ���ϱ�
    int reportInterval_;        //ͳ�������ϱ������ڣ��룬С��1:1������:�Զ���
    size_t attrCount_;          //ͳ�����Եĸ������ޣ�С��1000:1000������:�Զ���
    bool nowait_;               //false:run()�ȴ������߳̽����ŷ��أ�true:run()������ɺ���������
    //defaults
    CReaperParams()
        : maxFdNum_(0)
        , epollTimeoutMs_(500)
        , notifyStackSz_(0)
        , ioStackSz_(0)
        , processStackSz_(0)
        , processThreadCountMin_(0)
        , processThreadCountMax_(0)
        , callback_(NULL)
        , reportInterval_(60)
        , attrCount_(0)
        , nowait_(false)
    {}
};

class CReaperEngine
{
public:
    //����/���������̰߳�ȫ
    CReaperEngine();
    ~CReaperEngine();
    //����TCP�����������̰߳�ȫ
    //hostAddr: ������ַ
    //callback: �ص�����
    bool addTcpServer(const __SockAddr & hostAddr, CServerCallback * callback);
    //����TCP�ͻ��ˣ����̰߳�ȫ
    //hostAddr: ���˰󶨵�ַ
    //peerAddr: �Է���������ַ
    //callback: �ص�����
    bool addTcpClient(const __SockAddr & peerAddr, CServerCallback * callback);
    bool addTcpClient(const __SockAddr & hostAddr, const __SockAddr & peerAddr, CServerCallback * callback);
    //����UDP���ӣ����̰߳�ȫ
    //hostAddr: ���˰󶨵�ַ
    //peerAddr: �Է���������ַ
    //callback: �ص�����
    bool addUdp(const __SockAddr & hostAddr, const __SockAddr & peerAddr, CServerCallback * callback);
    //��ʼ���У�ֻ�ܵ���һ�Σ����̰߳�ȫ
    bool run(const CReaperParams & params = CReaperParams());
    //֪ͨ�����߳̽������У����̰߳�ȫ
    void stop();
    //��������߳��Ƿ���������̰߳�ȫ
    bool stopped() const volatile;
private:
    CReaperEngine(const CReaperEngine &);   //disable copy and assignment
    CReaperEngine & operator =(const CReaperEngine &);
    //field
    NS_HSHA_IMPL::CHshaEngineImpl * impl_;
};

NS_HSHA_END

#endif

