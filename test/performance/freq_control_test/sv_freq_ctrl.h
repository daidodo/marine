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
 * ��ʼ������Ͱ
 * @pstTB [IN] TokenBucket������ָ��
 * @dwFreqPerSec [IN] �û�Ԥ��Ƶ�ʣ���λ��1/s
 * @dwBucketSize [IN] ����Ͱ��С��ͰԽ�������ͻ������Խ�󣬽���һ������<=qwFreqPerSec
 * return 0��ʾ�ɹ� <0ʧ��
 */
int TokenBucket_Init(TokenBucket *pstTB, uint32_t dwFreqPerSec, uint32_t dwBucketSize);

/*
 * �����û�Ԥ��Ƶ�ʲ������ƣ��������Ե���
 * @pstTB [IN] TokenBucket������ָ��
 * @ptvNow [IN] ��ǰʱ���������NULL�����ڲ�����gettimeofday��ȡ
 * return 0��ʾ�ɹ� <0ʧ��
 */
int TokenBucket_Gen(TokenBucket *pstTB, const struct timeval *ptvNow);

/*
 * ��������Ƿ��㹻��������Ƶ�ʿ���
 * @pstTB [IN] TokenBucket������ָ��
 * @NeedTokens [IN] ��Ҫ������������������鵱ǰͰ�е������Ƿ��㹻
 * return 0��ʾ�㹻 <0��ʾ����
 */
int TokenBucket_Check(TokenBucket *pstTB, uint32_t dwNeedTokens);

/*
 * ��������Ƿ��㹻���۳����ƣ�������Ƶ�ʿ���
 * @pstTB [IN] TokenBucket������ָ��
 * @NeedTokens [IN] ��Ҫ������������������鵱ǰͰ�е������Ƿ��㹻�����ҽ���Щ����ȡ��
 * return 0��ʾ�ɹ�ȡ�� <0��ʾ����δȡ��
 */
int TokenBucket_Get(TokenBucket *pstTB, uint32_t dwNeedTokens);

/*
 * ֱ�ӿ۳����ƣ���͸֧���ƣ�����
 * @pstTB [IN] TokenBucket������ָ��
 * @NeedTokens [IN] ��Ҫ��������������������鵱ǰ����ֱ��ȡ�����ƣ�����͸֧����
 * return 0��ʾ�ɹ�ȡ�� >0͸֧��������
 */
int TokenBucket_Overdraft(TokenBucket *pstTB, uint32_t dwNeedTokens);

/*
 * �޸�����Ͱ
 * @pstTB [IN] TokenBucket������ָ��
 * @dwFreqPerSec [IN] �µ�Ƶ�ʣ���λ��1/s
 * @dwBucketSize [IN] �µ�����Ͱ��С��ͰԽ�������ͻ������Խ�󣬽���һ������<=qwFreqPerSec
 * return ��
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
