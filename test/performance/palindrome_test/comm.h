#include "../pf_inc.h"

int g_count;
std::string g_str;

PFINIT_T(test, test)
{
    g_str.resize(2048, 'a');
}

