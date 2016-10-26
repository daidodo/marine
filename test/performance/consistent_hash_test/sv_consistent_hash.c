/* 
 * Copyright (c) 2013, Tencent.com, All rights reserved.
 * mikewei@tencent.com
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "sv_consistent_hash.h"
#include "sv_md5.h"

#define VNODES_PER_NODE (1500)
#define VNODES_MAX (VNODES_PER_NODE * 10000)  /* max limit for check*/
#define USE_RADIX_SORT

#if 0
#	define DBG(fmt, ...) do { \
	printf(fmt , ##__VA_ARGS__); \
} while (0)
#else
#	define DBG(fmt, ...)
#endif

#define ALIGNED_SIZE(size, align) (((size) + (align) - 1) / (align) * (align))

#define Hash32bit Hash32bit_Simple

static inline uint32_t Hash32bit_Simple(uint32_t dwVal)
{
	// borrowed from stackoverflow.com
	dwVal = ((dwVal >> 16) ^ dwVal) * 0x45d9f3b;
	dwVal = ((dwVal >> 16) ^ dwVal) * 0x45d9f3b;
	dwVal = ((dwVal >> 16) ^ dwVal);
	return dwVal;
}

static inline uint32_t Hash32bit_Md5(uint32_t dwVal)
{
	uint32_t adwBuf[4];
	Md5HashBuffer(adwBuf, &dwVal, sizeof(dwVal));
	return (adwBuf[0] ^ adwBuf[1]) + (adwBuf[2] ^ adwBuf[3]);
}

static inline uint32_t KeyToPos(uint32_t dwKey)
{
	return Hash32bit(dwKey + 0x434977);
}

static inline uint32_t NodeToVnodePos(uint32_t dwNodeIdOrLastPos, uint32_t dwNum)
{
	return Hash32bit(dwNodeIdOrLastPos + dwNum);
}

#ifdef USE_RADIX_SORT
/* RadixSort is about x4 faster than qsort */
static void RadixSort(ConsistentHashRingVnode *pstVnodes, uint32_t dwVnodesNum)
{
	int64_t i;
	uint32_t awBucket[0x10000]; /* 256KB stack space */
	ConsistentHashRingVnode *pstTmpVnodes = malloc(ALIGNED_SIZE(dwVnodesNum * sizeof(ConsistentHashRingVnode), 4096));
	// step 1
	memset(awBucket, 0, sizeof(awBucket));
	for (i = 0; i < dwVnodesNum; i++) {
		awBucket[pstVnodes[i].dwRingPos & 0xffff]++;
	}
	for (i = 1; i < 0x10000; i++) {
		awBucket[i] += awBucket[i - 1];
	}
	for (i = dwVnodesNum - 1; i >= 0; i--) {
		pstTmpVnodes[--awBucket[pstVnodes[i].dwRingPos & 0xffff]] = pstVnodes[i];
	}
	// step 2
	memset(awBucket, 0, sizeof(awBucket));
	for (i = 0; i < dwVnodesNum; i++) {
		awBucket[pstTmpVnodes[i].dwRingPos >> 16]++;
	}
	for (i = 1; i < 0x10000; i++) {
		awBucket[i] += awBucket[i - 1];
	}
	for (i = dwVnodesNum - 1; i >= 0; i--) {
		pstVnodes[--awBucket[pstTmpVnodes[i].dwRingPos >> 16]] = pstTmpVnodes[i];
	}
	free(pstTmpVnodes);
}
#else
static int VnodeCompare(const void *vnode1, const void *vnode2)
{
#define POS(vnode) (((const ConsistentHashRingVnode*)(vnode))->dwRingPos)
#define NODE(vnode) (((const ConsistentHashRingVnode*)(vnode))->dwNodeId)
	if (POS(vnode1) > POS(vnode2)) return 1;
	else if (POS(vnode1) < POS(vnode2)) return -1;
	/* when RingPos hash-collision happens, ordered by NodeId for consistency
	 * we will remove duplicate RingPos and we want to give priority to higher NodeId
	 * (higher NodeId overload lower one), so here HigherNodeId < LowerNodeId */
	else if (NODE(vnode1) > NODE(vnode2)) return -1;
	else if (NODE(vnode1) < NODE(vnode2)) return 1;
	else return 0;
}
#endif

int ConsistentHashInit(ConsistentHashRing *pstRing)
{
	memset(pstRing, 0, sizeof(ConsistentHashRing));
	return 0;
}

int ConsistentHashReload(ConsistentHashRing *pstRing, const NodeInfo *pstNodes, uint32_t dwNodesNum)
{
	uint32_t i, j, dwVnodesCount = 0;
	uint32_t dwLastRingPos = 0;
	size_t nMallocSize = 0;
	uint64_t qwTotalWeight = 0;
	uint8_t sMd5[16];

	/* fast path: check md5 */
	Md5HashBuffer(sMd5, pstNodes, sizeof(NodeInfo) * dwNodesNum);
	if (pstRing->pstVnodes) {
		if (memcmp(sMd5, pstRing->sNodesMd5, 16) == 0) {
			DBG("MD5 is same, reload done");
			return 0;
		} else {
			ConsistentHashFree(pstRing);
			ConsistentHashInit(pstRing);
		}
	}
	/* alloc vnodes array */
	pstRing->dwVnodesMax = (dwNodesNum + 2) * VNODES_PER_NODE;
	if (pstRing->dwVnodesMax > VNODES_MAX)
		return -1;
	nMallocSize = ALIGNED_SIZE(pstRing->dwVnodesMax * sizeof(ConsistentHashRingVnode), 4096);
	DBG("alloc %u vnodes slots (%lu Bytes) for %u nodes\n", pstRing->dwVnodesMax, nMallocSize, dwNodesNum);
	pstRing->pstVnodes = malloc(nMallocSize);
	/* calc total weight */
	for (i = 0; i < dwNodesNum; i++) {
		qwTotalWeight += pstNodes[i].dwWeight;
	}

	/* map Nodes to VNodes */
	for (i = 0; i < dwNodesNum; i++) {
		uint32_t dwNodeIdOrLastVnode = pstNodes[i].dwNodeId;
		/* vnodes/total_vnodes == weight/total_weight */
		uint32_t dwVnodesNum = (uint64_t)dwNodesNum * VNODES_PER_NODE * pstNodes[i].dwWeight / qwTotalWeight;
		if (dwVnodesNum == 0)
			dwVnodesNum = 1;
		DBG("Node[%u] has %u vnodes\n", i, dwVnodesNum);
		for (j = 0;  j < dwVnodesNum; j++) {
			uint32_t dwNewVnode = NodeToVnodePos(dwNodeIdOrLastVnode, j);
			dwNodeIdOrLastVnode = dwNewVnode;
			if (dwVnodesCount >= pstRing->dwVnodesMax) {
				return -2;
			}
			pstRing->pstVnodes[dwVnodesCount].dwRingPos = dwNewVnode;
			pstRing->pstVnodes[dwVnodesCount].dwNodeId = pstNodes[i].dwNodeId;
			dwVnodesCount++;
		}
	}
	/* sort Vnodes for binary search */
#ifdef USE_RADIX_SORT
	RadixSort(pstRing->pstVnodes, dwVnodesCount);
#else
	qsort(pstRing->pstVnodes, dwVnodesCount, sizeof(ConsistentHashRingVnode), VnodeCompare);
#endif
	/* remove duplicate Vnodes (with same RingPos) - low probability */
	for (i = 0, j = 0; i < dwVnodesCount; i++) { /* j is write-pointer here */
		if (i > 0 && pstRing->pstVnodes[i].dwRingPos == dwLastRingPos) {
			DBG("dup vnode at %u server %u\n", pstRing->pstVnodes[i].dwRingPos, pstRing->pstVnodes[i].dwNodeId);
			continue;
		}
		dwLastRingPos = pstRing->pstVnodes[i].dwRingPos;
		if (i != j) {
			pstRing->pstVnodes[j] = pstRing->pstVnodes[i];
		}
		j++;
		DBG("vnode[%u] at %u server %u\n", j, pstRing->pstVnodes[i].dwRingPos, pstRing->pstVnodes[i].dwNodeId);
	}
	pstRing->dwVnodesNum = j;
	/* update MD5 at last */
	memcpy(pstRing->sNodesMd5, sMd5, 16);
	return 0;
}

static int64_t BinarySearchRing(const ConsistentHashRingVnode *pstVnodes, uint32_t dwVnodesNum, uint32_t dwKey)
{
#define VAL(idx) (pstVnodes[(idx)].dwRingPos)
	uint32_t dwLeft = 0, dwRight = dwVnodesNum - 1;
	if (dwVnodesNum == 0)
		return -1;
	if (dwKey > VAL(dwVnodesNum - 1))
		return 0; /* wrap around */
	while (dwLeft < dwRight) {
		uint32_t dwMiddle = (dwLeft + dwRight) / 2;
		if (dwKey < VAL(dwMiddle)) 
			dwRight = dwMiddle;
		else if (dwKey > VAL(dwMiddle))
			dwLeft = dwMiddle + 1;
		else
			return dwMiddle;
	}
	return dwRight;
}

int ConsistentHashMap(const ConsistentHashRing *pstRing, uint32_t dwKey, uint32_t *pdwNodeId)
{
	uint32_t dwKeyPos = KeyToPos(dwKey);
	int64_t r = BinarySearchRing(pstRing->pstVnodes, pstRing->dwVnodesNum, dwKeyPos);
	if (r < 0)
		return -1;
	*pdwNodeId = pstRing->pstVnodes[r].dwNodeId;
	return 0;
}

void ConsistentHashFree(ConsistentHashRing *pstRing)
{
	if (pstRing->pstVnodes) {
		free(pstRing->pstVnodes);
		pstRing->pstVnodes = NULL;
	}
}

