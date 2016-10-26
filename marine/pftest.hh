#ifndef DOZERG_PFTEST_H_20130619
#define DOZERG_PFTEST_H_20130619

/*
    通用性能测试框架
        PFINIT      定义全局初始化代码
        PFUNINIT    定义全局结束代码
        PFINIT_T    定义测试用例的初始化代码
        PFUNINIT_T  定义测试用例的结束代码
        PFTEST      定义测试用例的测试代码
        PFASSERT    检查表达式的值
//*/

#include "impl/pftest_impl.hh"

//#define __TIME    //enable speed test
//#define __GPROF   //enable gprof test
//#define __GPERF   //enable gperftools CPU test
//#define __PERF    //enable perf test

//定义全局初始化/结束代码
#define PFINIT()    __PF_GLOBAL_INIT(true)
#define PFUNINIT()  __PF_GLOBAL_INIT(false)

//定义测试用例的初始化/结束代码
#define PFINIT_T(case_name, test_name)      __PF_TEST_INIT(case_name, test_name, true)
#define PFUNINIT_T(case_name, test_name)    __PF_TEST_INIT(case_name, test_name, false)

//定义测试用例的测试代码
//pf_thread_index: 保留的变量名，表示线程索引，从0开始
//pf_thread_count: 保留的变量名，表示线程总数
//pf_thread_finish: 保留的变量名，表示线程是否应该结束
#define PFTEST(case_name, test_name)            __PFTEST(case_name, test_name)

//检查表达式的值，并可以输出错误信息
//expr:
//  true    通过，继续执行
//  false   不通过，输出错误信息，中断执行
//示例:
//  int x = 0;
//  PFASSERT(x == 0)<<"x="<<x<<" is NOT 0!";
#define PFASSERT(expr)    __PFASSERT(expr)

#endif

