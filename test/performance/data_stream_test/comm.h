#include "../pf_inc.h"

struct CTest
{
    char a;
    signed char b;
    unsigned char c;
    short d;
    unsigned short e;
    int f;
    unsigned int g;
    long h;
    unsigned long i;
    long long j;
    unsigned long long k;
    wchar_t l;
};

char buf[1024];
CTest t;
int g_v = 0;

void initT()
{
    t.a = 1;
    t.b = 2;
    t.c = 3;
    t.d = 4;
    t.e = 5;
    t.f = 6;
    t.g = 7;
    t.h = 8;
    t.i = 9;
    t.j = 10;
    t.k = 11;
    t.l = 12;
}
