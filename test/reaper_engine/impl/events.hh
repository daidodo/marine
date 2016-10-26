#ifndef DOZERG_EVENTS_H_20130506
#define DOZERG_EVENTS_H_20130506

#include <template.hh>      //ARRAY_SIZE
#include <tools/debug.hh>   //tools::ToStringBits
#include "base.hh"

NS_HSHA_IMPL_BEGIN

//event flags
typedef uint32_t __Events;

const __Events kEventClose = 1 << 0;
const __Events kEventIn = 1 << 1;
const __Events kEventOut = 1 << 2;
const __Events kEventAccept = 1 << 3;
const __Events kEventTcpRecv = 1 << 4;
const __Events kEventUdpRecv = 1 << 5;
const __Events kEventTcpSend = 1 << 6;
const __Events kEventUdpSend = 1 << 7;
const __Events kEventRead = 1 << 6;
const __Events kEventWrite = 1 << 7;

namespace events{
    inline bool NeedClose(__Events ev){return (ev & kEventClose);}
    inline bool CanInput(__Events ev){return (ev & kEventIn);}
    inline bool CanOutput(__Events ev){return (ev & kEventOut);}
    inline bool CanAccept(__Events ev){return (ev & kEventAccept);}
    inline bool CanTcpRecv(__Events ev){return (ev & kEventTcpRecv);}
    inline bool CanUdpRecv(__Events ev){return (ev & kEventUdpRecv);}
    inline bool CanTcpSend(__Events ev){return (ev & kEventTcpSend);}
    inline bool CanUdpSend(__Events ev){return (ev & kEventUdpSend);}
    inline bool CanRecv(__Events ev){return CanTcpRecv(ev) || CanUdpRecv(ev);}
    inline bool CanSend(__Events ev){return CanTcpSend(ev) || CanUdpSend(ev);}
    inline bool CanRead(__Events ev){return (ev & kEventRead);}
    inline bool CanWrite(__Events ev){return (ev & kEventWrite);}
    inline bool NeedInput(__Events ev){return CanInput(ev) || CanRecv(ev) || CanAccept(ev) || CanRead(ev);}
    inline bool NeedOutput(__Events ev){return CanOutput(ev) || CanSend(ev) || CanWrite(ev);}
    inline std::string ToString(__Events ev){
        const char * const kBitName[] = {
            "CLOSE",
            "IN",
            "OUT",
            "ACCEPT",
            "TCP_RECV",
            "UDP_RECV",
            "TCP_SEND",
            "UDP_SEND",
            "READ",
            "WRITE",
        };
        return marine::tools::ToStringBits(ev, kBitName, ARRAY_SIZE(kBitName));
    }
}//namespace Events

class CFdEvent
{
public:
    static const int & ExtractFd(const CFdEvent & fe){return fe.fd_;}
    CFdEvent(int f, __Events e):fd_(f),ev_(e){}
    int fd() const{return fd_;}
    __Events events() const{return ev_;}
private:
    int fd_;
    __Events ev_;
};

NS_HSHA_IMPL_END

#endif

