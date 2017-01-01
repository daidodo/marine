#ifndef DOZERG_ATTR_STATS_IMPL_H_20130606
#define DOZERG_ATTR_STATS_IMPL_H_20130606

#include <stdint.h>
#include <utility>  //std::pair
#include <vector>
#include "../atomic_sync.hh"

NS_IMPL_BEGIN

class CAttrStats
{
    typedef std::pair<int, uint64_t>    __Attr;
    typedef CAtomicSync<int>            __AA;
    typedef CAtomicSync<uint64_t>       __AV;
    typedef std::vector<__Attr>         __AttrList;
    CAttrStats(){}
    ~CAttrStats(){}
public:
    static CAttrStats & Inst(){
        static CAttrStats inst;
        return inst;
    }
    bool init(size_t capacity){
        if(!attrs_.empty())
            return false;
        attrs_.resize(capacity);
        return true;
    }
    uint64_t * attrAdd(int attr, uint64_t val){
        uint64_t * v = findAttr(attr);
        if(v)
            attrAdd(v, val);
        return v;
    }
    static void attrAdd(uint64_t * v, uint64_t val){
        assert(v);
        __AV::add_fetch(v, val);
    }
    uint64_t * attrSet(int attr, uint64_t val, uint64_t * old = NULL){
        uint64_t * v = findAttr(attr);
        if(v)
            attrSet(v, val, old);
        return v;
    }
    static void attrSet(uint64_t * v, uint64_t val, uint64_t * old = NULL){
        assert(v);
        if(old)
            *old = __AV::swap(v, val);
        else
            __AV::store(v, val);
    }
    template<class Op>
    void iterate(Op op){
        __AttrList::iterator i = attrs_.begin();
        for(;i != attrs_.end() && i->first;++i)
            op(i->first, __AV::swap(&i->second, 0));
    }
private:
    CAttrStats(const CAttrStats &);     //disable copy and assignment
    CAttrStats & operator =(const CAttrStats &);
    uint64_t * findAttr(int attr){
        if(attr <= 0)
            return NULL;
        __AttrList::iterator i = attrs_.begin();
        for(;i != attrs_.end();++i){
            if(!i->first){
                if(__AA::compare_swap(&i->first, 0, attr, NULL))
                    return &i->second;
            }
            if(i->first == attr)
                return &i->second;
        }
        return NULL;
    }
    //fields
    __AttrList attrs_;
};

template<int N>
struct __AttrDummy;

NS_IMPL_END

#endif

