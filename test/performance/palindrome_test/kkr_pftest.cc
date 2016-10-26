#include "comm.h"

using std::string;

string preprocess(string s)
{
    if(s.length()==0)
        return "^$";
    string z = "^";
    for(int i=0;i<int(s.length());i++)
    {   z +="#";
        z = z.append(s.substr(i,1));
    }
    z+="#$";
    return z;
}

int find123(string s)
{
    string z;
    int *count = (int *)malloc(s.length() *sizeof(int)* 2 + 3*sizeof(int));
    //int L=0,
    int R=0,C=0,sum=0;
    z  = preprocess(s);
    for(int i=1;i<int(z.length());i++)
    {
        int index = 2*C-i;
        count[i] = (R>i) ? std::min(R-i,count[index]):0;
        while(i+1+count[i]<int(z.length())&&z[i+1+count[i]]==z[i-1-count[i]])
            count[i]++;
        if(i+count[i]>R)
        {
            C=i;
            R = i+count[i];
        }
        if(count[i] > 1)
            sum+=(count[i]+1)/2;
        else
            sum+=count[i];
    }
    free(count);
    return sum;
}

PFTEST(test, test)
{
    g_count += find123(g_str);
}
