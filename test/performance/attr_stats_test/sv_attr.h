#ifndef _SV_ATTR_H
#define _SV_ATTR_H

#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>
#include <stddef.h>

#define	ATTR_SHM_ID		24555
#define MAX_ATTR_NODE	1000

//带字符串报警特性相关的宏定义
#define DEP_ATTTIBUTE_BASE 600
#define DEP_SHM_ID              24552//告警共享内存
#define DEP_SHM_LOCK_ID         34552
#define MAX_DEP_SHMEM_LEN  2*1024*1024
#define MAX_DEP_VALUE_LEN  (MAX_DEP_SHMEM_LEN - sizeof(int))
#define ATTRID_ALARM(id, fmt, args...) ({ \
		static char __sBuf[1024];\
		int __iRet = 0;\
		__iRet = snprintf(__sBuf, sizeof(__sBuf), fmt, ##args);\
		if(__iRet < 0) __iRet = sizeof(__sBuf) - 1;\
		Attr_API_Alarm(id, __iRet, __sBuf);\
})
#define ATTR_ALARM(fmt, args...) ATTRID_ALARM(52574, fmt, ##args)

typedef struct
{
	int32_t iUse;
	int32_t iAttrID;
	int32_t iCurValue;
} AttrNode;

typedef struct
{
	AttrNode astNode[MAX_ATTR_NODE];
} AttrList;

int Attr_API(int32_t attr,int32_t iValue);
int Attr_API_EX(int32_t attr,int32_t iValue, int32_t *piCurValue);
int Attr_API_Set(int32_t attr,int32_t iValue);
int Attr_API_Get(int32_t attr,int32_t *piValue);
int Attr_API_Alarm(int32_t attr_id , size_t len , char* pvalue);

int AttrProfile(int32_t iId, int32_t iAttr1, int32_t iAttr2);


#ifdef __cplusplus
}
#endif

#endif
