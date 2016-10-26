#ifndef _SV_ERRNO_H
#define _SV_ERRNO_H

#ifdef __cplusplus
extern "C" {
#endif


/*
 * ����Ŷ���
 * �����Ϊͬһ�������ռ�, ���ظ�����, ���㶨λ����
 * int�ͺ�������ֵһ�㶨��, 0��ʾ�ɹ�, ������ʾʧ��, ��ֵΪE_XXXX, E_XXXX��Ϊ�����
 * ��ģ���ѡ�����Լ��ر�Ĵ����, Ҳ����ѡ���ʹ��ͨ�õ�return E_ERR
 */

// generic
#define E_ERR				(-1)
#define E_BAD_PARAM			(-2)
#define E_CK_SOF			(-3)
#define E_OVERFLOW			(-4)
#define E_UNDERFLOW			(-5)
#define E_NOT_INIT			(-6)
#define E_IO_FAIL			(-7)
#define E_EMPTY				(-8)
#define E_FULL				(-9)
#define E_NOT_FOUND			(-10)
#define	E_INCONSISTENT		(-11)
#define E_TRUNC             (-12)
#define	E_DUPLICATE			(-13)
#define E_CONN_FAIL			(-14)
#define E_SEL_GRADE			(-15)
#define E_TIMEOUT			(-16)
#define E_TIMEIN			(-17)
#define E_OUT_OF_RANGE		(-18)
#define E_IP_ADDR_ERR		(-19)
#define E_BAD_VERSION		(-20)
#define E_BAD_APP_ID		(-21)
#define E_LOAD_FULL			(-22)
#define E_RE_INIT			(-23)

// crypt
#define E_BAD_CRYPT_ALGO	(-101)

// state
#define E_STATE_INVALID		(-201)
#define E_STATE_REDEFINE	(-202)
#define E_STATE_UNDEFINE	(-203)
#define E_STATE_MISMATCH	(-204)
#define E_NO_HANDLER		(-205)

// lock
#define E_LOCK_BUSY			(-301)

// config&sched API
#define E_SCHED_API_STATIC_USED (-401)
#define E_SCHED_

#ifdef __cplusplus
}
#endif

#endif // _SV_ERRNO_H
