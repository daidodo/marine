#include "comm.h"

static inline uint32_t Hash32bit_Simple(uint32_t h)
{
    h ^= h >> 16;
    h *= 0x85ebca6b;
    ++h;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    ++h;
    h ^= h >> 16;
    return h;
}

#include "main.h"
