#ifndef DOZERG_CMD_H
#define DOZERG_CMD_H

#include <data_stream.hh>
#include <to_string.hh>

struct CReqCmd
{
    uint8_t ver_;
    uint32_t time_;
    std::string resv_;
    std::string toString() const{
        marine::CToString oss;
        oss<<"{ver_="<<int(ver_)
            <<", time_="<<time_
            <<", resv_="<<marine::tools::Dump(resv_)
            <<"}";
        return oss.str();
    }
};

inline marine::CInByteStream operator >>(marine::CInByteStream & ds, CReqCmd & cmd)
{
    return (ds>>cmd.ver_
            >>cmd.time_
            >>cmd.resv_);
}

inline marine::COutByteStream operator <<(marine::COutByteStream & ds, const CReqCmd & cmd)
{
    return (ds<<cmd.ver_
            <<cmd.time_
            <<cmd.resv_);
}

struct CRespCmd
{
    uint8_t ver_;
    std::string time_;
    std::string resv_;
    std::string toString() const{
        marine::CToString oss;
        oss<<"{ver_="<<int(ver_)
            <<", time_="<<marine::tools::Dump(time_)
            <<", resv_="<<marine::tools::Dump(resv_)
            <<"}";
        return oss.str();
    }
};

inline marine::CInByteStream operator >>(marine::CInByteStream & ds, CRespCmd & cmd)
{
    return (ds>>cmd.ver_
            >>cmd.time_
            >>cmd.resv_);
}

inline marine::COutByteStream operator <<(marine::COutByteStream & ds, const CRespCmd & cmd)
{
    return (ds<<cmd.ver_
            <<cmd.time_
            <<cmd.resv_);
}

#endif

