#include "sv_freq_ctrl.h"

#include "../pf_inc.h"

TokenBucket tb;

PFINIT()
{
    TokenBucket_Init(&tb, 1000, 1000);
}

PFTEST(TokenBucket, generate)
{
    TokenBucket_Gen(&tb, NULL);
}

PFTEST(TokenBucket, get)
{
    TokenBucket_Get(&tb, 1);
}
