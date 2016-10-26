/*
 * =====================================================================================
 *
 *       Filename:  CAS_64bit.h
 *
 *    Description:  CAS对64bit字长进行操作
 *
 *        Version:  1.0
 *        Created:  03/06/2013 11:05:53 AM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  sunnyhao
 *        Company:  Tencent
 *
 * =====================================================================================
 */
#ifndef __CAS_64BIT_H__
#define __CAS_64BIT_H__

#include <stdint.h>

#if defined(__x86_64__)
#define CAS_64bit(p,o,n) __sync_bool_compare_and_swap((uint64_t*)(p), *(uint64_t*)(&o), *(uint64_t*)(&n))
#else
#define CAS_64bit(p,o,n) CAS2_IMPL((uint64_t*)(p), (uint32_t*)(&o), (uint32_t*)(&n))

#ifndef __cplusplus
typedef _Bool bool;
#endif                                          /* __cplusplus */

#ifndef __PIC__                                 /* 编译选项中加-fPIC */
inline
bool CAS2_IMPL(uint64_t* addr, 
        uint32_t* old_value, 
        uint32_t* new_value) 
{ 
    bool ret; 
    __asm__ __volatile__( 
            "lock cmpxchg8b %1;\n" 
            "sete %0;\n" 
            :"=m"(ret),"+m" (*(volatile uint64_t *) (addr)) 
            :"a" (old_value[0]), "d" (old_value[1]), "b" (new_value[0]), "c" (new_value[1])
			:"memory"); 
    return ret; 
}
#else
__attribute__ ((noinline))                      /* 禁止编译器进行内联优化, 否则会有问题 */
bool CAS2_IMPL(uint64_t* addr, 
        uint32_t* old_value, 
        uint32_t* new_value) 
{ 
    bool ret; 
    __asm__ __volatile__(                       /* 因为PIC参数下, 需要用%%ebx存储段地址, 因此在汇编指令中需要特殊处理 */
            "pushl %%ebx;"
            "movl %4, %%ebx;"
            "lock cmpxchg8b %1;\n" 
            "sete %0;\n" 
            "popl %%ebx;\n" 
            :"=m"(ret),"+m" (*(volatile uint64_t *) (addr)) 
            :"a" (old_value[0]), "d" (old_value[1]), "m" (new_value[0]), "c" (new_value[1])
			:"memory"); 
    return ret; 
}
#endif                                          /* __PIC__ */

#endif                                          /* __x86_64__ */

#endif                                          /* __CAS_64BIT_H__ */
