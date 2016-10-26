#include "comm.h"

int countPalindromesN2(const std::string & str)
{
    int c = str.size();
    if(c > 1){
        for(size_t i = 0;i < str.size() - 1;++i){
            for(size_t j = 1;j <= i && i + j < str.size();++j)
                if(str[i - j] == str[i + j])
                    ++c;
                else
                    break;
            for(size_t j = 0;j <= i && i + j + 1 < str.size();++j)
                if(str[i - j] == str[i + 1 + j])
                    ++c;
                else
                    break;
        }
    }
    return c;
}

PFTEST(test, test)
{
    g_count += countPalindromesN2(g_str);
}
