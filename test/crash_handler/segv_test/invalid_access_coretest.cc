#define THREAD_COUNT    10

#include "../c_inc.h"

static void Test()
{
    int * p = (int *)1;
    SegvTest(p);    //segment fault
}

