#ifndef DOZERG_TO_STRING_IMPL_HH_20130412
#define DOZERG_TO_STRING_IMPL_HH_20130412

#include "environment.hh"

NS_IMPL_BEGIN

struct CToStringBase
{
    int base_;
    explicit CToStringBase(int b):base_(b){}
};

struct CToStringWidth
{
    int w_;
    explicit CToStringWidth(int b):w_(b){}
};

struct CToStringFill
{
    char fill_;
    explicit CToStringFill(char b):fill_(b){}
};

NS_IMPL_END

#endif

