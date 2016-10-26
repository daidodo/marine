#ifndef _SV_MACRO_H
#define _SV_MACRO_H

#include <stddef.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#if __GNUC__ < 4
#   define __builtin_offsetof   offsetof
#endif

#ifndef DIM
#define DIM(x) (sizeof(x) / sizeof((x)[0]))
#endif

#ifndef MemberToStruct
#define MemberToStruct(MemberPtr, StructType, MemberName) \
	((StructType *) ((char *) MemberPtr - offsetof(StructType, MemberName)))
#endif

#ifndef CAT_TOKEN
#define CAT_TOKEN_1(t1,t2) t1##t2
#define CAT_TOKEN(t1,t2) CAT_TOKEN_1(t1,t2)
#endif

#ifndef COMPILE_ASSERT
#define COMPILE_ASSERT(x)  \
					enum {CAT_TOKEN (comp_assert_at_line_, __LINE__) = 1 / !!(x) };
#endif

#ifndef CHECK_SIZE
#define CHECK_SIZE(type, size) extern int sizeof_##type##_is_##size [!!(sizeof(type)==(size_t)size) - 1]
#endif//CHECK_SIZE

#ifndef CHECK_OFFSET
#define CHECK_OFFSET(type, member, value) \
	    extern int offset_of_##member##_in_##type##_is_##value \
	[!!(__builtin_offsetof(type,member)==((size_t)(value))) - 1]
#endif//CHECK_OFFSET

#define STRING(X) #X
#define STRVAL(X) STRING(X)

#define CHECK_RET(expr)	\
	do{	\
		int iRet = (expr);	\
		if(0 != iRet)	\
			return iRet;	\
	}while(0)

#define CHECK_RET1(expr, ret)	\
	do{	\
		int iRet = (expr);	\
		if(0 != iRet)	\
			return ret;	\
	}while(0)


#ifndef FIELD_SIZEOF
#define FIELD_SIZEOF(t, f) (sizeof(((t*)0)->f))
#endif


//dwVal in [dwMin, dwMax]
#define RANGE32_IN(dwVal, dwMin, dwMax)   ((uint32_t)dwMin <= (uint32_t)dwVal && (uint32_t)dwVal <= (uint32_t)dwMax)

//dwVal out [dwMin, dwMax]
#define RANGE32_OUT(dwVal, dwMin, dwMax)  ((uint32_t)dwMin > (uint32_t)dwVal || (uint32_t)dwVal > (uint32_t)dwMax)

//wVal in [wMin, wMax]
#define RANGE16_IN(wVal, wMin, wMax)    ((uint16_t)wMin <= (uint16_t)wVal && (uint16_t)wVal <= (uint16_t)wMax)

//wVal out [wMin, wMax]
#define RANGE16_OUT(wVal, wMin, wMax)   ((uint16_t)wMin > (uint16_t)wVal || (uint16_t)wVal >= (uint16_t)wMax)

#ifndef __cplusplus
#ifndef min
#define min(_x, _y) (_x) < (_y) ? (_x) : (_y)
#endif

#ifndef max
#define max(_x, _y)	(_x) > (_y) ? (_x) : (_y)
#endif
#endif

//#define DUMP_FUNC const char *
#define DUMP_INIT \
static char __sDumpBuf[4096]; \
	char *__pCur = __sDumpBuf; \
	int __iLeft = sizeof(__sDumpBuf); \
	int __iRet = 0
#define DUMP_LINE(fmt, args...) do { \
	if ((__iRet = snprintf(__pCur, __iLeft, fmt , ##args)) >= __iLeft){__pCur[__iLeft - 1] = 0; return __sDumpBuf;} \
	else { __pCur += __iRet; __iLeft -= __iRet; } \
} while (0)
#define DUMP_END return __sDumpBuf

#ifdef __cplusplus
}
#endif

#endif
// end of file
