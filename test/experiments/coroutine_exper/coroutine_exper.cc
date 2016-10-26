#include <deque>
#include <iostream>

#include "coroutine.hh"

using namespace std;
using namespace marine;

namespace n1{
    typedef CCoroutine<int> __Coro;

    void func(__Coro::yield_type & yield){
        for(;;){
            cout<<yield.get()<<endl;
            yield();
        }
    }

    void test1()
    {
        __Coro a(func);
        a(1);
        a(2);
        a(3);
    }
}

namespace n2{
    typedef CCoroutine<void> __Coro;

    void func(__Coro::yield_type & yield){
        for(;;){
            cout<<"Hi"<<endl;
            yield();
        }
    }

    void test()
    {
        __Coro a(func);
        a();
        a();
        a();
    }
}

namespace n3{
    typedef CCoroutine<int &> __Coro;
    int x = 0;
    void func(__Coro::yield_type & yield){
        for(;;){
            cout<<yield.get()++<<endl;
            yield();
        }
    }

    void test()
    {
        __Coro a(func);
        a(x);
        a(x);
        a(x);
        cout<<x<<endl;
    }
}

namespace n4{
    typedef CCoroutine<void> __Coro;
    void producer(__Coro::yield_type &);
    void consumer(__Coro::yield_type &);

    __Coro p(producer), c(consumer);
    deque<int> taskQue;
    int cnt = 0;

    void producer(__Coro::yield_type & yield)
    {
        cout<<"enter producer\n";
        for(;;){
            if(taskQue.size() > 3)
                yield(c);
            taskQue.push_back(cnt++);
        }
    }

    void consumer(__Coro::yield_type & yield)
    {
        cout<<"enter consumer\n";
        for(;;){
            if(taskQue.empty())
                yield(p);
            int v = taskQue.front();
            taskQue.pop_front();
            if(0 == (v & 0x3FFFFF))
                cout<<v<<endl;
        }
    }

    void test()
    {
        c();
    }
}

void proc(CCoroutine<int &>::yield_type & yield)
{
}

int main()
{
    n1::test1();
    cout<<"---\n";
    n2::test();
    cout<<"---\n";
    n3::test();
    cout<<"---\n";
    n4::test();
    return 0;
}
