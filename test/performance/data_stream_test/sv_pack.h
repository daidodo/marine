#ifndef _SV_PACK_H
#define _SV_PACK_H

#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>

//����/���� ��������

// ppCur: ����/���뻺����
// pdwLeft: ������ʣ��ռ��С�����Ϊ0����ʾ���Ա߽���

int C2_AddByte(void **ppCur, uint32_t *pdwLeft, uint8_t bValue);
int C2_AddWord(void **ppCur, uint32_t *pdwLeft, uint16_t wValue);
int C2_AddDWord(void **ppCur, uint32_t *pdwLeft, uint32_t dwValue);
int C2_AddQWord(void **ppCur, uint32_t *pdwLeft, uint64_t qwValue);

//ֱ�ӽ�pBuf���ݷ��뻺����
int C2_AddBuf(void **ppCur, uint32_t *pdwLeft, const void *pBuf, uint32_t dwBufLen);

//�Ƚ�����dwBufLen��2�ֽڱ������������Ȼ�����pBuf����
int C2_AddWLenBuf(void **ppCur, uint32_t *pdwLeft, const void *pBuf, uint32_t dwBufLen);

int C2_GetByte(const void **ppCur, uint32_t *pdwLeft, uint8_t *pbValue);
int C2_GetWord(const void **ppCur, uint32_t *pdwLeft, uint16_t *pwValue);
int C2_GetDWord(const void **ppCur, uint32_t *pdwLeft, uint32_t *pdwValue);
int C2_GetQWord(const void **ppCur, uint32_t *pdwLeft, uint64_t *pqwValue);

//ֱ�Ӵӻ�������ȡ���ݵ�pBuf
//���pBufΪ0����ֻ�Ǹı�pdwLeft������������
int C2_GetBuf(const void **ppCur, uint32_t *pdwLeft, void *pBuf, uint32_t dwBufLen);

// pwBufLen: pBuf��������ԭʼ���ȣ������ջ�ȡ���ݵ�ʵ�ʳ���
//�Ƚ���2�ֽڵ����ݳ��ȣ�����pwBufLen��Ȼ���ȡ���ݵ�pBuf
//���pBuf��pwBufLenΪ0����ֻ�Ƕ�ȡ���Ⱥ͸ı�pdwLeft������������
int C2_GetWLenBuf(const void **ppCur, uint32_t *pdwLeft, void *pBuf, uint16_t *pwBufLen);

// ppBuf: ָ����������ָ�룬����C2_GetWLenBuf���ڴ渴��
// pwBufLen: ppBuf��������ʵ�ʳ���
//�Ƚ���2�ֽڵ����ݳ��ȣ��������pwBufLen��Ȼ����������ָ�����ppBuf�����轫�ڴ渴�Ƴ���
int C2_GetWLenBufPtr(const void **ppCur, uint32_t *pdwLeft, const void **ppBuf, uint16_t *pwBufLen);


//�����Ǳ����ֽ���汾(������htonת��)

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
