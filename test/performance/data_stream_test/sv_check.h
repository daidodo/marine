#ifndef _SV_CHECK_H
#define _SV_CHECK_H

#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>
#include <assert.h>

#include "sv_macro.h"

// 检查无符号整数是否可能存在符号溢出
#define CK_U32_SOF(u32) ((int32_t)(u32) < 0)
#define CK_U64_SOF(u32) ((int64_t)(u32) < 0)

#ifdef NDEBUG
#	define declare_assert(x)	int CAT_TOKEN(___dummy, __LINE__) __attribute__((unused))
#else
#	define declare_assert(x) \
	int CAT_TOKEN(___dummy, __LINE__) __attribute__((unused)) = ({assert(x); 0;})
#endif


#ifdef __cplusplus
}
#endif

#endif // _SV_CHECK_H
