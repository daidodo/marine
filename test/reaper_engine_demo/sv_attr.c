#include <sys/sem.h>
#include <sys/shm.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>

//#include "sv_errno.h"
#include "sv_attr.h"

static AttrList * pstAttr = NULL;

static int32_t iProfId = 0; // 是否打开profile, 并决定本进程profile的记录位置
static int32_t iProfAttr1 = 0; // 调用Attr_API的次数
static int32_t iProfAttr2 = 0; // 遍历查找的结点数
#define PROF_NODE1 (pstAttr->astNode[((iProfId)-1)*2])
#define PROF_NODE2 (pstAttr->astNode[((iProfId)-1)*2+1])

inline static int ensureAttrList()
{
    int iShmId;
	if(pstAttr)
		return 0;
	iShmId = shmget(ATTR_SHM_ID, sizeof(AttrList), 0666);
	if(iShmId < 0){
		//perror("cannot shmget Attr shm:");
		return -1;
	}
	if((pstAttr = shmat(iShmId, NULL ,0)) == (void *)-1){
		//perror("cannot shmat Attr shm:");
		return -1;
	}
	return 0;
}

int AttrProfile(int32_t iId, int32_t iAttr1, int32_t iAttr2)
{
	if (iId <= 0 || iId > 16) return -1;
	iProfId = iId;
	iProfAttr1 = iAttr1;
	iProfAttr2 = iAttr2;
	return 0;
}

int ShowAllAttr()
{
	int i;

	i=0;
	while(1)
	{
		if (i>=MAX_ATTR_NODE) break;
		if (pstAttr->astNode[i].iUse == 0) break;

		printf("attr: %d value: %d\n", pstAttr->astNode[i].iAttrID, pstAttr->astNode[i].iCurValue);
		i++;
	}

	return 0;
}

inline static int SearchAttrID(AttrList *pstAttr,int32_t attr,int *piPos)
{
	int i=0;
	*piPos=0;

	if (iProfId) // self profile
	{
		PROF_NODE2.iUse = 1;
		PROF_NODE2.iAttrID = iProfAttr2;
	}

	while(pstAttr->astNode[i].iUse)
	{
		if (iProfId) PROF_NODE2.iCurValue++; // 统计查找次数

		if(pstAttr->astNode[i].iAttrID == attr)
		{
			*piPos=i;
			return 1;
		}
		++i;
		if(i>=MAX_ATTR_NODE)return -1;
	}

	*piPos = i;

	return 0;
}

int Attr_API(int32_t attr,int32_t iValue)
{
	int AttrPos, iRet;

	if (ensureAttrList() != 0)
		return -1;

	if (iProfId){ // self profile
		PROF_NODE1.iUse = 1;
		PROF_NODE1.iAttrID = iProfAttr1;
		PROF_NODE1.iCurValue++;
	}

	iRet = SearchAttrID(pstAttr,attr,&AttrPos);

	if(!iRet){//返回0 attr不存在，返回1 attr存在
		pstAttr->astNode[AttrPos].iUse=1;
		pstAttr->astNode[AttrPos].iAttrID=attr;
		pstAttr->astNode[AttrPos].iCurValue=iValue;
	}else if(iRet==-1){
		return -1;
	}else
		pstAttr->astNode[AttrPos].iCurValue+=iValue;

	return 0;
}

/*
 *
 注:使用返回的piCurValue要小心,不能简单判断if(iCurValue < 5),因为很多情况下会返回iCurValue ==0
 *
 */
int Attr_API_EX(int32_t attr,int32_t iValue, int32_t *piCurValue)
{
	int AttrPos, iRet;

	if(piCurValue) *piCurValue = 0;

	if (ensureAttrList() != 0)
		return -1;

	iRet = SearchAttrID(pstAttr,attr,&AttrPos);

	if(!iRet){//返回0 attr不存在，返回1 attr存在
		pstAttr->astNode[AttrPos].iUse=1;
		pstAttr->astNode[AttrPos].iAttrID=attr;
		pstAttr->astNode[AttrPos].iCurValue=iValue;
		if(piCurValue) *piCurValue = pstAttr->astNode[AttrPos].iCurValue;
	}else if(iRet==-1){
		return -1;
	}else{
		pstAttr->astNode[AttrPos].iCurValue+=iValue;
		if(piCurValue)
			*piCurValue = pstAttr->astNode[AttrPos].iCurValue;
	}
	return 0;
}

int Attr_API_Set(int32_t attr,int32_t iValue)
{
	int AttrPos, iRet;

	if (ensureAttrList() != 0)
		return -1;

	if (iProfId){ // self profile
		PROF_NODE1.iUse = 1;
		PROF_NODE1.iAttrID = iProfAttr1;
		PROF_NODE1.iCurValue++;
	}

	iRet = SearchAttrID(pstAttr,attr,&AttrPos);

	if(!iRet){//返回0 attr不存在，返回1 attr存在
		pstAttr->astNode[AttrPos].iUse=1;
		pstAttr->astNode[AttrPos].iAttrID=attr;
		pstAttr->astNode[AttrPos].iCurValue=iValue;
	}else if(iRet==-1){
		return -1;
	}else
		pstAttr->astNode[AttrPos].iCurValue=iValue;

	return 0;
}
int Attr_API_Get(int32_t attr,int32_t *piValue)
{
	int AttrPos, iRet;

	if (ensureAttrList() != 0)
		return -1;

	iRet = SearchAttrID(pstAttr,attr,&AttrPos);

	if(!iRet){//返回0 attr不存在，返回1 attr存在
		*piValue = 0;
		return 1;
	}else if(iRet==-1){
		*piValue = 0;
		return -1;
	}else
		*piValue = pstAttr->astNode[AttrPos].iCurValue;

	return 0;
}

//===================================================================================================
//带字符串报警特性的实现
//===================================================================================================
static char* g_pBusiShareMem = NULL;

static int semlock()
{
	struct sembuf lock[1];
	int ret = 0;
	int lockid = semget(DEP_SHM_LOCK_ID, 1, 0);
	if (lockid == -1)
	{
		//perror("get singal error\n");
		return -1;
	}

	lock[0].sem_num = 0;
	lock[0].sem_op  = -1;
	lock[0].sem_flg = SEM_UNDO;

	ret = semop(lockid, lock, 1);
	//if (ret<0) 
		//perror("op signal error\n");

	return ret;
}

static int semunlock()
{
	int ret = 0;
	struct sembuf lock[1];
	int lockid = semget(DEP_SHM_LOCK_ID, 1, 0);
	if (lockid == -1)
		return -1;

	lock[0].sem_num = 0;
	lock[0].sem_op  = 1;
	lock[0].sem_flg = SEM_UNDO;

	ret = semop(lockid, lock, 1);
	return ret;
}

inline static int ensureBusiShareMem()
{
    int iShmId;
	if(g_pBusiShareMem)
		return 0;
	iShmId = shmget(DEP_SHM_ID, MAX_DEP_SHMEM_LEN, 0666);
	if(iShmId < 0){
		//perror("cannot shmget BusiShareMem shm:");
		return -1;
	}
	if((g_pBusiShareMem = shmat(iShmId, NULL ,0)) == (void *)-1){
		//perror("cannot shmat BusiShareMem shm:");
		return -1;
	}
	return 0;
}

//异常ID上报携带特性字符串
int Attr_API_Alarm(int32_t attr_id , size_t len , char* pvalue)
{
	char *p = NULL;
	uint32_t *plen = NULL;
	if (attr_id < DEP_ATTTIBUTE_BASE ||
			len == 0 ||
			pvalue == NULL)
		return -1;

	//加锁
	if (semlock() < 0)
		return -1;

	if(ensureBusiShareMem() != 0){
		semunlock();
		return -1;// 修正无法获取共享内存问题,modified by arrowliu, 2006-03-29
	}

	//取可用长度
	plen = (uint32_t *)g_pBusiShareMem;
	if (plen == NULL){
		semunlock();
		return -1;
	}

	if (*plen > MAX_DEP_VALUE_LEN || (MAX_DEP_VALUE_LEN - *plen) < len + 8){ // modified by arrowliu, 2006-08-10
		semunlock();
		return -1;
	}

	//copy value
	p = g_pBusiShareMem;
	p += sizeof(uint32_t);//all the data length
	p += *plen;

	*plen += len + 2*sizeof(int32_t);//increase length,一个整形是长度，一个是id

	//first 4byte is length of data
	*(int32_t *)p = htonl(len);
	p += sizeof(int32_t);

	//second 4byte is attribute id of data
	*(int32_t *)p = htonl(attr_id);
	p += sizeof(int32_t);

	//other buffer for the data
	memcpy(p , pvalue , len);

	semunlock();
	return 0;
}
