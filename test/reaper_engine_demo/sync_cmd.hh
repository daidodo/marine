#include <string>
#include <sock_addr.hh>
#include <data_stream.hh>
#include <tools/time.hh>

using namespace marine;

#if 0
#   define __RET_FALSE     throw 1
#else
#   define __RET_FALSE     return false
#endif

#define CMD_REGISTER        1
#define CMD_REGISTER_RESP   2
#define CMD_HAND_SHAKE      3
#define CMD_HAND_SHAKE_RESP 4
#define CMD_PUSH_DATA       5
#define CMD_PUSH_DATA_RESP  6
#define CMD_HELLO           7

template<class C>
void encodePkg(COutByteStream & ds, const C & cmd)
{
    const size_t off1 = ds.cur();
    ds<<uint16_t(0);
    const size_t off2 = ds.cur();
    ds<<'{'<<'{'<<cmd<<'}'<<'}';
    ds<<Manip::offset_value<uint16_t>(off1, ds.cur() - off2);
}

template<class C>
bool decodePkg(CInByteStream & ds, C & cmd)
{
    uint16_t len = 0;
    char tx[2];
    if(!(ds>>len))
        __RET_FALSE;
    if(len != ds.left())
        __RET_FALSE;
    if(!(ds>>tx))
        __RET_FALSE;
    if(tx[0] != '{' || tx[1] != '{')
        __RET_FALSE;
    if(!(ds>>cmd))
        __RET_FALSE;
    if(ds.left() < 2)
        __RET_FALSE;
    ds>>Manip::skip(ds.left() - 2)
        >>tx;
    if(tx[0] != '}' || tx[1] != '}')
        __RET_FALSE;
    return true;
}

struct CPkgHead
{
    uint8_t cVer;
    uint16_t wUnitNo;
    uint16_t wCmd;
    uint8_t cResult;
    CPkgHead():cVer(0), wUnitNo(0), wCmd(0), cResult(0){}
    CPkgHead(uint16_t unit, uint16_t cmd):cVer(3), wUnitNo(unit), wCmd(cmd), cResult(0){}
};

inline COutByteStream & operator <<(COutByteStream & ds, const CPkgHead & msg)
{
    return (ds<<msg.cVer<<msg.wUnitNo<<msg.wCmd<<msg.cResult);
}

inline CInByteStream & operator >>(CInByteStream & ds, CPkgHead & msg)
{
    return (ds>>msg.cVer>>msg.wUnitNo>>msg.wCmd>>msg.cResult);
}

struct CRegisterTlv1
{
    typedef std::pair<uint16_t, uint16_t> __AppRange;
    typedef std::vector<__AppRange>     __AppRangeArr;
    uint16_t    wAppId;
    uint16_t    wSubAppId;
    uint16_t    wAuthType;
    std::string sAuthVal;
    __AppRangeArr apps_;
    uint16_t    wModuleId;
    uint16_t    wRegionId;
    CRegisterTlv1():wAppId(1), wSubAppId(3), wAuthType(0), wModuleId(0), wRegionId(0){}
};

inline COutByteStream & operator <<(COutByteStream & ds, const CRegisterTlv1 & msg)
{
    ds<<msg.wAppId<<msg.wSubAppId<<msg.wAuthType<<msg.sAuthVal;
    if(0 == msg.wAppId){
        ds<<uint16_t(msg.apps_.size());
        for(size_t i = 0;i < msg.apps_.size();++i)
            ds<<msg.apps_[i].first<<msg.apps_[i].second;
    }
    ds<<msg.wModuleId<<msg.wRegionId;
    return ds;
}

struct CRegisterTlv2
{
    uint64_t    qwUinStart;
    uint64_t    qwUinNum;
    CRegisterTlv2():qwUinStart(0), qwUinNum(0){}
};

inline COutByteStream & operator <<(COutByteStream & ds, const CRegisterTlv2 & msg)
{
    return (ds<<msg.qwUinStart<<msg.qwUinNum);
}

struct CRegisterTlv3
{
    typedef std::pair<uint16_t, uint16_t> __Feild;
    typedef std::vector<__Feild>     __FieldArr;
    __FieldArr fields_;
};

inline COutByteStream & operator <<(COutByteStream & ds, const CRegisterTlv3 & msg)
{
    ds<<uint16_t(msg.fields_.size());
    for(size_t i = 0;i < msg.fields_.size();++i)
        ds<<msg.fields_[i].first<<msg.fields_[i].second;
    return ds;
}

template<class V>
void addTlv(COutByteStream & ds, uint16_t t, const V & v)
{
    ds<<t;
    const size_t off1 = ds.cur();
    ds<<uint16_t(0);
    const size_t off2 = ds.cur();
    ds<<v;
    ds<<Manip::offset_value<uint16_t>(off1, ds.cur() - off2);
}

struct CCmdRegister
{
    CPkgHead head_;
    CRegisterTlv1    stTlv1;
    CRegisterTlv2    stTlv2;
    CRegisterTlv3    stTlv3;
    //�����ֶβ���Э���ֶΣ����Ǳ�ʾ�����Ӧ��Tlv�Ƿ�ʹ��
    int             iUseTlv2;
    int             iUseTlv3;
    CCmdRegister(int uintNo = 0):head_(uintNo, CMD_REGISTER), iUseTlv2(0), iUseTlv3(0){}
};

inline COutByteStream & operator <<(COutByteStream & ds, const CCmdRegister & msg)
{
    ds<<msg.head_;
    addTlv(ds, 1, msg.stTlv1);
    if(msg.iUseTlv2)
        addTlv(ds, 2, msg.stTlv2);
    if(msg.iUseTlv3)
        addTlv(ds, 3, msg.stTlv3);
    return ds;
}

struct CCmdRegisterResp
{
    CPkgHead head_;
};

inline CInByteStream & operator >>(CInByteStream & ds, CCmdRegisterResp & msg)
{
    return (ds>>msg.head_);
}

struct CCmdHandShake
{
    CPkgHead    head_;
    uint64_t    qwLastLinkSeq;
    uint32_t    dwLastLinkTime;
    uint32_t    dwTimeInterval;
    CCmdHandShake(int uintNo = 0):head_(uintNo, CMD_HAND_SHAKE), qwLastLinkSeq(0), dwLastLinkTime(0), dwTimeInterval(0){}
};

inline COutByteStream & operator <<(COutByteStream & ds, const CCmdHandShake & msg)
{
    return (ds<<msg.head_<<msg.qwLastLinkSeq<<msg.dwLastLinkTime<<msg.dwTimeInterval);
}

struct CCmdHandShakeResp
{
    CPkgHead    head_;
    uint8_t     cReconnectType;
    uint64_t    qwFromLinkSeq;
};

inline CInByteStream & operator >>(CInByteStream & ds, CCmdHandShakeResp & msg)
{
    return (ds>>msg.head_>>msg.cReconnectType>>msg.qwFromLinkSeq);
}

struct C2_SyncData
{
    uint8_t     cVer;
    uint64_t    qwUin;
    uint16_t    wAppId;
    uint32_t    dwInstanceId;
    uint8_t     cFlag;
    uint16_t    wRealRegion;
    uint16_t    wSubAppId;
    uint32_t    dwLoginTime;
    std::string extra_;
    uint32_t    dwInstanceFromIp;
    uint64_t    qwInstanceSeq;
    uint32_t    dwInstanceStatusTime;
    uint8_t     cType;
    std::string flv_;
    std::string addinfo_;
    C2_SyncData():cVer(0),qwUin(0),wAppId(0),dwInstanceId(0),cFlag(0),wRealRegion(0),wSubAppId(0),dwLoginTime(0),dwInstanceFromIp(0),qwInstanceSeq(0),dwInstanceStatusTime(0),cType(0){}
    std::string toString() const{
        CToString oss;
        oss<<"{cVer="<<int(cVer)
            <<", qwUin="<<qwUin
            <<", wAppId="<<wAppId
            <<", dwInstanceId="<<dwInstanceId
            <<", cFlag="<<int(cFlag)
            <<", wRealRegion="<<wRealRegion
            <<", wSubAppId="<<wSubAppId
            <<", dwLoginTime=("<<dwLoginTime<<")"<<tools::TimeString(dwLoginTime)
            <<", dwInstanceFromIp=("<<dwInstanceFromIp<<")"<<CSockAddr::IPv4String(dwInstanceFromIp, false)
            <<", qwInstanceSeq="<<qwInstanceSeq
            <<", dwInstanceStatusTime=("<<dwInstanceStatusTime<<")"<<tools::TimeString(dwInstanceStatusTime)
            <<", cType="<<int(cType)
            <<"}";
        return oss.str();
    }
};

inline CInByteStream & operator >>(CInByteStream & ds, C2_SyncData & msg)
{
    if(!(ds>>msg.cVer))
        return ds;
    uint32_t uin;
    switch(msg.cVer){
        case 1:     //SP_Normal
            if(!(ds>>msg.qwUin>>msg.wAppId>>msg.dwInstanceId>>msg.cFlag>>msg.wRealRegion))
                return ds;
            break;
        case 2:     //SP_Opt
            if(!(ds>>uin>>msg.wAppId>>msg.dwInstanceId>>msg.cFlag))
                return ds;
            msg.qwUin = uin;
            break;
        case 3:     //SP_ThinConn
            if(!(ds>>uin))
                return ds;
            msg.qwUin = uin;
            msg.wAppId = 1;
            msg.dwInstanceId = 1;
            break;
        default:return ds.bad(1);
    }
    if(((1 << 4) & msg.cFlag) && !(ds>>msg.wSubAppId))
        return ds;
    if(((1 << 3) & msg.cFlag) && !(ds>>msg.dwLoginTime))
        return ds;
    if(((1 << 5) & msg.cFlag) && !(ds>>msg.extra_))
        return ds;
    uint16_t flen, alen;
    if(!(ds>>Manip::host_order_value(msg.dwInstanceFromIp)>>msg.qwInstanceSeq>>msg.dwInstanceStatusTime>>msg.cType>>flen>>alen))
        return ds;
    return (ds>>Manip::raw(msg.flv_, flen)>>Manip::raw(msg.addinfo_, alen));
}

struct CCmdPushData
{
    typedef std::vector<C2_SyncData> __SyncDataArr;
    CPkgHead    head_;
    uint64_t    qwMinSeq;
    uint64_t    qwMaxSeq;
    uint32_t    dwLastLinkTime;
    __SyncDataArr syncDatas_;
};

inline CInByteStream & operator >>(CInByteStream & ds, CCmdPushData & msg)
{
    return (ds>>msg.head_>>msg.qwMinSeq>>msg.qwMaxSeq>>msg.dwLastLinkTime>>Manip::array(msg.syncDatas_));
}
