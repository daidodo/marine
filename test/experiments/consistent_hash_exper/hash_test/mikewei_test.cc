#include "comm.h"

static inline uint32_t Hash32bit_Simple(uint32_t dwVal)
{
    // borrowed from stackoverflow.com
    dwVal = ((dwVal >> 16) ^ dwVal) * 0x45d9f3b;
    dwVal = ((dwVal >> 16) ^ dwVal) * 0x45d9f3b;
    dwVal = ((dwVal >> 16) ^ dwVal);
    return dwVal;
}

#include "main.h"
