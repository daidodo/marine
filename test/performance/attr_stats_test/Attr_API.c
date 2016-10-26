/*
 * =====================================================================================
 *
 *       Filename:  Attr_API.c
 *
 *    Description:  ��ǿ��������ϱ�api
 *					1) ��֤ԭ�Ӳ���, ֧�ֶ��߳�/����̲�������
 *					2) hash��ʽ���빲���ڴ�, ʱ�临�ӶȽ���Ϊ׼O(1)
 *					3) ֧�����1w�����Ե��ϱ�
 *					4) �����monitor����, ���ֶ��¾�api�ļ���, �û������޸�֪���滻���µ�api
 *
 *        Version:  1.0
 *        Created:  12/24/2012 04:53:50 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  sunnyhao
 *        Company:  Tencent
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "Attr_API.h"
#include "CAS_64bit.h"
#include "spin_lock.h"

// 0x5fee = 24558
#define AGENT_ATTR_SUM_SHM_KEY 0x5fee
#define AGENT_ATTR_SET_SHM_KEY 0x5fef

#define DEP_SHM_LOCK_ID         34552
#define DEP_SHM_ID				24552
#define DEP_ATTTIBUTE_BASE		600
#define MAX_DEP_SHMEM_LEN		(2*1024*1024)
#define MAX_DEP_VALUE_LEN		(MAX_DEP_SHMEM_LEN - sizeof(int))

// ����id��, ��ʾ�����Ǵ��ϱ�ip������ֵ
#define	ATTR_EXE_DATA				12345678
// ����id��, ��ʾ�����Ǵ��ϱ�ip�������ַ���
#define	ATTR_EXE_STR				12345679

// ����hash�ĳ��Ⱥͽ���
#define AGENT_ATTR_SHM_HASH_LEN 1000
#define AGENT_ATTR_SHM_HASH_TIMES 40

// ����д��ͻ���Ե�������
#define MAX_ATTEMPT_TIMES 50

// ����������
const int g_arrMod[AGENT_ATTR_SHM_HASH_TIMES] = {998, 997, 991, 983, 982, 977, 976, 974, 971, 967, 
	964, 958, 956, 953, 947, 944, 941, 937, 934, 932, 929, 926, 922, 919, 916, 914, 911, 908, 907, 904, 
	898, 892, 887, 886, 883, 881, 878, 877, 872, 866};

// ������agent�����ڴ��еĽڵ�ṹ
typedef union
{
	struct                                      /* ���԰���һ����Աʹ�� */
	{
		uint32_t dwAttrId;
		uint32_t dwCurVal;
	};

	uint64_t number_value;

} AGENT_ATTR_NODE;

const int AGENT_ATTR_SHM_SIZE = sizeof(AGENT_ATTR_NODE) * AGENT_ATTR_SHM_HASH_LEN * AGENT_ATTR_SHM_HASH_TIMES;

// ȫ�ֱ���
static void* g_pSumShm = NULL;
static void* g_pSetShm = NULL;

static char* OI_GetShm(int iKey, int iSize, int iFlag) 
{
	int iShmID;
	char *sShm;
	char sErrMsg[50];

	if( 0 == iKey ) 
	{
		snprintf(sErrMsg, sizeof(sErrMsg), "shmget %d %d: iKey don't zero(0)", iKey, iSize);
		perror(sErrMsg);
		return(NULL);
	}

	if((iShmID = shmget(iKey, iSize, iFlag)) < 0) 
	{
		snprintf(sErrMsg, sizeof(sErrMsg), "shmget %d %d", iKey, iSize);
		perror(sErrMsg);
		return NULL;
	}

	if((sShm = (char *) shmat(iShmID, NULL, 0)) == (char *) -1) 
	{
		perror("shmat");
		return NULL;
	}

	return sShm;
}

static int OI_GetShm2(void **pstShm, int iKey, int iSize, int iFlag) 
{
	char *sShm;
	if( 0 == iKey ) 
	{
		return(-1);
	}

	if(!(sShm = OI_GetShm(iKey, iSize, iFlag & (~IPC_CREAT)))) 
	{
		if(!(iFlag & IPC_CREAT)) 
		{
			return -1;
		}

		if(!(sShm = OI_GetShm(iKey, iSize, iFlag))) 
		{
			return -1;
		}

		memset(sShm, 0, iSize);
		*pstShm = sShm;
		return(1);
	}

	*pstShm = sShm;
	return 0;
}

static void DetachShm(void)
{
	if ( g_pSumShm != NULL )
	{
		shmdt(g_pSumShm);
		g_pSumShm = NULL;
	}

	if ( g_pSetShm != NULL )
	{
		shmdt(g_pSetShm);
		g_pSetShm = NULL;
	}
}

static int iInitRet = 0;
static void InitShm(void) 
{
	// ��ʼ����ʱ����(��ִ��һ��)
	if ( OI_GetShm2( (void **)&g_pSumShm, AGENT_ATTR_SUM_SHM_KEY, AGENT_ATTR_SHM_SIZE, 0666|IPC_CREAT) < 0 ) 
	{
		g_pSumShm = NULL;
		iInitRet = -1;
	}

	// ��ʼ����ʱ����(��ִ��һ��)
	if ( OI_GetShm2( (void **)&g_pSetShm, AGENT_ATTR_SET_SHM_KEY, AGENT_ATTR_SHM_SIZE, 0666|IPC_CREAT) < 0 ) 
	{
		g_pSetShm = NULL;
		iInitRet = -1;
	}

	// �˳���ʱ��ж�ع����ڴ�
	atexit(DetachShm);
}

static spinlock_t lock = SPINLOCK_INITIALIZER;
//static pthread_once_t pthInitDone = PTHREAD_ONCE_INIT;

static void InitOnce(void)
{
	spinlock_lock(&lock);                       /* ���� */
	InitShm();
	spinlock_unlock(&lock);                     /* ���� */
//	pthread_once(&pthInitDone, InitShm);
}

typedef int (*UPDATE_METHOD_PTR)(volatile AGENT_ATTR_NODE*, uint32_t, uint32_t, uint32_t*);

static inline int callback_AddValue(volatile AGENT_ATTR_NODE* pNode, uint32_t dwAttrId, uint32_t dwNewVal, uint32_t *pdwValue)
{
	volatile AGENT_ATTR_NODE stNodeCopy = *pNode;

	// �ж� stNodeCopy ��ȡֵ֮ǰ, pNode �е����ݱ������߳�/�����޸Ĺ�
	if ( stNodeCopy.dwAttrId != dwAttrId )
	{
		return 1;
	}

	// �ж� CAS ���½ڵ�����Ƿ�ɹ�
	volatile AGENT_ATTR_NODE stNewNode;
	stNewNode.dwAttrId = dwAttrId;
	stNewNode.dwCurVal = stNodeCopy.dwCurVal + dwNewVal;

	// �ж� CAS ���½ڵ�����Ƿ�ɹ�
	if ( !CAS_64bit(&pNode->number_value, stNodeCopy.number_value, stNewNode.number_value) )
	{
		return 2;
	}

	if ( NULL != pdwValue )
	{
		*pdwValue = stNewNode.dwCurVal;
	}

	return 0;
}

static inline int callback_SetValue(volatile AGENT_ATTR_NODE* pNode, uint32_t dwAttrId, uint32_t dwNewVal, uint32_t *pdwValue)
{
	volatile AGENT_ATTR_NODE stNodeCopy = *pNode;

	// �ж� stNodeCopy ��ȡֵ֮ǰ, pNode �е����ݱ������߳�/�����޸Ĺ�
	if ( stNodeCopy.dwAttrId != dwAttrId )
	{
		return 1;
	}

	// �ж� CAS ���½ڵ�����Ƿ�ɹ�
	volatile AGENT_ATTR_NODE stNewNode;
	stNewNode.dwAttrId = dwAttrId;
	stNewNode.dwCurVal = dwNewVal;

	if ( !CAS_64bit(&pNode->number_value, stNodeCopy.number_value, stNewNode.number_value) ) 
	{
		return 2;
	}

	if ( NULL != pdwValue )
	{
		*pdwValue = stNewNode.dwCurVal;
	}

	return 0;
}

// ���� attri_id ��Ӧ�Ľڵ�, ��������ھʹ���һ���½ڵ�
int CreateOrUpdateNode(volatile void* pShm, uint32_t dwAttrId, uint32_t dwNewVal, UPDATE_METHOD_PTR UpdateNode, uint32_t dwRecurDepth, uint32_t* pdwValue)
{
	if ( dwRecurDepth > MAX_ATTEMPT_TIMES )     /* ��������Դ���, ��ֹͣ�ݹ�, ��ֹ������ѭ�� */
	{
		return -1;
	}

	// ȷ���Ѿ���ʼ���������ڴ�
	if ( !pShm || dwAttrId == 0 )
	{
		return -2;
	}

	int i = 0;
	uint32_t dwHashVal = 0;
	volatile AGENT_ATTR_NODE *pNode = NULL;

	// ��¼�սڵ���Ϊ���α�ѡ�ڵ�
	int iEmptyNodeSize = 0;
	volatile AGENT_ATTR_NODE* arrEmptyNodeAddr[AGENT_ATTR_SHM_HASH_TIMES] = {NULL}; /* ��ʼ��ȫΪNULL */

	// ���Բ��ҽڵ�
	for ( i = 0; i < AGENT_ATTR_SHM_HASH_TIMES; i++ )
	{
		dwHashVal = dwAttrId % g_arrMod[i];
		pNode = (volatile AGENT_ATTR_NODE*)pShm + AGENT_ATTR_SHM_HASH_LEN*i + dwHashVal;

		if ( 0 == pNode->dwAttrId )				/* ��¼(attrid==0)�Ľڵ��б� */
		{
			arrEmptyNodeAddr[iEmptyNodeSize++] = pNode;
		}
		else
		{
			if ( pNode->dwAttrId == dwAttrId )  /* �ҵ���Ӧ�ڵ� */
			{
				if ( UpdateNode(pNode, dwAttrId, dwNewVal, pdwValue) > 0 ) /* ����ʧ��, �ݹ����� */
				{
					return CreateOrUpdateNode(pShm, dwAttrId, dwNewVal, UpdateNode, dwRecurDepth+1, pdwValue);
				}

				return 0;
			}
		}
	}

	// �����½��ڵ�
	volatile AGENT_ATTR_NODE stNewNode;
	stNewNode.dwAttrId = dwAttrId;
	stNewNode.dwCurVal = dwNewVal;

	volatile AGENT_ATTR_NODE stEmptyNode;
	stEmptyNode.number_value = 0;

	// 1) ������attrid==0�Ŀ��нڵ����½��ڵ�
	if ( iEmptyNodeSize > 0 )                   /* ����attrid==0�Ŀ��нڵ� */
	{
		for ( i = 0; i < iEmptyNodeSize; i++ )
		{
			// �ж� CAS �����ڵ�����Ƿ�ɹ�
			if ( CAS_64bit(&arrEmptyNodeAddr[i]->number_value, stEmptyNode.number_value, stNewNode.number_value) )
			{
				return 0;
			}

			// ����ڵ��Ѿ�����������/�̴߳���, ��ô�͵ݹ�����(����)
			if ( arrEmptyNodeAddr[i]->dwAttrId == dwAttrId )
			{
				return CreateOrUpdateNode(pShm, dwAttrId, dwNewVal, UpdateNode, dwRecurDepth+1, pdwValue);
			}

			// ����ڵ��Ǳ��������Զ�Ӧ�Ľڵ�ռ��, ��ô�ͼ���ѭ������
		}
	}

	// 2) ������value==0�Ŀ��нڵ����½��ڵ�
	if ( iEmptyNodeSize == 0 )					/* û��attrid==0�Ŀ��нڵ� */
	{
		volatile AGENT_ATTR_NODE stNodeCopy;
		for ( i = AGENT_ATTR_SHM_HASH_TIMES-1; i >= 0; i-- )
		{
			dwHashVal = dwAttrId % g_arrMod[i];
			pNode = (volatile AGENT_ATTR_NODE*)pShm + AGENT_ATTR_SHM_HASH_LEN*i + dwHashVal;

			stNodeCopy = *pNode;
			if ( 0 == stNodeCopy.dwCurVal )
			{
				// �ж� CAS �����ڵ�����Ƿ�ɹ�
				if ( CAS_64bit(&pNode->number_value, stNodeCopy.number_value, stNewNode.number_value))
				{
					return 0;
				}

				// ����ڵ��Ǳ��������Զ�Ӧ�Ľڵ�ռ��, ��ô�ͼ���ѭ������
			}
		}
	}

	// ���г��Զ�ʧ��
	return -3;
}

int Attr_API(int iAttrId, int iValue)
{
	// ���ع����ڴ�
	if ( NULL == g_pSumShm )
	{
		InitOnce();                             /* �̰߳�ȫ */
		if ( 0 != iInitRet || NULL == g_pSumShm )
		{
			return -1;
		}
	}

	return CreateOrUpdateNode(g_pSumShm, iAttrId, iValue, callback_AddValue, 0, NULL);
}

int Attr_API_EX(int iAttrId, int iValue, int *piCurValue)
{
	// ���ع����ڴ�
	if ( NULL == g_pSumShm )
	{
		InitOnce();                             /* �̰߳�ȫ */
		if ( 0 != iInitRet || NULL == g_pSumShm )
		{
			return -1;
		}
	}

	return CreateOrUpdateNode(g_pSumShm, iAttrId, iValue, callback_AddValue, 0, (uint32_t*)piCurValue);
}

int Attr_API_Set(int iAttrId, int iValue)
{
	// ���ع����ڴ�
	if ( NULL == g_pSetShm )
	{
		InitOnce();                             /* �̰߳�ȫ */
		if ( 0 != iInitRet || NULL == g_pSetShm )
		{
			return -1;
		}
	}

	return CreateOrUpdateNode(g_pSetShm, iAttrId, iValue, callback_SetValue, 0, NULL);
}

int Attr_API_Get(int iAttrId, int* piValue)
{
	int i = 0;
	uint32_t dwHashVal = 0;
	AGENT_ATTR_NODE *pNode = NULL;

	for ( i = 0; i < AGENT_ATTR_SHM_HASH_TIMES; i++ )
	{
		dwHashVal = iAttrId % g_arrMod[i];

		// ��sum���ڴ��в���
		pNode = (AGENT_ATTR_NODE*)g_pSumShm + AGENT_ATTR_SHM_HASH_LEN*i + dwHashVal;
		if ( pNode->dwAttrId == (uint32_t)iAttrId )  /* �ҵ���Ӧ�ڵ� */
		{
			*piValue = pNode->dwCurVal;
			return 0;
		}

		// ��set���ڴ��в���
		pNode = (AGENT_ATTR_NODE*)g_pSetShm + AGENT_ATTR_SHM_HASH_LEN*i + dwHashVal;
		if ( pNode->dwAttrId == (uint32_t)iAttrId )  /* �ҵ���Ӧ�ڵ� */
		{
			*piValue = pNode->dwCurVal;
			return 0;
		}
	}

	*piValue = 0;
	return -1;
}

//===================================================================================================
//���ַ����������Ե�ʵ��
//===================================================================================================
static char *g_pBusiShareMem = NULL;

static int
semlock()
{
    struct sembuf lock[1];
    int ret = 0;
    int lockid = semget(DEP_SHM_LOCK_ID, 1, 0);

    if(lockid == -1)
    {
        perror("get singal error\n");
        return -1;
    }

    lock[0].sem_num = 0;
    lock[0].sem_op = -1;
    lock[0].sem_flg = SEM_UNDO;

    ret = semop(lockid, lock, 1);
    if(ret < 0)
        perror("op signal error\n");

    return ret;
}

static int
semunlock()
{
    int ret = 0;
    struct sembuf lock[1];
    int lockid = semget(DEP_SHM_LOCK_ID, 1, 0);

    if(lockid == -1)
        return -1;

    lock[0].sem_num = 0;
    lock[0].sem_op = 1;
    lock[0].sem_flg = SEM_UNDO;

    ret = semop(lockid, lock, 1);
    return ret;
}

//�쳣ID�ϱ�Я�������ַ���
int
Attr_API_Alarm(int attr_id, int len, char *pvalue)
{
    char *p = NULL;
    int *plen = NULL;

    if(attr_id < DEP_ATTTIBUTE_BASE || len == 0 || pvalue == NULL)
        return -1;

    //����  
    if(semlock() < 0)
        return -1;

    if(!g_pBusiShareMem && OI_GetShm2((void **) &g_pBusiShareMem, DEP_SHM_ID, MAX_DEP_SHMEM_LEN, 0666) < 0)
    {
        semunlock();
        return -1;  // �����޷���ȡ�����ڴ�����,modified by arrowliu, 2006-03-29
    }

    //ȡ���ó���
    plen = (int *) g_pBusiShareMem;
    if(plen == NULL)
    {
        semunlock();
        return -1;
    }

    if(*plen > MAX_DEP_VALUE_LEN || (MAX_DEP_VALUE_LEN - *plen) < len + 8)  // modified by arrowliu, 2006-08-10
    {
        semunlock();
        return -1;
    }

    //copy value
    p = g_pBusiShareMem;
    p += sizeof(int);   //all the data length
    p += *plen;

    *plen += len + 2 * sizeof(int); //increase length,һ�������ǳ��ȣ�һ����id

    //first 4byte is length of data
    *(int *) p = htonl(len);

    //second 4byte is attribute id of data
    p += sizeof(int);
    *(int *) p = htonl(attr_id);

    //other buffer for the data
    p += sizeof(int);
    memcpy(p, pvalue, len);

    semunlock();
    return 0;
}

int AttrProfile(int iId, int iAttrId1, int iAttrId2)
{
	printf("not support!\n");
	return 0;
}

#define ULONG uint32_t
#define setULong_H(_pos, value) {*((ULONG*)(_pos))=htonl(value);}

int setNumAttrWithIP(const char* szIP, int iAttrId, int iValue)
{
	char buf[12];
	char *p = NULL;
	int iIP = -1;
	struct in_addr inaddr;

	if ( !szIP )
	{
		fprintf(stderr, "strIP is NULL, error!\n");
		return -1;
	}

	if ( 0==inet_aton(szIP, &inaddr) )
	{
		fprintf(stderr, "invalid IP: %s\n", szIP);
		return -1;
	}

	iIP = inaddr.s_addr;

	if ( iAttrId <= 0 )
	{
		fprintf(stderr, "invalid iAttrID: %d\n", iAttrId);
		return -1;
	}

	// ��װЭ�飬�����ֽ���
	// 4���ֽڵ�IP, 4���ֽ�id��4���ֽ�value
	bzero(buf, sizeof(buf));
	p = buf;
	setULong_H(p, iIP);
	p+=4;

	setULong_H(p, iAttrId);
	p+=4;

	setULong_H(p, iValue);
	p+=4;

	return Attr_API_Alarm(ATTR_EXE_DATA, sizeof(buf), buf);
}

int setStrAttrWithIP(const char* strIP, int iAttrID, size_t len , char* pval) 
{
	char buf[65536];
	char* p = NULL;
	int iIP = -1;
	struct in_addr inaddr;

	if (len > sizeof(buf)) 
	{
		fprintf(stderr, "len is %zd, longger than 65536!\n", len);
		return -1;
	}

	if (!strIP) 
	{
		fprintf(stderr, "strIP is NULL, error!\n");
		return -1;
	}

	if (0==inet_aton(strIP, &inaddr)) 
	{
		fprintf(stderr, "invalid IP: %s\n", strIP);
		return -1;
	}

	iIP = inaddr.s_addr;

	if (iAttrID <=0) 
	{
		fprintf(stderr, "invalid iAttrID: %d\n", iAttrID);
		return -1;
	}

	// ��װЭ�飬�����ֽ���
	// 4���ֽڵ�IP, 4���ֽ�id��4���ֽڳ��ȣ�len���ֽ�����
	bzero(buf, sizeof(buf));
	p = buf;
	setULong_H(p, iIP);
	p+=4;

	setULong_H(p, iAttrID);
	p+=4;

	setULong_H(p, len);
	p+=4;

	memcpy(p, pval, len);
	p+=len;

	return Attr_API_Alarm(ATTR_EXE_STR, p-buf, buf);
}
