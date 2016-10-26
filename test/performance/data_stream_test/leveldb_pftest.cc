#include "../pf_inc.h"
#include "comm.h"

//from google leveldb source (leveldb-1.14.0/util/coding.h|cc)
static inline char* EncodeVarint32(char* dst, uint32_t v) {
    // Operate on characters as unsigneds
    unsigned char* ptr = reinterpret_cast<unsigned char*>(dst);
    static const int B = 128;
    if (v < (1<<7)) {
        *(ptr++) = v;
    } else if (v < (1<<14)) {
        *(ptr++) = v | B;
        *(ptr++) = v>>7;
    } else if (v < (1<<21)) {
        *(ptr++) = v | B;
        *(ptr++) = (v>>7) | B;
        *(ptr++) = v>>14;
    } else if (v < (1<<28)) {
        *(ptr++) = v | B;
        *(ptr++) = (v>>7) | B;
        *(ptr++) = (v>>14) | B;
        *(ptr++) = v>>21;
    } else {
        *(ptr++) = v | B;
        *(ptr++) = (v>>7) | B;
        *(ptr++) = (v>>14) | B;
        *(ptr++) = (v>>21) | B;
        *(ptr++) = v>>28;
    }
    return reinterpret_cast<char*>(ptr);
}

static inline const char* GetVarint32PtrFallback(const char* p,
                                           const char* limit,
                                           uint32_t* value) {
    uint32_t result = 0;
    for (uint32_t shift = 0; shift <= 28 && p < limit; shift += 7) {
        uint32_t byte = *(reinterpret_cast<const unsigned char*>(p));
        p++;
        if (byte & 128) {
            // More bytes are present
            result |= ((byte & 127) << shift);
        } else {
            result |= (byte << shift);
            *value = result;
            return reinterpret_cast<const char*>(p);
        }
    }
    return NULL;
}

static inline const char* GetVarint32Ptr(const char* p,
                                          const char* limit,
                                          uint32_t* value) {
    if (p < limit) {
        uint32_t result = *(reinterpret_cast<const unsigned char*>(p));
        if ((result & 128) == 0) {
            *value = result;
            return p + 1;
        }
    }
    return GetVarint32PtrFallback(p, limit, value);
}

PFTEST(LevelDB, varint_encode)
{
    char * p = buf;
    for(uint32_t i = 0;i <= 2000;i += 10)
        p = EncodeVarint32(p, i);
}

PFTEST(LevelDB, varint_encode1b)
{
    char * p = buf;
    for(uint32_t i = 0;i <= 127;i++)
        p = EncodeVarint32(p, i);
}

PFTEST(LevelDB, varint_encode2b)
{
    char * p = buf;
    for(uint32_t i = 128;i <= 500;i++)
        p = EncodeVarint32(p, i);
}

PFINIT_T(LevelDB, varint_decode)
{
    char * p = buf;
    for(uint32_t i = 0;i <= 2000;i += 10)
        p = EncodeVarint32(p, i);
}

PFTEST(LevelDB, varint_decode)
{
    const char * p = buf;
    const char * e = buf + sizeof buf;
    uint32_t v = 0;
    for(uint32_t i = 0;i <= 2000;i += 10){
        p = GetVarint32Ptr(p, e, &v);
        g_v += v;
    }
}

PFINIT_T(LevelDB, varint_decode1b)
{
    char * p = buf;
    for(uint32_t i = 0;i <= 127;i++)
        p = EncodeVarint32(p, i);
}

PFTEST(LevelDB, varint_decode1b)
{
    const char * p = buf;
    const char * e = buf + sizeof buf;
    uint32_t v = 0;
    for(uint32_t i = 0;i <= 127;i++){
        p = GetVarint32Ptr(p, e, &v);
        g_v += v;
    }
}

PFINIT_T(LevelDB, varint_decode2b)
{
    char * p = buf;
    for(uint32_t i = 128;i <= 500;i++)
        p = EncodeVarint32(p, i);
}

PFTEST(LevelDB, varint_decode2b)
{
    const char * p = buf;
    const char * e = buf + sizeof buf;
    uint32_t v = 0;
    for(uint32_t i = 128;i <= 500;i++){
        p = GetVarint32Ptr(p, e, &v);
        g_v += v;
    }
}
