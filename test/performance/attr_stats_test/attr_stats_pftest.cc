#include <marine/attr_stats.hh>

#include "../pf_inc.h"

#define NAME AttrMarine

PFINIT()
{
    ATTR_INIT();
    for(int i = 0;i < 900;++i)
        ATTR_ADD_SLOW(i, 1);
}

PFTEST(NAME, add)
{
    for(int i = 0;i < 10000;++i)
        ATTR_ADD(12345, 1);
}
