#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>

//#include "sv_errno.h"
//#include "sv_check.h"
//#include "sv_misc.h"
#include "sv_freq_ctrl.h"

#define TV2US(ptv) ((ptv)->tv_sec * 1000000 + (ptv)->tv_usec)

int TokenBucket_Init(TokenBucket *pstTB, uint32_t dwFreqPerSec, uint32_t dwBucketSize)
{
	//declare_assert(pstTB);
	struct timeval tv;

	memset(pstTB, 0, sizeof(*pstTB));
	pstTB->dwFreqPerSec = dwFreqPerSec;
	pstTB->dwBucketSize = dwBucketSize;
	pstTB->llTokenCount = dwBucketSize; // 防止刚初始化后无令牌而丢包
	gettimeofday(&tv, NULL);
	pstTB->qwLastGenTime = TV2US(&tv);

	return 0;
}

int TokenBucket_Gen(TokenBucket *pstTB, const struct timeval *ptvNow)
{
	//declare_assert(pstTB);
	struct timeval tv;
	uint64_t qwNow, qwPastUs;
	uint64_t qwNewTokens, qwCalcDelta;
	int64_t llNewTokenCount;

	if (ptvNow == NULL) {
		gettimeofday(&tv, NULL);
		ptvNow = &tv;
	}
	qwNow = TV2US(ptvNow);
	if (qwNow < pstTB->qwLastGenTime) {
		// 时间回跳
		pstTB->qwLastGenTime = qwNow;
		return -1;
	}

	qwPastUs = qwNow - pstTB->qwLastGenTime;
	qwNewTokens = (((uint64_t)pstTB->dwFreqPerSec * qwPastUs + pstTB->qwLastCalcDelta) / 1000000);
	qwCalcDelta = (((uint64_t)pstTB->dwFreqPerSec * qwPastUs + pstTB->qwLastCalcDelta) % 1000000);

	// 生成Tokens
	pstTB->qwLastGenTime = qwNow;
	pstTB->qwLastCalcDelta = qwCalcDelta;
	llNewTokenCount = pstTB->llTokenCount + qwNewTokens;
	if (llNewTokenCount < pstTB->llTokenCount) {
		// 溢出
		pstTB->llTokenCount = pstTB->dwBucketSize;
		return -1;
	}
	if (llNewTokenCount > pstTB->dwBucketSize) {
		llNewTokenCount = pstTB->dwBucketSize;
	}
	pstTB->llTokenCount = llNewTokenCount;

	return 0;
}

int TokenBucket_Check(TokenBucket *pstTB, uint32_t dwNeedTokens)
{
	//declare_assert(pstTB);

	if (pstTB->llTokenCount < (int64_t)dwNeedTokens) {
		return -1;
	}
	return 0;
}

int TokenBucket_Get(TokenBucket *pstTB, uint32_t dwNeedTokens)
{
	//declare_assert(pstTB);

	if (pstTB->llTokenCount < (int64_t)dwNeedTokens) {
		return -1;
	}
	pstTB->llTokenCount -= dwNeedTokens;
	return 0;
}

int TokenBucket_Overdraft(TokenBucket *pstTB, uint32_t dwNeedTokens)
{
	//declare_assert(pstTB);

	pstTB->llTokenCount -= dwNeedTokens;
	return (pstTB->llTokenCount < 0 ? -pstTB->llTokenCount : 0);
}

void TokenBucket_Mod(TokenBucket * pstTB, uint32_t dwFreqPerSec, uint32_t dwBucketSize)
{
	//assert(pstTB);
	pstTB->dwFreqPerSec = dwFreqPerSec;
	pstTB->dwBucketSize = dwBucketSize;
}
