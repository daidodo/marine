#include <arpa/inet.h>
#include <string.h>
#include <assert.h>

#include "sv_errno.h"
#include "sv_check.h"
#include "sv_net.h"
#include "sv_pack.h"

int C2_AddByte(void **ppCur, uint32_t *pdwLeft, uint8_t bValue)
{
	assert(ppCur && *ppCur);
	if(pdwLeft){
		if(CK_U32_SOF(*pdwLeft))
			return E_BAD_PARAM;
		if(*pdwLeft < sizeof bValue)
			return E_OVERFLOW;
	}

	**(uint8_t **)ppCur = bValue;
	*ppCur = (char *)*ppCur + 1;
	if(pdwLeft)
		-- *pdwLeft;

	return 0;
}

inline static int addWord(void **ppCur, uint32_t *pdwLeft, uint16_t wValue, int iNetByteOrder)
{
	assert(ppCur && *ppCur);
	if(pdwLeft){
		if(CK_U32_SOF(*pdwLeft))
			return E_BAD_PARAM;
		if(*pdwLeft < sizeof wValue)
			return E_OVERFLOW;
	}

	**(uint16_t **)ppCur = (iNetByteOrder ? htons(wValue) : wValue);
	*ppCur = (char *)*ppCur + sizeof wValue;
	if(pdwLeft)
		*pdwLeft -= sizeof wValue;

	return 0;
}

inline static int addDWord(void **ppCur, uint32_t *pdwLeft, uint32_t dwValue, int iNetByteOrder)
{
	assert(ppCur && *ppCur);
	if(pdwLeft){
		if(CK_U32_SOF(*pdwLeft))
			return E_BAD_PARAM;
		if(*pdwLeft < sizeof dwValue)
			return E_OVERFLOW;
	}

	**(uint32_t **)ppCur = (iNetByteOrder ? htonl(dwValue) : dwValue);
	*ppCur = (char *)*ppCur + sizeof dwValue;
	if(pdwLeft)
		*pdwLeft -= sizeof dwValue;

	return 0;
}

inline static int addQWord(void **ppCur, uint32_t *pdwLeft, uint64_t qwValue, int iNetByteOrder)
{
	assert(ppCur && *ppCur);
	if(pdwLeft){
		if(CK_U32_SOF(*pdwLeft))
			return E_BAD_PARAM;
		if(*pdwLeft < sizeof qwValue)
			return E_OVERFLOW;
	}

	**(uint64_t **)ppCur = (iNetByteOrder ? Hton64(qwValue) : qwValue);
	*ppCur = (char *)*ppCur + sizeof qwValue;
	if(pdwLeft)
		*pdwLeft -= sizeof qwValue;

	return 0;
}

int C2_AddBuf(void **ppCur, uint32_t *pdwLeft, const void *pBuf, uint32_t dwBufLen)
{
	assert(ppCur && *ppCur);
	assert(pBuf);
	if(dwBufLen){
		if(CK_U32_SOF(dwBufLen))
			return E_BAD_PARAM;
		if(pdwLeft){
			if(CK_U32_SOF(*pdwLeft))
				return E_BAD_PARAM;
			if(*pdwLeft < dwBufLen)
				return E_OVERFLOW;
		}

		memcpy(*ppCur,pBuf,dwBufLen);
		*ppCur = (char *)*ppCur + dwBufLen;
		if(pdwLeft)
			*pdwLeft -= dwBufLen;
	}

	return 0;
}

inline static int addWLenBuf(void **ppCur, uint32_t *pdwLeft, const void *pBuf, uint32_t dwBufLen, int iNetByteOrder)
{
    int iRet;
	assert(ppCur && *ppCur);
	assert(pBuf);

	if(CK_U32_SOF(dwBufLen))
		return E_BAD_PARAM;
	if(pdwLeft){
		if(CK_U32_SOF(*pdwLeft))
			return E_BAD_PARAM;
		if(*pdwLeft < sizeof(uint16_t) + dwBufLen)
			return E_OVERFLOW;
	}

	iRet = addWord(ppCur,pdwLeft,dwBufLen, iNetByteOrder);
	if(0 != iRet)
		return iRet;

	return C2_AddBuf(ppCur,pdwLeft,pBuf,dwBufLen);
}

int C2_GetByte(const void **ppCur, uint32_t *pdwLeft, uint8_t *pbValue)
{
	assert(ppCur && *ppCur);
	if(pdwLeft){
		if(CK_U32_SOF(*pdwLeft))
			return E_BAD_PARAM;
		if(*pdwLeft < sizeof *pbValue)
			return E_UNDERFLOW;
	}

	if(pbValue)
		*pbValue = **(const uint8_t **)ppCur;
	*ppCur = (const char *)*ppCur + 1;
	if(pdwLeft)
		-- *pdwLeft;

	return 0;
}

inline static int getWord(const void **ppCur, uint32_t *pdwLeft, uint16_t *pwValue, int iNetByteOrder)
{
	assert(ppCur && *ppCur);
	if(pdwLeft){
		if(CK_U32_SOF(*pdwLeft))
			return E_BAD_PARAM;
		if(*pdwLeft < sizeof *pwValue)
			return E_UNDERFLOW;
	}

	if(pwValue){
		*pwValue = **(const uint16_t **)ppCur;
		if(iNetByteOrder)
			*pwValue = htons(*pwValue);
	}
	*ppCur = (const char *)*ppCur + sizeof *pwValue;
	if(pdwLeft)
		*pdwLeft -= sizeof *pwValue;

	return 0;
}

inline static int getDWord(const void **ppCur, uint32_t *pdwLeft, uint32_t *pdwValue, int iNetByteOrder)
{
	assert(ppCur && *ppCur);
	if(pdwLeft){
		if(CK_U32_SOF(*pdwLeft))
			return E_BAD_PARAM;
		if(*pdwLeft < sizeof *pdwValue)
			return E_UNDERFLOW;
	}

	if(pdwValue){
		*pdwValue = **(const uint32_t **)ppCur;
		if(iNetByteOrder)
			*pdwValue = htonl(*pdwValue);
	}
	*ppCur = (const char *)*ppCur + sizeof *pdwValue;
	if(pdwLeft)
		*pdwLeft -= sizeof *pdwValue;

	return 0;
}

inline static int getQWord(const void **ppCur, uint32_t *pdwLeft, uint64_t *pqwValue, int iNetByteOrder)
{
	assert(ppCur && *ppCur);
	if(pdwLeft){
		if(CK_U32_SOF(*pdwLeft))
			return E_BAD_PARAM;
		if(*pdwLeft < sizeof *pqwValue)
			return E_UNDERFLOW;
	}

	if(pqwValue){
		*pqwValue = **(const uint64_t **)ppCur;
		if(iNetByteOrder)
			*pqwValue = Hton64(*pqwValue);
	}
	*ppCur = (const char *)*ppCur + sizeof *pqwValue;
	if(pdwLeft)
		*pdwLeft -= sizeof *pqwValue;

	return 0;
}

int C2_GetBuf(const void **ppCur, uint32_t *pdwLeft, void *pBuf, uint32_t dwBufLen)
{
	assert(ppCur && *ppCur);

	if(dwBufLen){
		if(CK_U32_SOF(dwBufLen))
			return E_BAD_PARAM;
		if(pdwLeft){
			if(CK_U32_SOF(*pdwLeft))
				return E_BAD_PARAM;
			if(*pdwLeft < dwBufLen)
				return E_UNDERFLOW;
		}

		if(pBuf)
			memcpy(pBuf,*ppCur,dwBufLen);
		*ppCur = (const char *)*ppCur + dwBufLen;
		if(pdwLeft)
			*pdwLeft -= dwBufLen;
	}

	return 0;
}

inline static int getWLenBuf(const void **ppCur, uint32_t *pdwLeft, void *pBuf, uint16_t *pwBufLen, int iNetByteOrder)
{
	declare_assert(ppCur);
	const void * const pTmp = *ppCur;
	const uint32_t dwTmp = (pdwLeft ? *pdwLeft : 0);
	uint16_t wLen = 0;

	int iRet = getWord(ppCur,pdwLeft,&wLen, iNetByteOrder);
	if(0 == iRet){
		if(!pwBufLen){
			iRet = C2_GetBuf(ppCur,pdwLeft, 0, wLen);
		}else if(wLen <= *pwBufLen){
			iRet = C2_GetBuf(ppCur,pdwLeft,pBuf,wLen);
		}else
			iRet = E_OVERFLOW;
	}
	if(0 == iRet){
		if(pwBufLen)
			*pwBufLen = wLen;
	}else{
		*ppCur = pTmp;
		if(pdwLeft)
			*pdwLeft = dwTmp;
	}
	return iRet;
}

inline static int getWLenBufPtr(const void **ppCur, uint32_t *pdwLeft, const void **ppBuf, uint16_t *pwBufLen, int iNetByteOrder)
{
	declare_assert(ppCur);
	declare_assert(ppBuf && pwBufLen);

	const void * const pTmp = *ppCur;
	const uint32_t dwTmp = (pdwLeft ? *pdwLeft : 0);
	uint16_t wLen = 0;

	int iRet = getWord(ppCur,pdwLeft,&wLen, iNetByteOrder);
	if(0 == iRet){
		if(!pdwLeft || (pdwLeft && wLen <= *pdwLeft)){
			*ppBuf = *ppCur;
			*ppCur = (const char *)*ppCur + wLen;
			*pwBufLen = wLen;
			if(pdwLeft)
				*pdwLeft -= wLen;
		}else
			iRet = E_OVERFLOW;
	}
	if(0 != iRet){
		*ppCur = pTmp;
		if(pdwLeft)
			*pdwLeft = dwTmp;
	}
	return iRet;
}

int C2_AddWord(void **ppCur, uint32_t *pdwLeft, uint16_t wValue)
{
	return addWord(ppCur, pdwLeft, wValue, 1);
}

int C2_AddDWord(void **ppCur, uint32_t *pdwLeft, uint32_t dwValue)
{
	return addDWord(ppCur, pdwLeft, dwValue, 1);
}

int C2_AddQWord(void **ppCur, uint32_t *pdwLeft, uint64_t qwValue)
{
	return addQWord(ppCur, pdwLeft, qwValue, 1);
}

int C2_AddWLenBuf(void **ppCur, uint32_t *pdwLeft, const void *pBuf, uint32_t dwBufLen)
{
	return addWLenBuf(ppCur, pdwLeft, pBuf, dwBufLen, 1);
}

int C2_GetWord(const void **ppCur, uint32_t *pdwLeft, uint16_t *pwValue)
{
	return getWord(ppCur, pdwLeft, pwValue, 1);
}

int C2_GetDWord(const void **ppCur, uint32_t *pdwLeft, uint32_t *pdwValue)
{
	return getDWord(ppCur, pdwLeft, pdwValue, 1);
}

int C2_GetQWord(const void **ppCur, uint32_t *pdwLeft, uint64_t *pqwValue)
{
	return getQWord(ppCur, pdwLeft, pqwValue, 1);
}

int C2_GetWLenBuf(const void **ppCur, uint32_t *pdwLeft, void *pBuf, uint16_t *pwBufLen)
{
	return getWLenBuf(ppCur, pdwLeft, pBuf, pwBufLen, 1);
}

int C2_GetWLenBufPtr(const void **ppCur, uint32_t *pdwLeft, const void **ppBuf, uint16_t *pwBufLen)
{
	return getWLenBufPtr(ppCur, pdwLeft, ppBuf, pwBufLen, 1);
}

int C2_AddWordH(void **ppCur, uint32_t *pdwLeft, uint16_t wValue)
{
	return addWord(ppCur, pdwLeft, wValue, 0);
}

int C2_AddDWordH(void **ppCur, uint32_t *pdwLeft, uint32_t dwValue)
{
	return addDWord(ppCur, pdwLeft, dwValue, 0);
}

int C2_AddQWordH(void **ppCur, uint32_t *pdwLeft, uint64_t qwValue)
{
	return addQWord(ppCur, pdwLeft, qwValue, 0);
}

int C2_AddWLenBufH(void **ppCur, uint32_t *pdwLeft, const void *pBuf, uint32_t dwBufLen)
{
	return addWLenBuf(ppCur, pdwLeft, pBuf, dwBufLen, 0);
}

int C2_GetWordH(const void **ppCur, uint32_t *pdwLeft, uint16_t *pwValue)
{
	return getWord(ppCur, pdwLeft, pwValue, 0);
}

int C2_GetDWordH(const void **ppCur, uint32_t *pdwLeft, uint32_t *pdwValue)
{
	return getDWord(ppCur, pdwLeft, pdwValue, 0);
}

int C2_GetQWordH(const void **ppCur, uint32_t *pdwLeft, uint64_t *pqwValue)
{
	return getQWord(ppCur, pdwLeft, pqwValue, 0);
}

int C2_GetWLenBufH(const void **ppCur, uint32_t *pdwLeft, void *pBuf, uint16_t *pwBufLen)
{
	return getWLenBuf(ppCur, pdwLeft, pBuf, pwBufLen, 0);
}

int C2_GetWLenBufPtrH(const void **ppCur, uint32_t *pdwLeft, const void **ppBuf, uint16_t *pwBufLen)
{
	return getWLenBufPtr(ppCur, pdwLeft, ppBuf, pwBufLen, 0);
}

