#include <gtest/gtest.h>

using namespace std;
using namespace marine;

//extends for gtest
#define __ADD_GTEST_ENV(env, id)    \
    struct CInitFor##env##_##id{    \
        CInitFor##env##_##id(){::testing::AddGlobalTestEnvironment(new env);}   \
    }g_init##env##_##id
