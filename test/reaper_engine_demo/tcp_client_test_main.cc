#include <data_stream.hh>
#include <server_callback.hh>
#include <engine.hh>
#include "cmd.hh"
#include "comm.h"

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
    int scnt_;
    int rcnt_;
public:
    CTcpClientCallback():scnt_(0), rcnt_(0){}
    bool sendPkg(__Socket & socket, __SendList & sendList){
        std::string str;
        if(rcnt_ < 3){
            if(scnt_ < 3){
                if(pkg_.empty())
                    pkg_ = pkgBuf();
                if(!scnt_)
                    str = pkg_;
                else
                    str = pkg_.substr(10);
                str.append(pkg_.substr(0, 10));
                INFO("send buf="<<tools::Dump(str)<<" to socket="<<socket.toString());
                sendList.push_back(__SendElem(str, __SockAddr()));
                ++scnt_;
            }
            return true;
        }else if(rcnt_ < 10){
            if(scnt_ < 10){
                if(scnt_ == 3)
                    str = pkg_.substr(10);
                else
                    str = pkg_;
                str.append(pkg_);
                str.append(pkg_);
                str.append(pkg_);
                INFO("send buf="<<tools::Dump(str)<<" to socket="<<socket.toString());
                sendList.push_back(__SendElem(str, __SockAddr()));
                scnt_ += 4;
            }
            return true;
        }
        return false;
    }
    virtual bool onConnect(__Socket & socket, __SendList & sendList){
        DEBUG("finish connect() for sock="<<socket.toString());
        return sendPkg(socket, sendList);
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
        //decode
        CInByteStream ds(buf, sz);
        uint32_t len = 0;
        ds>>len;
        if(len > 1000)
            return -1;
        if(len > ds.left()){
            ERROR("format error len="<<len<<" in buf="<<tools::Dump(buf, sz)<<" from "<<from.toString()<<" for socket="<<socket.toString());
            return false;
        }
        CRespCmd resp;
        if(!(ds>>resp)){
            ERROR("format error CRespCmd in buf="<<tools::Dump(buf, sz)<<" from "<<from.toString()<<" for socket="<<socket.toString());
            return false;
        }
        INFO("recv resp="<<resp.toString()<<" from "<<from.toString()<<" for socket="<<socket.toString());
        ++rcnt_;
        //send req again
        //return sendPkg(socket, sendList);
        return false;
    }
    virtual void onSend(__Socket & socket, const char * buf, size_t sz, const __SockAddr & to){
        DEBUG("send to "<<to.toString()<<", buf="<<tools::Dump(buf, sz)<<" for socket="<<socket.toString());
    }
    virtual bool onClose(__Socket & socket){
        DEBUG("close socket="<<socket.toString());
        return false;
    }
    virtual std::string toString() const{return "CTcpClientCallback";}
};

int main(int argc, const char ** argv)
{
    if(!init(argv[0]))
        return 1;
    CTcpClientCallback cb;
    CReaperEngine e;
    e.addTcpClient(CSockAddr("127.0.0.1", 12345), &cb);
    e.run();
    return 0;
}
