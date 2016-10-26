#ifndef DOZERG_CONSISTENT_HASH_H_20131127
#define DOZERG_CONSISTENT_HASH_H_20131127

#include <stdint.h>
#include <cassert>
#include <map>
#include <vector>
#include "template.hh"  //CHashFn

NS_SERVER_BEGIN

template<
    class Key,
    template<typename>class HashKey = CHashFn
>class CConsistentHash
{
    //typedefs
    typedef CConsistentHash<Key, HashKey>   __Myt;
public:
    typedef Key                 key_type;
    typedef uint32_t            value_type;
    typedef HashKey<key_type>   hasher;
private:
    typedef std::map<value_type, value_type> __Map;      //pos -> value
    struct __Node{
        value_type last_;
        std::vector<value_type> pos_;
        size_t weight() const{return pos_.size();}
    };
    typedef std::map<value_type, __Node>     __Values;   //value -> node
public:
    //functions
    //计算key的一致性hash结果
    value_type hash(const key_type & key) const{
        assert(!ring_.empty());
        const value_type h = hasher()(key);
        typename __Map::const_iterator wh = ring_.lower_bound(h);
        if(ring_.end() == wh)
            wh = ring_.begin();
        return wh->second;
    }
    //修改hash结果集
    //value: hash结果值
    //weight: 权重，即value被hash到的概率
    //  0   相当于移除value
    void setValue(value_type value, size_t weight = 1000){
        if(weight){
            __Node & n = values_[value];
            if(n.weight() > weight){
                removeNode(value, weight, n);
            }else if(n.weight() < weight)
                insertNode(value, weight, n);
        }else{
            typename __Values::iterator wh = values_.find(value);
            if(values_.end() != wh){
                removeNode(value, 0, wh->second);
                values_.erase(wh);
            }
        }
    }
public:
    //以下是测试用接口
    void actualWeight(std::vector<std::pair<value_type, value_type> > & results) const{
        assert(!ring_.empty());
        value_type last = 0;
        for(typename __Map::const_iterator it = ring_.begin();it != ring_.end();++it){
            addWeight(it->second, it->first - last, results);
            last = it->first;
        }
        addWeight(ring_.begin()->second, value_type(-1) - last, results);
    }
    void vnodes(std::vector<std::pair<value_type, value_type> > & results) const{
        results.resize(ring_.size());
        std::copy(ring_.begin(), ring_.end(), results.begin());
    }
    static void addWeight(value_type value, value_type weight, std::vector<std::pair<value_type, value_type> > & results){
        typedef std::vector<std::pair<value_type, value_type> > __Ret;
        for(__Ret::iterator it = results.begin();it != results.end();++it)
            if(it->first == value){
                it->second += weight;
                return;
            }
        results.push_back(std::make_pair(value, weight));
    }
private:
    void insertNode(value_type value, size_t weight, __Node & n){
        while(n.weight() < weight){
            n.last_ = hashValue(value, n.weight(), n.last_);
            std::pair<typename __Map::iterator, bool> ret = ring_.insert(std::make_pair(n.last_, value));
            if(ret.second)
                n.pos_.push_back(n.last_);
        }
    }
    void removeNode(value_type value, size_t weight, __Node & n){
        while(n.weight() > weight){
            ring_.erase(n.pos_.back());
            n.pos_.pop_back();
            n.last_ = (n.pos_.empty() ? 0 : n.pos_.back());
        }
    }
    static value_type hashValue(value_type key, size_t index, value_type last){
        return u32Hash(key + last + (index << 16) + (index >> 16));
    }
    static value_type u32Hash(value_type val){
        val = ((val >> 16) ^ val) * 0x45d9f3b + 1;
        val = ((val >> 16) ^ val) * 0x45d9f3b + 3;
        val = ((val >> 16) ^ val);
        return val;
    }
    //fields
    __Values values_;
    __Map ring_;
};

NS_SERVER_END

#endif

