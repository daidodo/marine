#include "comm.h"

static bool isPalindromes(const std::string & str, size_t s, size_t e)
{
    assert(s < e && e < str.size());
    do{
        if(str[s] != str[e])
            return false;
    }while(++s < --e);
    return true;
}

//O(N^3) solution
inline int countPalindromesN3(const std::string & str)
{
    int c = str.size();
    if(c > 1){
        for(size_t i = 0;i < str.size() - 1;++i)
            for(size_t j = i + 1;j <= str.size() - 1;++j)
                if(isPalindromes(str, i, j))
                    ++c;
    }
    return c;
}

PFTEST(test, test)
{
    g_count += countPalindromesN3(g_str);
}
