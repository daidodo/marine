#include <marine/ring_buf.hh>

#include "../inc.h"

static bool testRingBuf()
{
    CRingBuf<std::string> ring(100);
    if(ring.capacity() != 100){
        cerr<<"ring.capacity()="<<ring.capacity()<<" is not 100\n";
        return false;
    }
    std::string s;
    for(int i = 0;i < 100;++i){
        s.push_back(i % 26 + 'a');
        if(!ring.push(s)){
            cerr<<"ring.push() element "<<i<<" failed\n";
            return false;
        }
    }
    if(ring.size() != 100){
        cerr<<"ring.size()="<<ring.size()<<" is not 100\n";
        return false;
    }
    if(ring.push(s)){
        cerr<<"ring.push() succ when full\n";
        return false;
    }
    s.clear();
    for(int i = 0;i < 100;++i){
        std::string v;
        if(!ring.pop(&v)){
            cerr<<"ring.pop() element "<<i<<" failed\n";
            return false;
        }
        s.push_back(i % 26 + 'a');
        if(s != v){
            cerr<<"ring.pop() element "<<i<<" is equal to '"<<v<<"' but '"<<s<<"'\n";
            return false;
        }
    }
    return true;
    if(!ring.empty()){
        cerr<<"ring.empty() is false\n";
        return false;
    }
    if(ring.pop(&s)){
        cerr<<"ring.pop() succ when empty\n";
        return false;
    }
    return true;
}

TEST(CRingBuf, all)
{
    EXPECT_TRUE(testRingBuf());
}
