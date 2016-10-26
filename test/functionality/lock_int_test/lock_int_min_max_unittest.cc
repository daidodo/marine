#include <marine/lock_int.hh>

#define TYPE    CLockIntMinMax
#define FNAME   "min_max.h"

#define VAL     char
#define VALN    char
#include FNAME
#undef VAL
#undef VALN

#define VAL     signed char
#define VALN    signed_char
#include FNAME
#undef VAL
#undef VALN

#define VAL     unsigned char
#define VALN    unsigned_char
#include FNAME
#undef VAL
#undef VALN

#define VAL     short
#define VALN    short
#include FNAME
#undef VAL
#undef VALN

#define VAL     unsigned short
#define VALN    unsigned_short
#include FNAME
#undef VAL
#undef VALN

#define VAL     int
#define VALN    int
#include FNAME
#undef VAL
#undef VALN

#define VAL     unsigned int
#define VALN    unsigned_int
#include FNAME
#undef VAL
#undef VALN

#define VAL     long
#define VALN    long
#include FNAME
#undef VAL
#undef VALN

#define VAL     unsigned long
#define VALN    unsigned_long
#include FNAME
#undef VAL
#undef VALN

#define VAL     long long
#define VALN    long_long
#include FNAME
#undef VAL
#undef VALN

#define VAL     unsigned long long
#define VALN    unsigned_long_long
#include FNAME
#undef VAL
#undef VALN
