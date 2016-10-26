#include <data_stream.hh>
#include <server_callback.hh>
#include <engine.hh>
#include "cmd.hh"
#include "comm.h"

CReaperEngine e;

static std::string pkgBuf()
{
    CReqCmd req;
    req.ver_ = 10;
    req.time_ = tools::Time(NULL);
    req.resv_ = "This is reserved data";
    std::string str;
    COutByteStream out;
    out<<uint32_t(0);
    size_t off = out.cur();
    out<<req;
    out<<Manip::offset_value<uint32_t>(0, out.cur() - off)
        <<Manip::end(str);
    return str;
}

class CTcpClientCallback : public CServerCallback
{
    std::string pkg_;
public:
    bool sendPkg(__Socket & socket, __SendList & sendList){
        if(pkg_.empty())
            pkg_ = pkgBuf();
        sendList.push_back(__SendElem(pkg_, __SockAddr()));
        return true;
    }
    virtual bool onConnect(__Socket & socket, __SendList & sendList){
        DEBUG("finish connect() for sock="<<socket.toString());
        return sendPkg(socket, sendList);
    }
    virtual ssize_t onRecv(__Socket & socket, const char * buf, size_t sz, const __SockAddr & from){
        DEBUG("recv from "<<from.toString()<<", buf="<<tools::DumpHex(buf, sz)<<" for socket="<<socket.toString());
        CInByteStream ds(buf, sz);
        uint32_t len = 0;
        ds>>len;
        if(len > 1000)
            return -1;
        if(!(ds>>Manip::skip(len)))
            return 0;
        return ds.cur();
    }
    virtual bool onProcess(__Socket & socket, const char * buf, size_t sz, const __SockAddr & from, __SendList & sendList){
        DEBUG("process data from "<<from.toString()<<", buf="<<tools::DumpHex(buf, sz)<<" for socket="<<socket.toString());
        //decode
        CInByteStream ds(buf, sz);
        uint32_t len = 0;
        ds>>len;
        if(len > 1000)
            return -1;
        if(len > ds.left()){
            ERROR("format error len="<<len<<" in buf="<<tools::DumpHex(buf, sz)<<" from "<<from.toString()<<" for socket="<<socket.toString());
            return false;
        }
        CRespCmd resp;
        if(!(ds>>resp)){
            ERROR("format error CRespCmd in buf="<<tools::DumpHex(buf, sz)<<" from "<<from.toString()<<" for socket="<<socket.toString());
            return false;
        }
        INFO("recv resp="<<resp.toString()<<" from "<<from.toString()<<" for socket="<<socket.toString());
        //send req again
        return sendPkg(socket, sendList);
    }
    virtual void onSend(__Socket & socket, const char * buf, size_t sz, const __SockAddr & to){
        DEBUG("send to "<<to.toString()<<", buf="<<tools::DumpHex(buf, sz)<<" for socket="<<socket.toString());
    }
    virtual bool onClose(__Socket & socket){
        ERROR("close socket="<<socket.toString());
        return true;
    }
    virtual std::string toString() const{return "CTcpClientCallback";}
};

int main(int argc, const char ** argv)
{
    if(!init(argv[0]))
        return 1;
    CTcpClientCallback cb;
    for(int i = 0;i < g_clients;++i)
        e.addTcpClient(CSockAddr("127.0.0.1", 12345), &cb);
    CReaperParams params;
    params.maxFdNum_ = 10000;
    e.run(params);
    FATAL("client exit");
    return 0;
}
