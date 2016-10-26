#ifndef DOZERG_PFTEST_H_20130619
#define DOZERG_PFTEST_H_20130619

/*
    ͨ�����ܲ��Կ��
        PFINIT      ����ȫ�ֳ�ʼ������
        PFUNINIT    ����ȫ�ֽ�������
        PFINIT_T    ������������ĳ�ʼ������
        PFUNINIT_T  ������������Ľ�������
        PFTEST      ������������Ĳ��Դ���
        PFASSERT    �����ʽ��ֵ
//*/

#include "impl/pftest_impl.hh"

//#define __TIME    //enable speed test
//#define __GPROF   //enable gprof test
//#define __GPERF   //enable gperftools CPU test
//#define __PERF    //enable perf test

//����ȫ�ֳ�ʼ��/��������
#define PFINIT()    __PF_GLOBAL_INIT(true)
#define PFUNINIT()  __PF_GLOBAL_INIT(false)

//������������ĳ�ʼ��/��������
#define PFINIT_T(case_name, test_name)      __PF_TEST_INIT(case_name, test_name, true)
#define PFUNINIT_T(case_name, test_name)    __PF_TEST_INIT(case_name, test_name, false)

//������������Ĳ��Դ���
//pf_thread_index: �����ı���������ʾ�߳���������0��ʼ
//pf_thread_count: �����ı���������ʾ�߳�����
//pf_thread_finish: �����ı���������ʾ�߳��Ƿ�Ӧ�ý���
#define PFTEST(case_name, test_name)            __PFTEST(case_name, test_name)

//�����ʽ��ֵ�����������������Ϣ
//expr:
//  true    ͨ��������ִ��
//  false   ��ͨ�������������Ϣ���ж�ִ��
//ʾ��:
//  int x = 0;
//  PFASSERT(x == 0)<<"x="<<x<<" is NOT 0!";
#define PFASSERT(expr)    __PFASSERT(expr)

#endif

