#include "comm.h"

//O(N) solution
std::string prepare(const std::string & str)
{
    assert(!str.empty());
    std::string ret(str.size() * 2 - 1, 0);
    for(size_t i = 0, j = 0;i < str.size() && j < ret.size();++i){
        assert('#' != str[i]);
        ret[j++] = str[i];
        if(j < ret.size())
            ret[j++] = '#';
    }
    return ret;
}

int countPalindromesN(const std::string & str)
{
    int c = str.size();
    if(c > 1){
        std::string s = prepare(str);
        std::vector<size_t> cnt(s.size());
        size_t center = 1;
        for(size_t i = 1;i < s.size();++i){
            if(center < i && i <= center + cnt[center]){
                assert(2 * center >= i);
                size_t ii = 2 * center - i;
                if(ii - cnt[ii] != center - cnt[center]){
                    cnt[i] = std::min(cnt[ii], center + cnt[center] - i);
                    continue;
                }
                cnt[i] = center + cnt[center] - i;
            }
            while(i > cnt[i] && i + cnt[i] + 1 < s.size() && s[i - cnt[i] - 1] == s[i + cnt[i] + 1])
                ++cnt[i];
            if(i != center && i + cnt[i] > center + cnt[center])
                center = i;
        }
        for(size_t i = 0;i < cnt.size();++i)
            if(i & 1)
                c += (cnt[i] + 1) / 2;
            else
                c += cnt[i] / 2;
    }
    return c;
}

PFTEST(test, test)
{
    g_count += countPalindromesN(g_str);
}
