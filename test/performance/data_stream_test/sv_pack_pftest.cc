#include "sv_pack.h"

#include "comm.h"

PFINIT_T(SvPack, encode)
{
    initT();
}

PFTEST(SvPack, encode)
{
    void * cur = buf;
    uint32_t left = sizeof buf;
    int ret;
    ret = C2_AddByte(&cur, &left, t.a);
    PFASSERT(0 == ret);
    ret = C2_AddByte(&cur, &left, t.b);
    PFASSERT(0 == ret);
    ret = C2_AddByte(&cur, &left, t.c);
    PFASSERT(0 == ret);
    ret = C2_AddWord(&cur, &left, t.d);
    PFASSERT(0 == ret);
    ret = C2_AddWord(&cur, &left, t.e);
    PFASSERT(0 == ret);
    ret = C2_AddDWord(&cur, &left, t.f);
    PFASSERT(0 == ret);
    ret = C2_AddDWord(&cur, &left, t.g);
    PFASSERT(0 == ret);
#if (32 == __WORDSIZE)
    ret = C2_AddDWord(&cur, &left, t.h);
    PFASSERT(0 == ret);
    ret = C2_AddDWord(&cur, &left, t.i);
    PFASSERT(0 == ret);
#else
    ret = C2_AddQWord(&cur, &left, t.h);
    PFASSERT(0 == ret);
    ret = C2_AddQWord(&cur, &left, t.i);
    PFASSERT(0 == ret);
#endif
    ret = C2_AddQWord(&cur, &left, t.j);
    PFASSERT(0 == ret);
    ret = C2_AddQWord(&cur, &left, t.k);
    PFASSERT(0 == ret);
    if(sizeof(wchar_t) == 2){
        ret = C2_AddWord(&cur, &left, t.l);
        PFASSERT(0 == ret);
    }else{
        ret = C2_AddDWord(&cur, &left, t.l);
        PFASSERT(0 == ret);
    }
}

PFINIT_T(SvPack, decode)
{
    memset(buf, ' ', sizeof buf);
}

PFTEST(SvPack, decode)
{
    const void * cur = buf;
    uint32_t left = sizeof buf;
    int ret;
    uint16_t u16;
    uint32_t u32;
    uint64_t u64;
    ret = C2_GetByte(&cur, &left, (uint8_t *)&t.a);
    PFASSERT(0 == ret);
    ret = C2_GetByte(&cur, &left, (uint8_t *)&t.b);
    PFASSERT(0 == ret);
    ret = C2_GetByte(&cur, &left, (uint8_t *)&t.c);
    PFASSERT(0 == ret);
    ret = C2_GetWord(&cur, &left, (uint16_t *)&t.d);
    PFASSERT(0 == ret);
    ret = C2_GetWord(&cur, &left, (uint16_t *)&t.e);
    PFASSERT(0 == ret);
    ret = C2_GetDWord(&cur, &left, (uint32_t *)&t.f);
    PFASSERT(0 == ret);
    ret = C2_GetDWord(&cur, &left, (uint32_t *)&t.g);
    PFASSERT(0 == ret);
#if (32 == __WORDSIZE)
    ret = C2_GetDWord(&cur, &left, (uint32_t *)&t.h);
    PFASSERT(0 == ret);
    ret = C2_GetDWord(&cur, &left, (uint32_t *)&t.i);
    PFASSERT(0 == ret);
#else
    ret = C2_GetQWord(&cur, &left, (uint64_t *)&t.h);
    PFASSERT(0 == ret);
    ret = C2_GetQWord(&cur, &left, (uint64_t *)&t.i);
    PFASSERT(0 == ret);
#endif
    ret = C2_GetQWord(&cur, &left, &u64);
    PFASSERT(0 == ret);
    t.j = u64;
    ret = C2_GetQWord(&cur, &left, &u64);
    PFASSERT(0 == ret);
    t.k = u64;
    if(sizeof(wchar_t) == 2){
        ret = C2_GetWord(&cur, &left, &u16);
        PFASSERT(0 == ret);
        t.l = u16;
    }else{
        ret = C2_GetDWord(&cur, &left, &u32);
        PFASSERT(0 == ret);
        t.l = u32;
    }
}
