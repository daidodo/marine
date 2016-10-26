#include "crc32c_leveldb.h"

#include "../pf_inc.h"
#include "comm.h"

PFTEST(Crc, all)
{
    uint32_t r = 0;
    for(int i = 0;i < 100;++i){
        r += leveldb::crc32c::Extend(0, &buf[0], buf.size());
        ++buf[0];
    }
}
