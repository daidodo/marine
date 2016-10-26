#include "sv_consistent_hash.h"

#include "../pf_inc.h"

typedef ConsistentHashRing __Hash;

#include "comm.h"

std::vector<NodeInfo> nodes(kMaxKey);

PFINIT_T(ConsistentHashRing, hash)
{
    for(int i = 0;i < kMaxKey;++i){
        nodes[i].dwNodeId = i;
        nodes[i].dwWeight = i + 1;
    }
    ConsistentHashInit(&h);
    ConsistentHashReload(&h, &nodes[0], nodes.size());
    key = 0;
}

PFUNINIT_T(ConsistentHashRing, hash)
{
    ConsistentHashFree(&h);
}

PFTEST(ConsistentHashRing, hash)
{
    uint32_t v = 0;
    ConsistentHashMap(&h, key, &v);
    key += v;
    ConsistentHashMap(&h, key, &v);
    key += v;
    ConsistentHashMap(&h, key, &v);
    key += v;
    ConsistentHashMap(&h, key, &v);
    key += v;
    ConsistentHashMap(&h, key, &v);
    key += v;
    ConsistentHashMap(&h, key, &v);
    key += v;
    ConsistentHashMap(&h, key, &v);
    key += v;
    ConsistentHashMap(&h, key, &v);
    key += v;
    ConsistentHashMap(&h, key, &v);
    key += v;
    ConsistentHashMap(&h, key, &v);
    key += v;
}

PFINIT_T(ConsistentHashRing, reload)
{
    for(int i = 0;i < kMaxKey;++i){
        nodes[i].dwNodeId = i;
        nodes[i].dwWeight = kMaxKey - i;
    }
    ConsistentHashInit(&h);
    ConsistentHashReload(&h, &nodes[0], nodes.size());
    key2 = 0;
}

PFUNINIT_T(ConsistentHashRing, reload)
{
    ConsistentHashFree(&h);
}

PFTEST(ConsistentHashRing, reload)
{
    if(key2 >= 0){
        nodes[kMaxKey - key2].dwWeight = 1;
    }else{
        nodes[-1 - key2].dwWeight = kMaxKey + 1 - key2;
    }
    ConsistentHashReload(&h, &nodes[0], nodes.size());
    ++key2;
}
