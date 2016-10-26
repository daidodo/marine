#include "crc32_brumme.h"

#include "../pf_inc.h"
#include "comm.h"

PFTEST(Crc, all)
{
    uint32_t r = 0;
    for(int i = 0;i < 100;++i){
        r += crc32_halfbyte(&buf[0], buf.size(), 0);
        ++buf[0];
    }
}
