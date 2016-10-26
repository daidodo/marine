#include <marine/freq_control.hh>

#include "../pf_inc.h"

CFreqControl fc(1000, 1000);

PFTEST(CFreqControl, generate)
{
    fc.generate();
}

PFTEST(CFreqControl, get)
{
    fc.get();
}
