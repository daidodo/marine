#include <marine/consistent_hash.hh>

#include "../pf_inc.h"

typedef CConsistentHash<uint32_t>   __Hash;

#include "comm.h"

PFINIT_T(CConsistentHash, hash)
{
    for(int i = 0;i < kMaxKey;++i)
        h.setValue(i, i + 1);
    key = 0;
}

PFTEST(CConsistentHash, hash)
{
    key += h.hash(key);
    key += h.hash(key);
    key += h.hash(key);
    key += h.hash(key);
    key += h.hash(key);
    key += h.hash(key);
    key += h.hash(key);
    key += h.hash(key);
    key += h.hash(key);
    key += h.hash(key);
}

PFINIT_T(CConsistentHash, reload)
{
    for(int i = 0;i < kMaxKey;++i)
        h.setValue(i, kMaxKey - i);
    key2 = 0;
}

PFTEST(CConsistentHash, reload)
{
    if(key2 >= 0){
        h.setValue(kMaxKey - key2, 1);
    }else{
        h.setValue(-1 - key2, kMaxKey + 1 - key2);
    }
    ++key2;
}
