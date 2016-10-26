#include "comm.h"

static inline uint32_t Hash32bit_Simple(uint32_t dwVal)
{
    dwVal = ((dwVal >> 16) ^ dwVal) * 0x45d9f3b + 1;
    dwVal = ((dwVal >> 16) ^ dwVal) * 0x45d9f3b + 3;
    dwVal = ((dwVal >> 16) ^ dwVal);
    return dwVal;
}

#include "main.h"
