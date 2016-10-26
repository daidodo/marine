#include "../pf_inc.h"
#include "comm.h"

static inline uint32_t u64Crc(uint32_t init, const char * buf, size_t sz)
{
    typedef uint32_t Int;
    const Int kSign = (Int(1) << (CTypeTraits<Int>::kMaxBits - 1));
    const char * const s = reinterpret_cast<const char *>(reinterpret_cast<uintptr_t>(buf) & ~uintptr_t(7));
    const char * e = reinterpret_cast<const char *>(reinterpret_cast<uintptr_t>(buf + sz) & ~uintptr_t(7));
    const char * const ee = buf + sz;
    if(buf < s){
        uint64_t v = 0;
        do{
            v = (v << 8) + (*buf++ & 0xFF);
        }while(buf < s);
        init = (init << 1) + (init & kSign ? 1 : 0) + v;
    }
    for(const uint64_t *i = (const uint64_t *)s, *j = (const uint64_t *)e;i < j;)
        init = (init << 1) + (init & kSign ? 1 : 0) + *i++;
    if(e < ee){
        uint64_t v = 0;
        do{
            v = (v << 8) + (*e++ & 0xFF);
        }while(e < ee);
        init = (init << 1) + (init & kSign ? 1 : 0) + v;
    }
    return init;
}

PFTEST(Crc, all)
{
    uint32_t r = 0;
    for(int i = 0;i < 100;++i){
        r += u64Crc(0, &buf[0], buf.size());
        ++buf[0];
    }
}
