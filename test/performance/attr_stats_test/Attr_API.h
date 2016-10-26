/*
 * =====================================================================================
 *
 *       Filename:  Attr_API.h
 *
 *    Description:  ��ǿ��������ϱ�api
 *					1) ��֤ԭ�Ӳ���
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
#ifndef ATTR_API_H
#define ATTR_API_H

#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

	// �ۼ��ϱ���api
	int Attr_API(int iAttrId, int iValue);

	// �ۼӲ������ϱ�ֵ��api
	int Attr_API_EX(int iAttrId, int iValue, int *piCurValue);

	// �����ϱ���api
	int Attr_API_Set(int iAttrId, int iValue);

	// ��ȡ�ϱ�ֵ��api
	int Attr_API_Get(int iAttrId, int *piValue);

	// �ϱ��ַ���
	int Attr_API_Alarm(int iAttrId, int iLen, char *pvalue);

	// δʵ��, �°治֧�ָ�api
	int AttrProfile(int iId, int iAttrId1, int iAttrId2);

	int setNumAttrWithIP(const char* szIP, int iAttrId, int iValue);

	int setStrAttrWithIP(const char* strIP, int iAttrID, size_t len , char* pval);

#ifdef __cplusplus
}
#endif

#endif
