/*
 * =====================================================================================
 *
 *       Filename:  Attr_API.h
 *
 *    Description:  加强版的属性上报api
 *					1) 保证原子操作
 *					2) hash方式插入共享内存, 时间复杂度降低为准O(1)
 *					3) 支持最多1w个属性的上报
 *					4) 相对于monitor而言, 保持对新旧api的兼容, 用户可以无感知的替换成新的api
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
#ifndef ATTR_API_H
#define ATTR_API_H

#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

	// 累加上报的api
	int Attr_API(int iAttrId, int iValue);

	// 累加并返回上报值的api
	int Attr_API_EX(int iAttrId, int iValue, int *piCurValue);

	// 覆盖上报的api
	int Attr_API_Set(int iAttrId, int iValue);

	// 获取上报值的api
	int Attr_API_Get(int iAttrId, int *piValue);

	// 上报字符串
	int Attr_API_Alarm(int iAttrId, int iLen, char *pvalue);

	// 未实现, 新版不支持该api
	int AttrProfile(int iId, int iAttrId1, int iAttrId2);

	int setNumAttrWithIP(const char* szIP, int iAttrId, int iValue);

	int setStrAttrWithIP(const char* strIP, int iAttrID, size_t len , char* pval);

#ifdef __cplusplus
}
#endif

#endif
