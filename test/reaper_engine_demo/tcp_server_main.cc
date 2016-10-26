#include <data_stream.hh>
#include <server_callback.hh>
#include <engine.hh>
#include "cmd.hh"
#include "comm.h"

class CTcpServerCallback : public CServerCallback
{
public:
    virtual bool onAccept(__Listen & listen, __TcpConn & client, CServerCallback ** clientCallback){
        DEBUG("new client="<<client.toString()<<" arrived for listen="<<listen.toString());
        assert(clientCallback);
        *clientCallback = this;
        return true;
    }
    virtual ssize_t onRecv(__Socket & socket, const char * buf, size_t sz, const __SockAddr & from){
        DEBUG("recv from "<<from.toString()<<", buf="<<tools::Dump(buf, sz)<<" for socket="<<socket.toString());
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
        DEBUG("process data from "<<from.toString()<<", buf="<<tools::Dump(buf, sz)<<" for socket="<<socket.toString());
        CInByteStream ds(buf, sz);
        uint32_t len = 0;
        ds>>len;
        if(len > 1000)
            return -1;
        if(len > ds.left()){
            ERROR("format error len="<<len<<" in buf="<<tools::Dump(buf, sz)<<" from "<<from.toString()<<" for socket="<<socket.toString());
            return false;
        }
        CReqCmd req;
        if(!(ds>>req)){
            ERROR("format error CReqCmd in buf="<<tools::Dump(buf, sz)<<" from "<<from.toString()<<" for socket="<<socket.toString());
            return false;
        }
        CRespCmd resp;
        resp.ver_ = req.ver_;
        resp.time_ = tools::TimeString(req.time_);
        resp.resv_ = req.resv_;
        INFO("process resp="<<resp.toString()<<" for req="<<req.toString()<<" from "<<from.toString()<<" for socket="<<socket.toString());
        COutByteStream out;
        std::string str;
        out<<uint32_t(0);
        size_t off = out.cur();
        out<<resp;
        out<<Manip::offset_value<uint32_t>(0, out.cur() - off)
            <<Manip::end(str);
        if(!out){
            ERROR("encode resp="<<resp.toString()<<" failed for req="<<req.toString()<<" from "<<from.toString()<<" for socket="<<socket.toString());
            return false;
        }
        sendList.push_back(__SendElem(str, from));
        return true;
    }
    virtual void onSend(__Socket & socket, const char * buf, size_t sz, const __SockAddr & to){
        DEBUG("send to "<<to.toString()<<", buf="<<tools::Dump(buf, sz)<<" for socket="<<socket.toString());
    }
    virtual bool onClose(__Socket & socket){
        ERROR("close socket="<<socket.toString());
        return true;
    }
    virtual std::string toString() const{return "CTcpServerCallback";}
};

int main(int argc, const char ** argv)
{
    if(!init(argv[0]))
        return 1;
    CTcpServerCallback cb;
    CReaperEngine e;
    e.addTcpServer(CSockAddr("127.0.0.1", 12345), &cb);
    CReaperParams params;
    params.maxFdNum_ = 10000;
    e.run(params);
    return 0;
}
