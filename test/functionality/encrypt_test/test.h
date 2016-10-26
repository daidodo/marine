#include <marine/tools/debug.hh>
#include "../inc.h"

static std::string text("1234567890abcdefghijklmnopeaibanadj;3apU(#JDFNPDdbdn(NNEndjafefJJENFbn apfa3 j[f asdfas]a\\3a0fa sdjfna3 nadka.fj3jadshadsfa");

template<class BufType, class CompType>
static bool testCompBuf(const char * compName, const CompType & comp)
{
    assert(compName);
    BufType origin(text.begin(), text.end());
    BufType encry;
    int i = comp.encrypt(origin, encry);
    if(0 != i){
        cerr<<compName<<"::encrypt(string) returns "<<i<<endl;
        return false;
    }
    BufType decry;
    i = comp.decrypt(encry, decry);
    if(0 != i){
        cerr<<compName<<"::decrypt(encry="<<tools::Dump(&encry[0], encry.size())<<") returns "<<i<<endl;
        return false;
    }
    if(origin != decry){
        cerr<<compName<<": origin="<<tools::Dump(&origin[0],origin.size())<<" is diff from decry="<<tools::Dump(&decry[0], decry.size())<<endl;
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

