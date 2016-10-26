#ifndef DOZERG_SAVE_STACK_IMPL_H_20130730
#define DOZERG_SAVE_STACK_IMPL_H_20130730

#include "environment.hh"

NS_IMPL_BEGIN

struct CAddrStack
{
    void * addr_[511];
    size_t top_;
};

bool __getStack(CAddrStack *) __attribute__ ((no_instrument_function,deprecated));

NS_IMPL_END

#endif

