#include "../inc.h"
#include <marine/tools/debug.hh>

static std::string text(1 << 20, 'a');

template<class BufType, class CompType>
static bool testCompBuf(const char * compName, const CompType & comp)
{
    assert(compName);
    BufType origin(text.begin(), text.end());
    BufType encry;
    bool i = comp.compress(origin, encry);
    if(!i){
        cerr<<compName<<"::compress(origin) returns "<<i<<endl;
        return false;
    }
    BufType decry;
    i = comp.decompress(encry, decry);
    if(!i){
        cerr<<compName<<"::decompress(encry="<<tools::Dump(encry)<<") returns "<<i<<endl;
        return false;
    }
    if(origin != decry){
        cerr<<compName<<": origin="<<tools::Dump(origin)<<" is diff from decry="<<tools::Dump(decry)<<endl;
        return false;
    }
    BufType encry2;
    i = comp.compress(decry, encry2);
    if(!i){
        cerr<<compName<<"::compress(decry="<<tools::Dump(decry)<<") returns "<<i<<endl;
        return false;
    }
    if(encry != encry2){
        cerr<<compName<<": encry="<<tools::Dump(encry)<<" is diff from encry2="<<tools::Dump(encry2)<<endl;
        return false;
    }
    return true;
}

template<class CompType>
static bool testComp(const char * compName, const CompType & comp)
{
    if(!testCompBuf<std::string>(compName, comp))
        return false;
    if(!testCompBuf<std::vector<char> >(compName, comp))
        return false;
    if(!testCompBuf<std::vector<signed char> >(compName, comp))
        return false;
    if(!testCompBuf<std::vector<unsigned char> >(compName, comp))
        return false;
    return true;
}

