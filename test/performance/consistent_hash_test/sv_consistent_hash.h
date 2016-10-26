/* 
 * Copyright (c) 2013, Tencent.com, All rights reserved.
 * mikewei@tencent.com
 */

#ifndef __SV_CONSISTENT_HASH_H__
#define __SV_CONSISTENT_HASH_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	uint32_t dwRingPos;
	uint32_t dwNodeId;
} ConsistentHashRingVnode;

typedef struct {
	ConsistentHashRingVnode *pstVnodes;
	uint32_t dwVnodesNum;
	uint32_t dwVnodesMax;
	uint8_t sNodesMd5[16];
} ConsistentHashRing;

typedef struct {
	uint32_t dwNodeId;
	uint32_t dwWeight;
} NodeInfo;

int ConsistentHashInit(ConsistentHashRing *pstRing);
int ConsistentHashReload(ConsistentHashRing *pstRing, const NodeInfo *pstNodes, uint32_t dwNodesNum);
int ConsistentHashMap(const ConsistentHashRing *pstRing, uint32_t dwKey, uint32_t *pdwNodeId);
void ConsistentHashFree(ConsistentHashRing *pstRing);

#ifdef __cplusplus
}
#endif

#endif // __SV_CONSISTENT_HASH_H__
