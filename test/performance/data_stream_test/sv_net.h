#ifndef __SV_NET_H__
#define __SV_NET_H__

#ifdef __cplusplus
extern "C" {
#endif


#include <net/if.h>
#include <netinet/in.h>
#include <endian.h>
#include <byteswap.h>
#include <stdint.h>

typedef struct{
	char name[IFNAMSIZ];
	struct sockaddr_in addr;
	int flags;
}IfDesc_ipv4;

int GetIf(IfDesc_ipv4 * pstDesc);
int GetIpFromIf(char szIp[16], const char * szIfName);


int C2_CreateTCPListenSocket(struct in_addr *pstListenAddr, unsigned short ushPort);
int C2_CreateTCPClientSocket(struct in_addr *pstAddr, unsigned short ushPort);
int C2_CreateUDPSocket(struct in_addr *pstAddr, unsigned short ushPort);

int C2_SockRead(int sockfd, char *buf, size_t count);

int C2_SockWrite(int sockfd, char *buf, size_t count);

int C2_IsInternalServerAddr(uint32_t dwIP);
 
//64位整数网络序与本地序转换
static inline uint64_t Hton64(uint64_t qwVal)
{
#if __BYTE_ORDER == __BIG_ENDIAN
	return qwVal;
#else
	return bswap_64(qwVal);
#endif
}

#define Ntoh64	Hton64


#ifdef __cplusplus
}
#endif

#endif
