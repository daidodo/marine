#define __FreqCounter   CFreqCounter<DUR, GRA>

TEST(NAME, misc)
{
    ASSERT_EQ(DUR, int(__FreqCounter::duration()));
    ASSERT_EQ(GRA, int(__FreqCounter::granularity()));
}

TEST(NAME, occur)
{
    __FreqCounter fc;
    size_t FREQ = 100;
    for(size_t i = 0;i <= FREQ;++i){
        fc.occur(i);
        fc.occur();
    }
    FREQ = FREQ * (FREQ + 1) / 2 + FREQ + 1;
    ASSERT_EQ(FREQ, fc.frequency())<<"fc="<<fc.toString();
}

TEST(NAME, frequency)
{
    size_t FREQ = 5000;
    __FreqCounter fc;
    time_t start = time(NULL);
    CFreqControl tk(FREQ, FREQ / 10);
    FREQ += 100;
    size_t FREQ_MIN = FREQ / 10 * 8;
    FREQ *= __FreqCounter::granularity();
    FREQ_MIN *= __FreqCounter::granularity();
    for(;;){
        tk.generate();
        int t = tk.token();
        if(t > 0){
            fc.occur(t);
            tk.get(t);
        }else
            usleep(1);
        t = time(NULL) - start;
        if(t > 3){
            ASSERT_GE(FREQ, fc.frequency())<<"t="<<t<<", fc="<<fc.toString();
            ASSERT_LE(FREQ_MIN, fc.frequency())<<"fc="<<fc.toString();
        }
        if(t > SLP)
            break;
    }
    ASSERT_GE(FREQ, fc.frequency())<<"fc="<<fc.toString();
    ASSERT_LE(FREQ_MIN, fc.frequency())<<"fc="<<fc.toString();
}

#undef DUR
#undef GRA
#undef SLP
#undef NAME
#undef __FreqCounter
