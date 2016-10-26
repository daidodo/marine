#include <cstdlib>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <vector>

#include <marine/consistent_hash.hh>
#include "sv_consistent_hash.h"

using namespace std;
using namespace marine;

const int kWeightMax = 10000;

typedef std::vector<NodeInfo> __Nodes;
typedef std::vector<std::pair<uint32_t, uint32_t> >   __Ret;
typedef std::vector<double> __Balance;

static void printUsage(const char * exe)
{
    cerr<<"Usage: "<<exe<<" NODE\n";
}

static void genNodes(size_t node, __Nodes & ret)
{
    ret.resize(node);
    for(size_t i = 0;i < ret.size();++i){
        ret[i].dwNodeId = i;
        ret[i].dwWeight = ::random() % kWeightMax + 1;
        //ret[i].dwWeight = i + 1;
    }
}

template<class Iter>
static double StandardDeviation(Iter first, Iter last)
{
    double sum = 0;
    double sq_sum = 0;
    size_t cnt = 0;
    for(Iter i = first;i != last;++i, ++cnt){
        sum += *i;
        sq_sum += double(*i) * (*i);
    }
    if(!cnt)
        return 0;
    const double ave = sum / cnt;
    assert(ave > 0);
    return (::sqrt((sq_sum - sum * ave) / cnt) / ave);
}

static double calcBalance(const __Nodes & nodes, const __Ret & ret, size_t sum)
{
    __Balance w;
    for(size_t i = 0;i < nodes.size();++i)
        for(size_t j = 0;j < ret.size();++j)
            if(nodes[i].dwNodeId == ret[j].first){
                w.push_back(double(ret[j].second) * sum / uint32_t(-1));
                break;
            }
    if(w.size() != nodes.size())
        cerr<<"actual weight size="<<w.size()<<" is not nodes size="<<nodes.size()<<"\n";
    return StandardDeviation(w.begin(), w.end());
}

static double marineCHBalance(const __Nodes & nodes)
{
    __Ret ret;
    size_t sum = 0;
    {
        CConsistentHash<uint32_t> h;
        for(size_t i = 0;i < nodes.size();++i){
            h.setValue(nodes[i].dwNodeId, nodes[i].dwWeight);
            sum += nodes[i].dwWeight;
        }
        h.actualWeight(ret);
    }
    return calcBalance(nodes, ret, sum);
}

static void addWeight(uint32_t id, uint32_t weight, __Ret & results)
{
    for(__Ret::iterator it = results.begin();it != results.end();++it)
        if(it->first == id){
            it->second += weight;
            return;
        }
    results.push_back(std::make_pair(id, weight));
}

static double svCHBalance(const __Nodes & nodes)
{
    __Ret ret;
    {
        ConsistentHashRing ring;
        ConsistentHashInit(&ring);
        ConsistentHashReload(&ring, &nodes[0], nodes.size());
        uint32_t last = 0;
        for(uint32_t i = 0;i < ring.dwVnodesNum;++i){
            addWeight(ring.pstVnodes[i].dwNodeId, ring.pstVnodes[i].dwRingPos - last, ret);
            last = ring.pstVnodes[i].dwRingPos;
        }
        addWeight(ring.pstVnodes[0].dwNodeId, uint32_t(-1) - last, ret);
        ConsistentHashFree(&ring);
    }
    size_t sum = 0;
    for(size_t i = 0;i < nodes.size();++i)
        sum += nodes[i].dwWeight;
    return calcBalance(nodes, ret, sum);
}

static void balanceCompare(const __Nodes & nodes)
{
    const double mch = marineCHBalance(nodes);
    cout<<std::left<<"\t"<<std::setw(8)<<mch<<std::flush;
    const double svch = svCHBalance(nodes);
    cout<<"\t"<<std::setw(8)<<svch
        <<"\t"<<std::setw(8)<<(svch / mch)
        <<"\n";
}

static void balanceCompare(int cnt)
{
    ::srandom(time(NULL));
    __Nodes nodes;
    genNodes(cnt, nodes);
    balanceCompare(nodes);
}


int main(int argc, const char ** argv)
{
    int cnt = 0;
    if(argc > 1)
        cnt = ::atoi(argv[1]);
    if(!cnt){
        printUsage(argv[0]);
        return 1;
    }
    balanceCompare(cnt);
    return 0;
}
