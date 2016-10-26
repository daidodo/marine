#include <marine/freq_control.hh>

#include "../inc.h"

TEST(CFreqControl, all)
{
    int FREQ, GET, startTime, token;

    FREQ = 200;
    GET = 10;
    token = 0;
    CFreqControl d(FREQ, 100);
    startTime = time(NULL);
    while(token < 1000){
        d.generate();
        if(d.token() > 0){
            ASSERT_TRUE(d.overdraft(GET));
            token += GET;
        }else{
            ASSERT_FALSE(d.token() > 0);
        }
        usleep(10);
    }
    startTime = time(NULL) - startTime;
    ASSERT_TRUE(startTime > 0)<<"startTime="<<startTime<<endl;
    ASSERT_EQ(FREQ, token / startTime);

    CFreqControl c;
    FREQ = 100;
    GET = 4;
    token = 0;
    c.init(FREQ, FREQ);
    startTime = time(NULL);
    while(token < 500){
        c.generate(tools::MonoTimeUs());
        if(c.check(GET)){
            ASSERT_TRUE(c.token() >= GET);
            ASSERT_TRUE(c.get(GET));
            token += GET;
        }else{
            ASSERT_FALSE(c.token() >= GET);
        }
        usleep(10);
    }
    startTime = time(NULL) - startTime;
    ASSERT_TRUE(startTime > 0)<<"startTime="<<startTime<<endl;
    EXPECT_EQ(FREQ, token / startTime);

    FREQ = 200;
    GET = 1;
    token = 0;
    c.init(FREQ, FREQ);
    startTime = time(NULL);
    while(token < 1000){
        c.generate();
        if(c.check(GET)){
            ASSERT_TRUE(c.token() >= GET);
            ASSERT_TRUE(c.get());   //GET = 1
            token += GET;
        }else{
            ASSERT_FALSE(c.token() >= GET);
        }
        usleep(10);
    }
    startTime = time(NULL) - startTime;
    ASSERT_TRUE(startTime > 0)<<"startTime="<<startTime<<endl;
    EXPECT_EQ(FREQ, token / startTime);

}
