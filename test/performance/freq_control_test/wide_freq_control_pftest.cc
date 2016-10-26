#include <marine/freq_control.hh>

#include "../pf_inc.h"

CWideFreqControl fc(1000);

PFTEST(CWideFreqControl, generate)
{
    fc.get();
}

PFTEST(CWideFreqControl, get)
{
    fc.get();
}
