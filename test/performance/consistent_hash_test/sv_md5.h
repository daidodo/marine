//#ifndef _SV_MD5_H //和comm库的名字起冲突了,导致被include两次,故使用与comm库同一个宏
//#define _SV_MD5_H
#ifndef _INCLUDED_OICQCRYPT_H_
#define _INCLUDED_OICQCRYPT_H_

#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>

#ifndef MD5LEN
#define MD5LEN          16
#endif

void Md5HashBuffer(void *pOutBuffer, const void *pInBuffer, uint32_t dwLength);

//#endif // _SV_MD5_H

#ifdef __cplusplus
}
#endif

#endif // _INCLUDED_OICQCRYPT_H_
