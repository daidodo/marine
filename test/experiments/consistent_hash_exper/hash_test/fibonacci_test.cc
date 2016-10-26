#include "comm.h"

static inline uint32_t Hash32bit_Simple(uint32_t key)
{
    return key *= 2654435769;
}

#include "main.h"
