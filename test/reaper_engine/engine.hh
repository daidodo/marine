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
    uint32_t maxFdNum_;         //最多打开文件个数，小于系统默认值时忽略
    int epollTimeoutMs_;        //epoll超时时间，毫秒
    size_t notifyStackSz_;      //notify线程栈大小，0:使用默认值；其他:自定义
    size_t ioStackSz_;          //io线程栈大小，0:使用默认值；其他:自定义
    size_t processStackSz_;     //处理线程栈大小，0:使用默认值；其他:自定义
    int processThreadCountMin_; //处理线程数最小值，0:使用默认值(1)；其他:自定义
    int processThreadCountMax_; //处理线程数最大值，0:使用默认值(根据处理器个数调整)；其他:自定义
    CServerCallback * callback_;//统计属性上报的回调对象，NULL:不启动统计上报；其他:启用统计上报
    int reportInterval_;        //统计属性上报的周期，秒，小于1:1；其他:自定义
    size_t attrCount_;          //统计属性的个数上限，小于1000:1000，其他:自定义
    bool nowait_;               //false:run()等待所有线程结束才返回；true:run()启动完成后立即返回
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
    //构造/析构，单线程安全
    CReaperEngine();
    ~CReaperEngine();
    //增加TCP服务器，多线程安全
    //hostAddr: 监听地址
    //callback: 回调对象
    bool addTcpServer(const __SockAddr & hostAddr, CServerCallback * callback);
    //增加TCP客户端，多线程安全
    //hostAddr: 本端绑定地址
    //peerAddr: 对方服务器地址
    //callback: 回调对象
    bool addTcpClient(const __SockAddr & peerAddr, CServerCallback * callback);
    bool addTcpClient(const __SockAddr & hostAddr, const __SockAddr & peerAddr, CServerCallback * callback);
    //增加UDP连接，多线程安全
    //hostAddr: 本端绑定地址
    //peerAddr: 对方服务器地址
    //callback: 回调对象
    bool addUdp(const __SockAddr & hostAddr, const __SockAddr & peerAddr, CServerCallback * callback);
    //开始运行，只能调用一次，单线程安全
    bool run(const CReaperParams & params = CReaperParams());
    //通知所有线程结束运行，单线程安全
    void stop();
    //检查所有线程是否结束，多线程安全
    bool stopped() const volatile;
private:
    CReaperEngine(const CReaperEngine &);   //disable copy and assignment
    CReaperEngine & operator =(const CReaperEngine &);
    //field
    NS_HSHA_IMPL::CHshaEngineImpl * impl_;
};

NS_HSHA_END

#endif

