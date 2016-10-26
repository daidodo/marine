#ifndef __SV_FREQ_CTRL_H__
#define __SV_FREQ_CTRL_H__

#ifdef __cplusplus
extern "C" {
#endif


#include <sys/time.h>
#include <stdint.h>

typedef struct {
	uint32_t dwFreqPerSec;
	uint32_t dwBucketSize;
	int64_t llTokenCount;
	uint64_t qwLastGenTime;
	uint64_t qwLastCalcDelta;
} TokenBucket;

/*
 * 初始化令牌桶
 * @pstTB [IN] TokenBucket描述符指针
 * @dwFreqPerSec [IN] 用户预设频率，单位是1/s
 * @dwBucketSize [IN] 令牌桶大小，桶越大允许的突发流量越大，建议一般设置<=qwFreqPerSec
 * return 0表示成功 <0失败
 */
int TokenBucket_Init(TokenBucket *pstTB, uint32_t dwFreqPerSec, uint32_t dwBucketSize);

/*
 * 根据用户预设频率产生令牌，需周期性调用
 * @pstTB [IN] TokenBucket描述符指针
 * @ptvNow [IN] 当前时间戳，如填NULL则由内部调用gettimeofday获取
 * return 0表示成功 <0失败
 */
int TokenBucket_Gen(TokenBucket *pstTB, const struct timeval *ptvNow);

/*
 * 检查令牌是否足够，用于做频率控制
 * @pstTB [IN] TokenBucket描述符指针
 * @NeedTokens [IN] 需要的令牌数，本函数检查当前桶中的令牌是否足够
 * return 0表示足够 <0表示不够
 */
int TokenBucket_Check(TokenBucket *pstTB, uint32_t dwNeedTokens);

/*
 * 检查令牌是否足够并扣除令牌，用于做频率控制
 * @pstTB [IN] TokenBucket描述符指针
 * @NeedTokens [IN] 需要的令牌数，本函数检查当前桶中的令牌是否足够，并且将这些令牌取出
 * return 0表示成功取出 <0表示不够未取出
 */
int TokenBucket_Get(TokenBucket *pstTB, uint32_t dwNeedTokens);

/*
 * 直接扣除令牌，可透支令牌，慎用
 * @pstTB [IN] TokenBucket描述符指针
 * @NeedTokens [IN] 需要的令牌数，本函数不检查当前个数直接取出令牌，可以透支令牌
 * return 0表示成功取出 >0透支的令牌数
 */
int TokenBucket_Overdraft(TokenBucket *pstTB, uint32_t dwNeedTokens);

/*
 * 修改令牌桶
 * @pstTB [IN] TokenBucket描述符指针
 * @dwFreqPerSec [IN] 新的频率，单位是1/s
 * @dwBucketSize [IN] 新的令牌桶大小，桶越大允许的突发流量越大，建议一般设置<=qwFreqPerSec
 * return 无
 */
void TokenBucket_Mod(TokenBucket * pstTB, uint32_t dwFreqPerSec, uint32_t dwBucketSize);

static inline uint32_t TokenBucket_Tokens(TokenBucket *pstTB)
{
	return (uint32_t)(pstTB->llTokenCount <= 0 ? 0 : pstTB->llTokenCount);
}

const char * TokenBucket_ToStr(const TokenBucket * pstTB);


#ifdef __cplusplus
}
#endif

#endif
