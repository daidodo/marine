#include "../pf_inc.h"

PFTEST(printf, out)
{
    typedef unsigned long long __Int;
    __Int v = -1;
    char buf[64];
    for(int i = 0;i < 1000;++i){
        snprintf(buf, sizeof buf, "%llu", (v - i));
    }
}
