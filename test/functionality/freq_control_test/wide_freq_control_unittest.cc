#include <marine/freq_control.hh>

#include "../inc.h"

TEST(CWideFreqControl, all)
{
    {
        const double FREQ = 1 / 2.5;
        const time_t TIME = 5;
        CWideFreqControl fc(FREQ);
        time_t startTime = time(NULL);
        for(size_t token = 0;token < size_t(TIME * FREQ);)
            if(fc.get())
                ++token;
        startTime = time(NULL) - startTime;
        ASSERT_EQ(TIME, startTime);
    }{
        const double FREQ = 1000;
        const time_t TIME = 5;
        CWideFreqControl fc(FREQ);
        time_t startTime = time(NULL);
        for(size_t token = 0;token < size_t(TIME * FREQ);)
            if(fc.get())
                ++token;
        startTime = time(NULL) - startTime;
        ASSERT_EQ(TIME, startTime);
    }
}
