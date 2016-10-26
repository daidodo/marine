#include "test.pb.h"
#include <google/protobuf/io/coded_stream.h>
#include "comm.h"

PbTest tt;
size_t sz;

PFINIT()
{
    initT();
    tt.set_a(t.a);
    tt.set_b(t.b);
    tt.set_c(t.c);
    tt.set_d(t.d);
    tt.set_e(t.e);
    tt.set_f(t.f);
    tt.set_g(t.g);
    tt.set_h(t.h);
    tt.set_i(t.i);
    tt.set_j(t.j);
    tt.set_k(t.k);
    tt.set_l(t.l);
}

PFINIT_T(Protobuf, encode)
{
    std::string str;
    tt.SerializeToString(&str);
    cout<<"encode size = "<<str.size()<<", ";
}

PFTEST(Protobuf, encode)
{
    bool ret = tt.SerializeToArray(buf, sizeof buf);
    PFASSERT(ret);
}

PFINIT_T(Protobuf, decode)
{
    tt.SerializeToArray(buf, sizeof buf);
    sz = tt.GetCachedSize();
}

PFTEST(Protobuf, decode)
{
    PbTest t1;
    bool ret = t1.ParseFromArray(buf, sz);
    PFASSERT(ret);
}

PFTEST(Protobuf, varint_encode)
{
    uint8_t * p = (uint8_t *)buf;
    for(uint32_t i = 0;i <= 2000;i += 10)
        p = google::protobuf::io::CodedOutputStream::WriteVarint32ToArray(i, p);
}

PFTEST(Protobuf, varint_encode1b)
{
    uint8_t * p = (uint8_t *)buf;
    for(uint32_t i = 0;i <= 127;i++)
        p = google::protobuf::io::CodedOutputStream::WriteVarint32ToArray(i, p);
}

PFTEST(Protobuf, varint_encode2b)
{
    uint8_t * p = (uint8_t *)buf;
    for(uint32_t i = 128;i <= 500;i++)
        p = google::protobuf::io::CodedOutputStream::WriteVarint32ToArray(i, p);
}

PFINIT_T(Protobuf, varint_decode)
{
    uint8_t * p = (uint8_t *)buf;
    for(uint32_t i = 0;i <= 2000;i += 10)
        p = google::protobuf::io::CodedOutputStream::WriteVarint32ToArray(i, p);
}

PFTEST(Protobuf, varint_decode)
{
    google::protobuf::io::CodedInputStream in((const uint8_t *)buf, sizeof buf);
    uint32_t v = 0;
    for(uint32_t i = 0;i <= 2000;i += 10){
        in.ReadVarint32(&v);
        g_v += v;
    }
}

PFINIT_T(Protobuf, varint_decode1b)
{
    uint8_t * p = (uint8_t *)buf;
    for(uint32_t i = 0;i <= 127;i++)
        p = google::protobuf::io::CodedOutputStream::WriteVarint32ToArray(i, p);
}

PFTEST(Protobuf, varint_decode1b)
{
    google::protobuf::io::CodedInputStream in((const uint8_t *)buf, sizeof buf);
    uint32_t v = 0;
    for(uint32_t i = 0;i <= 127;i++){
        in.ReadVarint32(&v);
        g_v += v;
    }
}

PFINIT_T(Protobuf, varint_decode2b)
{
    uint8_t * p = (uint8_t *)buf;
    for(uint32_t i = 128;i <= 500;i++)
        p = google::protobuf::io::CodedOutputStream::WriteVarint32ToArray(i, p);
}

PFTEST(Protobuf, varint_decode2b)
{
    google::protobuf::io::CodedInputStream in((const uint8_t *)buf, sizeof buf);
    uint32_t v = 0;
    for(uint32_t i = 128;i <= 500;i++){
        in.ReadVarint32(&v);
        g_v += v;
    }
}
