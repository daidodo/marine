/*
 * =====================================================================================
 *
 *       Filename:  spin_lock.h
 *
 *    Description:  用户态的自旋锁
 *
 *        Version:  1.0
 *        Created:  03/26/2013 09:21:32 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  sunnyhao
 *        Company:  Tencent
 *
 * =====================================================================================
 */
#ifndef __SPIN_LOCK_H__
#define __SPIN_LOCK_H__

#include "CAS_64bit.h"
#include <sched.h>

#define SPINLOCK_INITIALIZER 0

#ifdef __cplusplus
extern "C" {
#endif

typedef volatile uint64_t spinlock_t;

static void spinlock_lock(spinlock_t* pLock)
{
	uint64_t nNonLock = SPINLOCK_INITIALIZER, nLock = 1;
	while ( !CAS_64bit((uint64_t*)pLock, nNonLock, nLock) )
	{
		sched_yield();
	}
}

static void spinlock_unlock(spinlock_t* pLock)
{
	*pLock = SPINLOCK_INITIALIZER;
}

#ifdef __cplusplus
}
#endif

#endif
