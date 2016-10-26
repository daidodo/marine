#include <marine/tools/other.hh>

#include "../pf_inc.h"
#include "comm.h"

PFTEST(Crc, all)
{
    uint32_t r = 0;
    for(int i = 0;i < 100;++i){
        r += tools::Crc<uint32_t>(0, &buf[0], buf.size());
        ++buf[0];
    }
}
