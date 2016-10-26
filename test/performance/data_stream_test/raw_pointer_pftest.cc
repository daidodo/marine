#include <arpa/inet.h>
#include <marine/tools/net.hh>

#include "comm.h"

PFINIT_T(Raw, encode)
{
    initT();
}

PFTEST(Raw, encode)
{
    char * cur = buf;
    size_t left = sizeof buf;
    //a
    PFASSERT(left >= sizeof t.a);
    *cur = t.a;
    cur += sizeof t.a;
    left -= sizeof t.a;
    //b
    PFASSERT(left >= sizeof t.b);
    *cur = t.b;
    cur += sizeof t.b;
    left -= sizeof t.b;
    //c
    PFASSERT(left >= sizeof t.c);
    *cur = t.c;
    cur += sizeof t.c;
    left -= sizeof t.c;
    //d
    PFASSERT(left >= sizeof t.d);
    *(short *)cur = htons(t.d);
    cur += sizeof t.d;
    left -= sizeof t.d;
    //e
    PFASSERT(left >= sizeof t.e);
    *(unsigned short *)cur = htons(t.e);
    cur += sizeof t.e;
    left -= sizeof t.e;
    //f
    PFASSERT(left >= sizeof t.f);
    *(int *)cur = htonl(t.f);
    cur += sizeof t.f;
    left -= sizeof t.f;
    //g
    PFASSERT(left >= sizeof t.g);
    *(unsigned int *)cur = htonl(t.g);
    cur += sizeof t.g;
    left -= sizeof t.g;
    //h
    PFASSERT(left >= sizeof t.h);
    *(long *)cur = tools::SwapByteOrder(t.h);
    cur += sizeof t.h;
    left -= sizeof t.h;
    //i
    PFASSERT(left >= sizeof t.i);
    *(unsigned long *)cur = tools::SwapByteOrder(t.i);
    cur += sizeof t.i;
    left -= sizeof t.i;
    //j
    PFASSERT(left >= sizeof t.j);
    *(long long *)cur = tools::SwapByteOrder(t.j);
    cur += sizeof t.j;
    left -= sizeof t.j;
    //k
    PFASSERT(left >= sizeof t.k);
    *(unsigned long long *)cur = tools::SwapByteOrder(t.k);
    cur += sizeof t.k;
    left -= sizeof t.k;
    //l
    PFASSERT(left >= sizeof t.l);
    *(wchar_t *)cur = tools::SwapByteOrder(t.l);
    cur += sizeof t.l;
    left -= sizeof t.l;
}

PFINIT_T(Raw, decode)
{
    memset(buf, ' ', sizeof buf);
}

PFTEST(Raw, decode)
{
    const char * cur = buf;
    size_t left = sizeof buf;
    //a
    PFASSERT(left >= sizeof t.a);
    t.a = *cur;
    cur += sizeof t.a;
    left -= sizeof t.a;
    //b
    PFASSERT(left >= sizeof t.b);
    t.b = *cur;
    cur += sizeof t.b;
    left -= sizeof t.b;
    //c
    PFASSERT(left >= sizeof t.c);
    t.c = *cur;
    cur += sizeof t.c;
    left -= sizeof t.c;
    //d
    PFASSERT(left >= sizeof t.d);
    t.d = htons(*(const short *)cur);
    cur += sizeof t.d;
    left -= sizeof t.d;
    //e
    PFASSERT(left >= sizeof t.e);
    t.e = htons(*(const unsigned short *)cur);
    cur += sizeof t.e;
    left -= sizeof t.e;
    //f
    PFASSERT(left >= sizeof t.f);
    t.f = htonl(*(const int *)cur);
    cur += sizeof t.f;
    left -= sizeof t.f;
    //g
    PFASSERT(left >= sizeof t.g);
    t.g = htonl(*(const unsigned int *)cur);
    cur += sizeof t.g;
    left -= sizeof t.g;
    //h
    PFASSERT(left >= sizeof t.h);
    t.h = tools::SwapByteOrder(*(const long *)cur);
    cur += sizeof t.h;
    left -= sizeof t.h;
    //i
    PFASSERT(left >= sizeof t.i);
    t.i = tools::SwapByteOrder(*(const unsigned long *)cur);
    cur += sizeof t.i;
    left -= sizeof t.i;
    //j
    PFASSERT(left >= sizeof t.j);
    t.j = tools::SwapByteOrder(*(const long long *)cur);
    cur += sizeof t.j;
    left -= sizeof t.j;
    //k
    PFASSERT(left >= sizeof t.k);
    t.k = tools::SwapByteOrder(*(const unsigned long long *)cur);
    cur += sizeof t.k;
    left -= sizeof t.k;
    //l
    PFASSERT(left >= sizeof t.l);
    t.l = tools::SwapByteOrder(*(const wchar_t *)cur);
    cur += sizeof t.l;
    left -= sizeof t.l;
}
