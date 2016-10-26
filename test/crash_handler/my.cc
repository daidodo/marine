#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <marine/file.hh>
#include "my.h"

using namespace marine;

void SegvTest(int * p)
{
    *p = 100;
}

static int consumeStack()
{
    long var[2 << 17] = {1};
    int i = 0;
    for(;i < int(sizeof var / sizeof(long));++i)
        var[i] = i;
    for(i = 0;i < int(sizeof var / sizeof(long));++i)
        var[i] = 'a' + consumeStack();
    return i;
}

void StackOverflowTest()
{
    consumeStack();
}

const char * s1 = "skdddddddddddddddddddddddddddddddddddddddddsjdkgjksdjgksdjgkdsgjskdgskdjfksjfdksdfskdfj3325442252455";

static size_t realWriteStack()
{
    char s[10];
    int i = 1;
    while(i < 100)
        i += i;
    memcpy(s, s1, i);
    size_t len = ::strlen(s);
    return len;
}

void StackOverWrite()
{
    int x = 0;
    x += realWriteStack();
    x += realWriteStack();
    x += realWriteStack();
    for(int i = 0;i < 100;++i)
        x += i;
    x &= 1111;
}

void DoubleFree()
{
    char * p = static_cast<char *>(::malloc(100));
    if(NULL == p)
        return;
    ::memcpy(p, s1, 100);
    for(;;++p)
        ::free(p);
}

void IllTest()
{
    typedef void (*Func)(void);
    const static unsigned char insn[4] = {0xff, 0xff, 0xff, 0xff};
    Func function = reinterpret_cast<Func>(insn);
    function();
}

void BusError()
{
    const size_t kSize = 8192;
    CFile f("tmp.log", O_RDWR | O_CREAT, 0666);
    if(!f.valid())
        return;
    if(!f.truncate(kSize))
        return;
    char * p = (char *)mmap(NULL, kSize, PROT_READ | PROT_WRITE, MAP_SHARED, f.fd(), 0);
    char tmp = 0;
    for(size_t i = 0;i < kSize;++i)
        tmp += p[i];

    f.truncate(0);
    for(size_t i = 0;i < kSize;++i)
        p[i] = tmp;
}

int Divide0Test()
{
    int y = 100, s = 0;
    for(int i = 10;i > -1;--i)
        s += y / i;
    return s;
}

