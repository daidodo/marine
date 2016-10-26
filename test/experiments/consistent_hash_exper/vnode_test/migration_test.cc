#include <cstdlib>
#include <cmath>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>

#include <marine/consistent_hash.hh>
#include "sv_consistent_hash.h"

using namespace std;
using namespace marine;

int g_type = 0;

const int kWeightMax = 10000;

typedef std::vector<NodeInfo> __Nodes;
typedef std::vector<std::pair<uint32_t, uint32_t> >   __Ret;
typedef std::vector<double> __Balance;

struct CMigrationRet
{
    size_t diff_;
    double perc_;
    double ratio_;
    void standarize(size_t sum, size_t w){
        assert(w);
        const double f = double(sum) / w / uint32_t(-1);
        perc_ = double(diff_) * 100 / uint32_t(-1);
        ratio_ = perc_ * sum / w / 100;
    }
    std::string toString() const{
        std::ostringstream oss;
        oss<<"{"<<diff_
            <<", "<<perc_<<'%'
            <<", "<<ratio_
            <<"}";
        return oss.str();
    }
};

static void printUsage(const char * exe)
{
    cerr<<"Usage: "<<exe<<" NODE [type]\n"
        <<"  type    'rm' for remove a node, default\n"
        <<"          'add' for add a node\n"
        <<"          'old' for remove + add a node\n";
}

static void genNodes(size_t node, __Nodes & ret)
{
    ret.resize(node);
    for(size_t i = 0;i < ret.size();++i){
        ret[i].dwNodeId = i;
        ret[i].dwWeight = ::random() % kWeightMax + 1;
    }
}

static size_t migrationDiff(const __Ret & r1, const __Ret & r2)
{
    size_t ret = 0;
    size_t i = 0, j = 0;
    uint32_t last = 0;
    while(i < r1.size() && j < r2.size()){
        __Ret::const_reference a = r1[i];
        __Ret::const_reference b = r2[j];
        if(a.first < b.first){
            if(a.second != b.second)
                ret += a.first - last;
            last = a.first;
            ++i;
        }else if(a.first > b.first){
            if(a.second != b.second)
                ret += b.first - last;
            last = b.first;
            ++j;
        }else{
            if(a.second != b.second)
                ret += a.first - last;
            last = a.first;
            ++i;
            ++j;
        }
    }
    if(i < r1.size()){
        while(i < r1.size()){
            __Ret::const_reference a = r1[i];
            __Ret::const_reference b = r2[0];
            if(a.second != b.second)
                ret += a.first - last;
            last = a.first;
            ++i;
        }
    }else if(j < r2.size()){
        while(j < r2.size()){
            __Ret::const_reference a = r1[0];
            __Ret::const_reference b = r2[j];
            if(a.second != b.second)
                ret += b.first - last;
            last = b.first;
            ++j;
        }
    }
    if(r1[0].second != r2[0].second)
        ret += uint32_t(-1) - last;
    return ret;
}

static void calcMigration(const __Ret & old, const __Ret & rm, const __Ret & add, CMigrationRet & mr)
{
    switch(g_type){
        case 1:
            mr.diff_ = migrationDiff(rm, add);
            break;
        case 2:
            mr.diff_ = migrationDiff(old, add);
            break;
        default:
            mr.diff_ = migrationDiff(old, rm);
    }
}

static void marineCHMigration(const __Nodes & nodes, CMigrationRet & mr)
{
    __Ret old, rm, add;
    size_t sum = 0, w = 0;
    {
        CConsistentHash<uint32_t> h;
        for(size_t i = 0;i < nodes.size();++i){
            h.setValue(nodes[i].dwNodeId, nodes[i].dwWeight);
            sum += nodes[i].dwWeight;
        }
        h.vnodes(old);
        w = nodes.back().dwWeight;
        h.setValue(nodes.back().dwNodeId, 0);
        h.vnodes(rm);
        h.setValue(nodes.back().dwNodeId, w);
        h.vnodes(add);
    }
    calcMigration(old, rm, add, mr);
    mr.standarize(sum, w);
}

static void getVnodes(const ConsistentHashRing & ring, __Ret & ret)
{
    ret.reserve(ring.dwVnodesNum);
    for(uint32_t i = 0;i < ring.dwVnodesNum;++i)
        ret.push_back(std::make_pair(ring.pstVnodes[i].dwRingPos, ring.pstVnodes[i].dwNodeId));
}

static void svCHMigration(const __Nodes & nodes, CMigrationRet & mr)
{
    __Ret old, rm, add;
    size_t sum = 0, w = 0;
    {
        ConsistentHashRing ring;
        ConsistentHashInit(&ring);
        ConsistentHashReload(&ring, &nodes[0], nodes.size());
        getVnodes(ring, old);
        w = nodes.back().dwWeight;
        ConsistentHashReload(&ring, &nodes[0], nodes.size() - 1);
        getVnodes(ring, rm);
        ConsistentHashReload(&ring, &nodes[0], nodes.size());
        getVnodes(ring, add);
        ConsistentHashFree(&ring);
    }
    calcMigration(old, rm, add, mr);
    for(size_t i = 0;i < nodes.size();++i)
        sum += nodes[i].dwWeight;
    mr.standarize(sum, w);
}

static void printNodes(const __Nodes & nodes)
{
    cout<<"{";
    for(size_t i = 0;i < nodes.size();++i){
        if(i)
            cout<<", ";
        cout<<"["<<nodes[i].dwNodeId
            <<", "<<nodes[i].dwWeight
            <<"]";
    }
    cout<<"}";
}

static void migrationCompare(const __Nodes & nodes)
{
    CMigrationRet mret, svret;
    marineCHMigration(nodes, mret);
    cout<<"\t"<<mret.ratio_<<std::flush;
    svCHMigration(nodes, svret);
    cout<<"\t"<<svret.ratio_;
    cout<<"\t"<<mret.toString()
        <<"\t"<<svret.toString();
    if(mret.ratio_ > 10 || svret.ratio_ > 10){
        cout<<"\t";
        printNodes(nodes);
    }
    cout<<"\n";
}

static void migrationCompare(int cnt)
{
    ::srandom(time(NULL));
    __Nodes nodes;
    genNodes(cnt, nodes);
    migrationCompare(nodes);
}


int main(int argc, const char ** argv)
{
    int cnt = 0;
    std::string t;
    if(argc > 2){
        if(std::string("add") == argv[2])
            g_type = 1;
        else if(std::string("old") == argv[2])
            g_type = 2;
    }
    if(argc > 1)
        cnt = ::atoi(argv[1]);
    if(!cnt){
        printUsage(argv[0]);
        return 1;
    }
    migrationCompare(cnt);
    return 0;
}
