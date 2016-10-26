#include <marine/data_stream.hh>

#include "comm.h"

PFINIT_T(DataStream, encode)
{
    initT();
    COutByteStreamBuf out(buf, sizeof buf);
    out<<t.a
        <<t.b
        <<t.c
        <<t.d
        <<t.e
        <<t.f
        <<t.g
        <<t.h
        <<t.i
        <<t.j
        <<t.k
        <<t.l
        ;
    std::cout<<"encode size = "<<out.size()<<", ";
}

PFTEST(DataStream, encode)
{
    COutByteStreamBuf out(buf, sizeof buf);
    out<<t.a
        <<t.b
        <<t.c
        <<t.d
        <<t.e
        <<t.f
        <<t.g
        <<t.h
        <<t.i
        <<t.j
        <<t.k
        <<t.l
        ;
    PFASSERT(out);
}

PFINIT_T(DataStream, decode)
{
    memset(buf, ' ', sizeof buf);
}

PFTEST(DataStream, decode)
{
    CInByteStream in(buf, sizeof buf);
    in>>t.a
        >>t.b
        >>t.c
        >>t.d
        >>t.e
        >>t.f
        >>t.g
        >>t.h
        >>t.i
        >>t.j
        >>t.k
        >>t.l
        ;
    PFASSERT(in);
}

PFTEST(DataStream, varint_encode)
{
    COutByteStreamBuf out(buf, sizeof buf);
    for(uint32_t i = 0;i <= 2000;i += 10)
        out<<Manip::varint(i);
}

PFTEST(DataStream, varint_encode1b)
{
    COutByteStreamBuf out(buf, sizeof buf);
    for(uint32_t i = 0;i <= 127;i++)
        out<<Manip::varint(i);
}

PFTEST(DataStream, varint_encode2b)
{
    COutByteStreamBuf out(buf, sizeof buf);
    for(uint32_t i = 128;i <= 500;i++)
        out<<Manip::varint(i);
}

PFINIT_T(DataStream, varint_decode)
{
    COutByteStreamBuf out(buf, sizeof buf);
    for(uint32_t i = 0;i <= 2000;i += 10)
        out<<Manip::varint(i);
}

PFTEST(DataStream, varint_decode)
{
    CInByteStream in(buf, sizeof buf);
    uint32_t v = 0;
    for(uint32_t i = 0;i <= 2000;i += 10){
        in>>Manip::varint(v);
        g_v += v;
    }
}

PFINIT_T(DataStream, varint_decode1b)
{
    COutByteStreamBuf out(buf, sizeof buf);
    for(uint32_t i = 0;i <= 127;i++)
        out<<Manip::varint(i);
}

PFTEST(DataStream, varint_decode1b)
{
    CInByteStream in(buf, sizeof buf);
    uint32_t v = 0;
    for(uint32_t i = 0;i <= 127;i++){
        in>>Manip::varint(v);
        g_v += v;
    }
}

PFINIT_T(DataStream, varint_decode2b)
{
    COutByteStreamBuf out(buf, sizeof buf);
    for(uint32_t i = 128;i <= 500;i++)
        out<<Manip::varint(i);
}

PFTEST(DataStream, varint_decode2b)
{
    CInByteStream in(buf, sizeof buf);
    uint32_t v = 0;
    for(uint32_t i = 128;i <= 500;i++){
        in>>Manip::varint(v);
        g_v += v;
    }
}
