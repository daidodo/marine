#include <marine/threads.hh>

#include "../inc.h"

typedef CLockInt<int>   __Int;
typedef CLockQueue<int> __Que;

class CTest : public CThreadManager<int>
{
    typedef CThreadManager<int> __MyBase;
public:
    int v() const{return v_.load();}
protected:
    void run(int & task){
        v_ += task;
        ::usleep(10000);
    }
private:
    __Int v_;
};

TEST(CThreadManager, all)
{
    const unsigned int t_min = 3;
    const unsigned int t_max = 10;
    const int t_sched = 100;
    __Que que;

    CTest worker;
    //threadCountMin
    ASSERT_EQ(1, (int)worker.threadCountMin());
    worker.threadCountMin(t_min);
    ASSERT_EQ(t_min, worker.threadCountMin());
    //threadCountMax
    ASSERT_EQ(0, (int)worker.threadCountMax());
    worker.threadCountMax(t_max);
    ASSERT_EQ(t_max, worker.threadCountMax());
    //scheduleInterval
    ASSERT_EQ(500, worker.scheduleInterval());
    worker.scheduleInterval(t_sched);
    ASSERT_EQ(t_sched, worker.scheduleInterval());
    //start
    ASSERT_EQ(0, worker.v());
    const unsigned int count = 9;
    ASSERT_EQ(int(count), worker.startThreads(que, count, (16 << 10)));
    ASSERT_EQ(-1, worker.startThreads(que));
    ::sleep(1);
    ASSERT_EQ(count, worker.runningCount());
    ASSERT_EQ(0, (int)worker.activeCount());
    ASSERT_FALSE(worker.stopped());
    ASSERT_NE("", worker.toString());
    //feed slowly
    for(int i = 0;i < 20;++i){
        que.push(0);
        ::usleep(100000);
        if(count >= t_min + i)
            ASSERT_EQ(count - i, worker.runningCount())<<"i="<<i;
        else
            ASSERT_EQ(t_min, worker.runningCount())<<"i="<<i;
    }
    unsigned int max = 0;
    worker.runningCount(&max);
    ASSERT_EQ(count, max);
    max = 0;
    worker.activeCount(&max);
    ASSERT_EQ(1, (int)max);
    ASSERT_FALSE(worker.stopped());
    //feed fast
    __Que::container_type c;
    const int sum = 1000;
    for(int i = 1;i <= sum;++i)
        c.push_back(i);
    que.pushAll(c);
    ::sleep(2);
    max = 0;
    unsigned int cnt = worker.runningCount(&max);
    ASSERT_TRUE(t_min <= cnt && cnt <= t_max);
    ASSERT_EQ(t_max, max);
    max = 0;
    cnt = worker.activeCount(&max);
    ASSERT_EQ(0, (int)cnt);
    ASSERT_EQ(t_max, max);
    ASSERT_FALSE(worker.stopped());
    //test sum
    ASSERT_EQ(sum * (sum + 1) / 2, worker.v());
    //stop
    worker.stopThreads();
    ::sleep(1);
    ASSERT_TRUE(worker.stopped());
}
