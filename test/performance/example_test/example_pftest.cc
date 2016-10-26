#include <iostream>

#include "../pf_inc.h"

PFINIT()
{
    cout<<"this is global init\n";
}

PFUNINIT()
{
    cout<<"this is global uninit\n";
}

//---Case 1---

//test_1
PFINIT_T(case_1, test_1)
{
    cout<<"this is init for case_1.test_1\n";
}

PFUNINIT_T(case_1, test_1)
{
    cout<<"this is uninit for case_1.test_1\n";
}

PFTEST(case_1, test_1)
{
    int x = 0;
    for(int j = 0;j < 1000;++j)
        x += time(NULL);
}

//test_2
PFINIT_T(case_1, test_2)
{
    cout<<"this is init for case_1.test_2\n";
}

PFUNINIT_T(case_1, test_2)
{
    cout<<"this is uninit for case_1.test_2\n";
}

PFTEST(case_1, test_2)
{
    usleep(100000);
}

//---Case 2---

//test_1
PFINIT_T(case_2, test_1)
{
    cout<<"this is init for case_2.test_1\n";
}

PFUNINIT_T(case_2, test_1)
{
    cout<<"this is uninit for case_2.test_1\n";
}

PFTEST(case_2, test_1)
{
    sleep(2);
}

//test_2
PFINIT_T(case_2, test_2)
{
    cout<<"this is init for case_2.test_2\n";
}

PFUNINIT_T(case_2, test_2)
{
    cout<<"this is uninit for case_2.test_2\n";
}

PFTEST(case_2, test_2)
{
}

//---Case 3---

//test_1
static int x;

PFINIT_T(case_3, test_1)
{
    cout<<"this is init for case_3.test_1\n";
}

PFUNINIT_T(case_3, test_1)
{
    cout<<"this is uninit for case_3.test_1, x="<<x<<"\n";
}

PFTEST(case_3, test_1)
{
    for(int i = 0;i <= 100;++i)
        x += i * pf_thread_count + pf_thread_index;
    if(x % 3){
        sleep(1);
    }
    if(x > 1000)
        pf_thread_finish = true;
}

//test_2
PFINIT_T(case_3, test_2)
{
    cout<<"this is init for case_3.test_2\n";
/*
    cout<<toString();
    cout<<testName();
    void * p1 = (void *)&run;
//*/
}

PFUNINIT_T(case_3, test_2)
{
    cout<<"this is uninit for case_3.test_2\n";
}

PFTEST(case_3, test_2)
{
/*
    bool a;
    test(1, 2, a);
    cout<<toString();
    cout<<testName();
    void * p1 = (void *)&run;
    void * p2 = (void *)&RegisterTest;
    void * p3 = (void *)&debug;
    cout<<seconds();
    RegisterInit(NULL, false);
    void * p4 = (void *)&RunTests;
    printUsage(NULL);
//*/

}

