#include <marine/heap.hh>

#include "../inc.h"

#define MAX_VAL     100000
#define HEAP_SZ     1000

static bool testHeap()
{
    CHeap<int> heap;
    CFixedHeap<int> fixheap(HEAP_SZ / 10);
    for(int i = 0;i < HEAP_SZ;++i){
        int v = rand() % MAX_VAL;
        heap.push(v);
        fixheap.push(v);
    }
    if(heap.size() != HEAP_SZ){
        cerr<<"heap.size()="<<heap.size()<<" is not "<<HEAP_SZ<<endl;
        return false;
    }
    if(fixheap.size() != fixheap.max_size()){
        cerr<<"fixheap.size()="<<fixheap.size()<<" is not fixheap.max_size()="<<fixheap.max_size()<<endl;
        return false;
    }
    for(int prev = MAX_VAL;!heap.empty();heap.pop()){
        int cur = heap.top();
        if(prev < cur){
            cerr<<"cur="<<cur<<" > prev="<<prev<<" while heap.size()="<<heap.size()<<endl;
            return false;
        }
        if(heap.size() == fixheap.size()){
            int fcur = fixheap.top();
            if(cur != fcur){
                cerr<<"heap.top()="<<cur<<" != fixheap.top()="<<fcur<<" while fixheap.size()="<<fixheap.size()<<endl;
                return false;
            }
            fixheap.pop();
        }
        prev = cur;
    }
    return true;
}

TEST(CHeap, CFixedHeap)
{
    srand(time(0));
    EXPECT_TRUE(testHeap());
}
