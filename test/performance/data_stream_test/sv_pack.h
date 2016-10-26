#ifndef _SV_PACK_H
#define _SV_PACK_H

#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>

//编码/解码 函数集合

// ppCur: 编码/解码缓冲区
// pdwLeft: 缓冲区剩余空间大小，如果为0，表示忽略边界检查

int C2_AddByte(void **ppCur, uint32_t *pdwLeft, uint8_t bValue);
int C2_AddWord(void **ppCur, uint32_t *pdwLeft, uint16_t wValue);
int C2_AddDWord(void **ppCur, uint32_t *pdwLeft, uint32_t dwValue);
int C2_AddQWord(void **ppCur, uint32_t *pdwLeft, uint64_t qwValue);

//直接将pBuf数据放入缓冲区
int C2_AddBuf(void **ppCur, uint32_t *pdwLeft, const void *pBuf, uint32_t dwBufLen);

//先将长度dwBufLen以2字节编码进缓冲区，然后放入pBuf数据
int C2_AddWLenBuf(void **ppCur, uint32_t *pdwLeft, const void *pBuf, uint32_t dwBufLen);

int C2_GetByte(const void **ppCur, uint32_t *pdwLeft, uint8_t *pbValue);
int C2_GetWord(const void **ppCur, uint32_t *pdwLeft, uint16_t *pwValue);
int C2_GetDWord(const void **ppCur, uint32_t *pdwLeft, uint32_t *pdwValue);
int C2_GetQWord(const void **ppCur, uint32_t *pdwLeft, uint64_t *pqwValue);

//直接从缓冲区读取数据到pBuf
//如果pBuf为0，则只是改变pdwLeft，不复制数据
int C2_GetBuf(const void **ppCur, uint32_t *pdwLeft, void *pBuf, uint32_t dwBufLen);

// pwBufLen: pBuf数据区的原始长度，和最终获取数据的实际长度
//先解码2字节的数据长度，存入pwBufLen，然后读取数据到pBuf
//如果pBuf或pwBufLen为0，则只是读取长度和改变pdwLeft，不复制数据
int C2_GetWLenBuf(const void **ppCur, uint32_t *pdwLeft, void *pBuf, uint16_t *pwBufLen);

// ppBuf: 指向数据区的指针，避免C2_GetWLenBuf的内存复制
// pwBufLen: ppBuf数据区的实际长度
//先解码2字节的数据长度，存入存入pwBufLen，然后将数据区的指针存入ppBuf，无需将内存复制出来
int C2_GetWLenBufPtr(const void **ppCur, uint32_t *pdwLeft, const void **ppBuf, uint16_t *pwBufLen);


//以下是本地字节序版本(不进行hton转换)

#define C2_AddByteH	C2_AddByte
int C2_AddWordH(void **ppCur, uint32_t *pdwLeft, uint16_t wValue);
int C2_AddDWordH(void **ppCur, uint32_t *pdwLeft, uint32_t dwValue);
int C2_AddQWordH(void **ppCur, uint32_t *pdwLeft, uint64_t qwValue);

#define C2_AddBufH		C2_AddBuf

int C2_AddWLenBufH(void **ppCur, uint32_t *pdwLeft, const void *pBuf, uint32_t dwBufLen);

#define C2_GetByteH	C2_GetByte
int C2_GetWordH(const void **ppCur, uint32_t *pdwLeft, uint16_t *pwValue);
int C2_GetDWordH(const void **ppCur, uint32_t *pdwLeft, uint32_t *pdwValue);
int C2_GetQWordH(const void **ppCur, uint32_t *pdwLeft, uint64_t *pqwValue);

#define C2_GetBufH		C2_GetBuf

int C2_GetWLenBufH(const void **ppCur, uint32_t *pdwLeft, void *pBuf, uint16_t *pwBufLen);

int C2_GetWLenBufPtrH(const void **ppCur, uint32_t *pdwLeft, const void **ppBuf, uint16_t *pwBufLen);


#ifdef __cplusplus
}
#endif

#endif // _SV_PACK_H
