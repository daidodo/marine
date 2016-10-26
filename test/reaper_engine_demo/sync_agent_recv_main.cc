#include <signal.h>
#include <data_stream.hh>
#include <configuration.hh>
#include <attr_stats.hh>
#include <posix_shm.hh>
#include <atomic_hash_table.hh>
#include <engine.hh>
#include <server_callback.hh>
#include "sync_cmd.hh"
#include "sv_attr.h"
#include "user.pb.h"
#include <logger.hh>

using namespace marine;
using namespace reaper;
using namespace im::server::sync::user;

CReaperEngine * pe;

const size_t kCapacity = 75000000;
const int kRow = 30;
const size_t kLen = 256;

static void SigHandler(int sig)
{
    if(SIGTERM == sig){
        if(pe)
            pe->stop();
    }
}

class CGlobal
{
    typedef std::vector<int> __Units;
    typedef std::vector<CSockAddr>  __Ips;
    typedef CAtomicSync<time_t>     __Time;
    typedef CAtomicSync<uint64_t>   __Val;
    typedef CPosixShm<char>         __Shm;
    typedef CAtomicHashTable<uint64_t>  __HashTable;
    //fields
    int appId_;
    int subAppId_;
    int unitCnt_;
    __Units units_;
    __Ips ipDr1_;
    __Ips ipDr2_;
    //delay report
    __Time max_, min_;
    __Val total_, cnt_;
    //hashtable
    __Shm shm_;
    __HashTable ht_;

    //functions
    bool loadUnit(const char * fname, __Units & u);
    bool loadIp(const char * fname, __Ips & ips);
    void reportDelay(const C2_SyncData & s);
    void transform(const C2_SyncData & s, User & user);
    void processLogin(const User & user);
    void processLogout(const User & user);
    void processChange(const User & user);
public:
    bool init(const char * exe);
    bool run();
    CSockAddr serverIp(int dr, int unit) const;
    int appId() const{return appId_;}
    int subAppId() const{return subAppId_;}
    int unitCnt() const{return unitCnt_;}
    const __HashTable & ht() const{return ht_;}
    void processSyncData(const C2_SyncData & s);
    void getDelay(time_t & max, time_t & min, uint64_t & total, uint64_t & cnt);
}g;

struct CSyncAgentRecvCallback : public CServerCallback
{
    enum EState{
        kInit,
        kWaitRegisterResp,
        kWaitHandShakeResp,
        kWaitPushData,
    };
    //fields
    int unitNo_;
    EState state_;
public:
    //functions
    CSyncAgentRecvCallback():unitNo_(0), state_(kInit){}
    void unitNo(int n){unitNo_ = n;}
    std::string toString() const{return "CSyncAgentRecvCallback";}
    bool onConnect(__Socket & socket, __SendList & sendList);
    ssize_t onRecv(__Socket & socket, const char * buf, size_t sz, const __SockAddr & from);
    bool onProcess(__Socket & socket, const char * buf, size_t sz, const __SockAddr & from, __SendList & sendList);
    bool onClose(__Socket & socket);
    void onAttrReport();
    void reportAttr(int attr, uint64_t value);
private:
    bool processRegisterResp(__Socket & socket, const char * buf, size_t sz, const __SockAddr & from, __SendList & sendList);
    bool processHandshakeResp(__Socket & socket, const char * buf, size_t sz, const __SockAddr & from, __SendList & sendList);
    bool processPushData(__Socket & socket, const char * buf, size_t sz, const __SockAddr & from, __SendList & sendList);
};

bool CGlobal::loadUnit(const char * fname, __Units & u)
{
    CConfiguration config;
    if(!config.load("unit.conf")){
        FATAL("cannot open 'unit.conf'");
        return false;
    }
    u.resize(1000);
    for(size_t i = 0;i < u.size();++i){
        CToString oss;
        oss<<i;
        u[i] = config.getInt(oss.str());
    }
    return true;
}

bool CGlobal::loadIp(const char * fname, __Ips & ips)
{
    assert(fname);
    CConfiguration config;
    if(!config.load(fname)){
        FATAL("cannot open '"<<fname<<"'");
        return false;
    }
    ips.resize(30);
    for(size_t i = 0;i < ips.size();++i){
        CToString oss;
        oss<<(i + 1);
        const std::string ip = config.getString(oss.str());
        if(!ips[i].setIp(ip)){
            FATAL("invalid server ip="<<ip<<" in '"<<fname<<"' shard "<<(i + 1));
            return false;
        }
        if(!ips[i].setPort(7700)){
            FATAL("cannot set port in for shard "<<(i + 1));
            return false;
        }
    }
    return true;
}

bool CGlobal::init(const char * exe)
{
    //read conf
    std::string confFile = exe;
    confFile.append(".conf");
    CConfiguration config;
    if(!config.load(confFile)){
        FATAL("cannot open '"<<confFile<<"'");
        return false;
    }
    //init logger
    std::string level = config.getString("LOG_LEVEL");
    size_t sz = config.getInt<size_t>("LOG_FILE_SZ", 10 << 20, 1 << 20);
    int num = config.getInt("LOG_FILE_NUM", 5, 0);
    if(!LOGGER_INIT(std::string(exe) + ".log", level, sz, num)){
        FATAL("cannot init logger");
        return false;
    }
    //other
    appId_ = 1;
    subAppId_ = 3;
    unitCnt_ = config.getInt("UNIT_CNT", 1, 1, 1000);
    //unit
    if(!loadUnit("unit.conf", units_)){
        FATAL("load units failed");
        return false;
    }
    //server ip
    if(!loadIp("ip_dr1.conf", ipDr1_) || !loadIp("ip_dr2.conf", ipDr2_)){
        FATAL("load dr ip failed");
        return false;
    }
    //hashtable & shm
    /*
    sz = __HashTable::CalcBufSize(kCapacity, kRow, kLen);
    int ret = shm_.init("/atomic_hash_table", sz);
    if(ret < 0){
        FATAL("shm_.init('/atomic_hash_table', "<<sz<<") failed");
        return false;
    }else if(ret > 0){
        if(!ht_.init(shm_.ptr(), shm_.size(), kCapacity, kRow, kLen, true)){
            FATAL("ht_.init(sz="<<sz<<", kCapacity="<<kCapacity<<", kRow="<<kRow<<", kLen="<<kLen<<", create) failed");
            return false;
        }
    }else{
        if(!ht_.init(shm_.ptr(), shm_.size())){
            FATAL("ht_.init(sz="<<sz<<") failed");
            return false;
        }
    }
    */
    return true;
}

CSockAddr CGlobal::serverIp(int dr, int unit) const
{
    if(unit < 0 || size_t(unit) >= units_.size())
        return CSockAddr();
    int shard = units_[unit];
    const __Ips & ips = (1 == dr ? ipDr1_ : ipDr2_);
    if(shard < 1 || size_t(shard) > ips.size())
        return CSockAddr();
    return ips[shard - 1];
}

void CGlobal::transform(const C2_SyncData & msg, User & user)
{
    if(msg.qwUin)
        user.set_uin(msg.qwUin);
    if(msg.wAppId){
        App * app = user.add_apps();
        app->set_id(msg.wAppId);
        if(msg.dwInstanceId){
            Inst * inst = app->add_insts();
            inst->set_id(msg.dwInstanceId);
            if(msg.qwInstanceSeq)
                inst->set_normal_seq(msg.qwInstanceSeq);
            if(msg.dwInstanceStatusTime)
                inst->set_status_time(msg.dwInstanceStatusTime);
            inst->set_local_time(tools::Time(NULL));
            if(msg.dwInstanceFromIp)
                inst->set_from_ip(msg.dwInstanceFromIp);
            if(msg.cFlag)
                inst->set_flags(msg.cFlag);
            if(msg.wSubAppId)
                inst->set_sub_appid(msg.wSubAppId);
            if(msg.wRealRegion)
                inst->set_real_region(msg.wRealRegion);
            if(msg.dwLoginTime)
                inst->set_login_time(msg.dwLoginTime);
            if(msg.cType)
                inst->set_type(msg.cType);
            if(!msg.extra_.empty())
                inst->set_extras(msg.extra_);
            if(!msg.flv_.empty())
                inst->set_fields(msg.flv_);
            if(!msg.addinfo_.empty())
                inst->set_addinfo(msg.addinfo_);
        }
    }
}

void CGlobal::processLogin(const User & user)
{
    processChange(user);
}

void CGlobal::processLogout(const User & user)
{
    return;
    size_t sz = ht_.remove(user.uin());
    if(0 == sz){
        ERROR("remove none for user="<<user.DebugString());
    }
}

void CGlobal::processChange(const User & user)
{
    return;
    std::string buf;
    if(!user.SerializeToString(&buf)){
        ERROR("SerializeToString() failed for user="<<user.DebugString());
        return;
    }
    if(!ht_.update(user.uin(), buf)){
        ERROR("update hash table failed for user="<<user.DebugString());
    }
}

void CGlobal::processSyncData(const C2_SyncData & s)
{
    reportDelay(s);
    User user;
    transform(s, user);
    switch(s.cType){
        case 0: //login
            processLogin(user);
            break;
        case 1: //logout
            processLogout(user);
            break;
        case 3: //change
            processChange(user);
            break;
        default:;   //skip
    }
}

void CGlobal::reportDelay(const C2_SyncData & s)
{
    if(s.cFlag & 1)
        return; //跳过全量同步的sync data
    time_t cur = s.dwInstanceStatusTime;
    time_t now = tools::Time(NULL);
    if(cur < now)
        cur = now - cur;
    else
        cur = 0;
    total_ += cur;
    if(0 == cnt_++)
        min_ = cur;
    else if(cur < min_)
        min_ = cur;
    if(cur > max_)
        max_ = cur;
}

void CGlobal::getDelay(time_t & max, time_t & min, uint64_t & total, uint64_t & cnt)
{
    max = max_.swap(0);
    min = min_.swap(0);
    total = total_.swap(0);
    cnt = cnt_.swap(0);
}

bool CSyncAgentRecvCallback::onConnect(__Socket & socket, __SendList & sendList)
{
    //send register
    COutByteStream ds;
    CCmdRegister cmd(unitNo_);
    encodePkg(ds, cmd);
    std::string buf;
    if(!ds.finish(buf)){
        ERROR("encode register cmd failed for callback="<<toString());
        return false;
    }
    DEBUG("send register for unitNo_="<<unitNo_<<" to socket="<<socket.toString());
    sendList.push_back(__SendElem(buf, CSockAddr()));
    state_ = kWaitRegisterResp;
    return true;
}

ssize_t CSyncAgentRecvCallback::onRecv(__Socket & socket, const char * buf, size_t sz, const __SockAddr & from)
{
    DEBUG("recv buf="<<tools::DumpXxd(buf, sz)<<" from "<<from.toString());
    CInByteStream ds(buf, sz);
    uint16_t len = 0;
    if(!(ds>>len))
        return 0;
    if(len > 4096)
        return -1;
    if(!(ds>>Manip::skip(len)))
        return 0;
    return ds.cur();
}

bool CSyncAgentRecvCallback::onProcess(__Socket & socket, const char * buf, size_t sz, const __SockAddr & from, __SendList & sendList)
{
    switch(state_){
        case kWaitRegisterResp:
            return processRegisterResp(socket, buf, sz, from, sendList);
        case kWaitHandShakeResp:
            return processHandshakeResp(socket, buf, sz, from, sendList);
        case kWaitPushData:
            return processPushData(socket, buf, sz, from, sendList);
        default:;
    }
    ERROR("recv unexpected buf in state_="<<state_<<", buf="<<tools::Dump(buf, sz)<<" from "<<from.toString());
    return false;
}

bool CSyncAgentRecvCallback::processRegisterResp(__Socket & socket, const char * buf, size_t sz, const __SockAddr & from, __SendList & sendList)
{
    //register resp
    {
        CInByteStream ds(buf, sz);
        CCmdRegisterResp cmd;
        if(!decodePkg(ds, cmd)){
            ERROR("format error buf="<<tools::Dump(buf, sz)<<" from "<<from.toString());
            return false;
        }
        if(cmd.head_.wCmd != CMD_REGISTER_RESP){
            ERROR("register resp wCmd="<<cmd.head_.wCmd<<" is not CMD_REGISTER_RESP("<<CMD_REGISTER_RESP<<") for buf="<<tools::Dump(buf, sz)<<" from "<<from.toString());
            return false;
        }
        if(cmd.head_.cResult != 0){
            ERROR("register resp cResult="<<(int)cmd.head_.cResult<<" for buf="<<tools::Dump(buf, sz)<<" from "<<from.toString());
            return false;
        }
        DEBUG("recv register resp from "<<from.toString());
    }
    //hand shake
    {
        COutByteStream ds;
        CCmdHandShake cmd(unitNo_);
        cmd.dwLastLinkTime = tools::Time(NULL);
        encodePkg(ds, cmd);
        std::string buf;
        if(!ds.finish(buf)){
            ERROR("encode handshake failed for callback="<<toString());
            return false;
        }
        DEBUG("send handshake for unitNo_="<<unitNo_<<" to socket="<<socket.toString());
        sendList.push_back(__SendElem(buf, CSockAddr()));
        state_ = kWaitHandShakeResp;
    }
    return true;
}

bool CSyncAgentRecvCallback::processHandshakeResp(__Socket & socket, const char * buf, size_t sz, const __SockAddr & from, __SendList & sendList)
{
    //hand shake resp
    CInByteStream ds(buf, sz);
    CCmdHandShakeResp cmd;
    if(!decodePkg(ds, cmd)){
        ERROR("format error buf="<<tools::Dump(buf, sz)<<" from "<<from.toString());
        return false;
    }
    if(cmd.head_.wCmd != CMD_HAND_SHAKE_RESP){
        ERROR("register resp wCmd="<<cmd.head_.wCmd<<" is not CMD_HAND_SHAKE_RESP("<<CMD_HAND_SHAKE_RESP<<") for buf="<<tools::Dump(buf, sz)<<" from "<<from.toString());
        return false;
    }
    if(cmd.head_.cResult != 0){
        ERROR("register resp cResult="<<(int)cmd.head_.cResult<<" for buf="<<tools::Dump(buf, sz)<<" from "<<from.toString());
        return false;
    }
    DEBUG("recv hand shake resp from "<<from.toString());
    state_ = kWaitPushData;
    return true;
}

bool CSyncAgentRecvCallback::processPushData(__Socket & socket, const char * buf, size_t sz, const __SockAddr & from, __SendList & sendList)
{
    CInByteStream ds(buf, sz);
    CCmdPushData cmd;
    if(!decodePkg(ds, cmd)){
        ERROR("format error buf="<<tools::DumpXxd(buf, sz)<<" from "<<from.toString());
        ATTR_ADD(221271, 1);
        return true;
    }
    if(cmd.head_.wCmd != CMD_PUSH_DATA){
        ERROR("push data wCmd="<<cmd.head_.wCmd<<" is not CMD_PUSH_DATA("<<CMD_PUSH_DATA<<") for buf="<<tools::Dump(buf, sz)<<" from "<<from.toString());
        ATTR_ADD(221271, 1);
        return true;
    }
    if(cmd.head_.cResult != 0){
        ERROR("push data cResult="<<(int)cmd.head_.cResult<<" for buf="<<tools::Dump(buf, sz)<<" from "<<from.toString());
        ATTR_ADD(221271, 1);
        return true;
    }
    INFO("recv "<<cmd.syncDatas_.size()<<" push data from "<<from.toString());
    ATTR_ADD(109210, cmd.syncDatas_.size());
    typedef CCmdPushData::__SyncDataArr::const_iterator __Iter;
    for(__Iter i = cmd.syncDatas_.begin();i != cmd.syncDatas_.end();++i){
        INFO("recv SyncData="<<i->toString());
        g.processSyncData(*i);
    }
    return true;
}

bool CSyncAgentRecvCallback::onClose(__Socket & socket)
{
    ERROR("close socket="<<socket.toString());
    return true;    //re-connect
}

void CSyncAgentRecvCallback::onAttrReport()
{
    //delay
    time_t max, min;
    uint64_t total, cnt;
    g.getDelay(max, min, total, cnt);
    uint64_t x;
    ATTR_SET_EX(111085, min, &x);   //x is for test
    ATTR_SET(111086, max);
    if(cnt)
        total /= cnt;
    ATTR_SET(111087, total);
    ATTR_SET(111088, cnt);
    //hash table
    cnt = g.ht().size();
    total = g.ht().capacity();
    if(!total)
        total = 1;
    ATTR_SET(137307, cnt);
    ATTR_SET(109239, cnt * 1000 / total);
    //memory
    size_t vm = 0, rs = 0;
    if(tools::GetProcessMemoryUsage(&vm, &rs)){
        ATTR_SET(136241, (vm + 1023) >> 10);
        ATTR_SET(136244, (rs + 1023) >> 10);
    }else{
        ERROR("GetProcessMemoryUsage() failed");
    }
}

void CSyncAgentRecvCallback::reportAttr(int attr, uint64_t value)
{
    const int kAttrs[ATTR_MAX] = {
        0,
        307703,     //Reaper: Accept新客户端
        307704,     //Reaper: 关闭socket
        307705,     //Reaper: 通知读事件
        307706,     //Reaper: 通知写事件
        307707,     //Reaper: Tcp Recv成功
        307708,     //Reaper: Tcp Send成功
        307709,     //Reaper: Udp Recv成功
        307710,     //Reaper: Udp Send成功
        307711,     //Reaper: Push EventQue成功
        307712,     //Reaper: Pop EventQue成功
        307713,     //Reaper: Push EventQue失败,warning
        307714,     //Reaper: Push AddingQue成功
        307715,     //Reaper: Pop AddingQue成功
        307716,     //Reaper: Push AddingQue失败,warning
        307717,     //Reaper: Push PkgQue成功
        307718,     //Reaper: Pop PkgQue成功
        307719,     //Reaper: Push PkgQue失败,warning
        307720,     //Reaper: 向epoll中增加或修改fd
        307721,     //Reaper: 向epoll中增加或修改fd失败,warning
        307722,     //Reaper: epoll wait出错,warning
        307723,     //Reaper: 未知的Fd Type,warning
    };
//#define ATTR_THREAD_HANDLER_CREATE       307692   //CThreadManager: Handler线程创建
//#define ATTR_THREAD_HANDLER_EXIT         307693   //CThreadManager: Handler线程退出
    if(!value)
        return;
    if(attr < int(ARRAY_SIZE(kAttrs)))
        Attr_API_Set(kAttrs[attr], value);
    else
        Attr_API_Set(attr, value);
}

int main(int argc, const char ** argv)
{
    LOGGER_CRASH_HANDLER();
    //init
    if(!g.init(argv[0]))
        return 1;
    tools::DaemonMonitor();
    signal(SIGTERM, SigHandler);
    CReaperEngine e;
    pe = &e;
    //add conn
    std::vector<CSyncAgentRecvCallback> cb1(g.unitCnt());
    for(size_t i = 0;i < cb1.size();++i){
        cb1[i].unitNo(i);
        const CSockAddr addr = g.serverIp(1, i);
        if(!e.addTcpClient(addr, &cb1[i])){
            FATAL("engine.addTcpClient(proxy="<<addr.toString()<<", uintNo="<<i<<") failed");
            return 1;
        }
    }
    std::vector<CSyncAgentRecvCallback> cb2(g.unitCnt());
    for(size_t i = 0;i < cb2.size();++i){
        cb2[i].unitNo(i);
        const CSockAddr addr = g.serverIp(2, i);
        if(!e.addTcpClient(addr, &cb2[i])){
            FATAL("engine.addTcpClient(proxy="<<addr.toString()<<", uintNo="<<i<<") failed");
            return 1;
        }
    }
    //start
    CSyncAgentRecvCallback cb;
    CReaperParams params;
    params.maxFdNum_ = 10000;
    params.callback_ = &cb;
    if(!e.run(params)){
        FATAL("start engine failed");
        return 1;
    }
    //stop
    WARN("main thread exit!");
    return 0;
}
