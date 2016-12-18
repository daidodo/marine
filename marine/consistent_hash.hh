/*
 * Copyright (c) 2016 Zhao DAI <daidodo@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or any
 * later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see accompanying file LICENSE.txt
 * or <http://www.gnu.org/licenses/>.
 */

/**
 * @file
 * @brief [Consistent Hashng](https://en.wikipedia.org/wiki/Consistent_hashing) algorithm
 * implementation.
 * @author Zhao DAI
 */

#ifndef DOZERG_CONSISTENT_HASH_H_20131127
#define DOZERG_CONSISTENT_HASH_H_20131127

#include <stdint.h>
#include <cassert>
#include <map>
#include <vector>
#include "template.hh"  //CHashFn

NS_SERVER_BEGIN

/**
 * @brief A lightweight implementation of [Consistent Hashng]
 * (https://en.wikipedia.org/wiki/Consistent_hashing) algorithm.
 * Consistent Hashing is a technique to map a large number of keys to limited amount of values.
 * @n Consider a common use case below:
 * @n You have 10 servers to serve for 1 million users, you need to decide which user is served by
 * which server. CConsistentHash can do the mapping easily:
 *   @li Each user's ID is a key for CConsistentHash;
 *   @li Integer 1 to 10 are values denoting each server;
 *   @li Firstly you set weight for every value, which could mean the capability of each server, for
 *   example, like this:
 *   @code{.cpp}
 *   CConsistentHash<UserId> mapping;
 *   mapping.setValue(1, 1000);
 *   mapping.setValue(2, 1500);
 *   mapping.setValue(3, 500);
 *   // ...
 *   @endcode
 *   @li Then for every user request, get which server it should go to, like this:
 *   @code{.cpp} uint32_t serverId = mapping.hash(userId); @endcode
 *
 * The real power of consistent hashing algorithm is that, when you want to add or remove a server
 * from your servers pool, only a fraction of your users will be redirected to a different server,
 * which could make the impact as small as possible. CConsistentHash makes this operation even
 * easier by using one API @ref setValue only:
 *   @li Set a positive weight for a new value to add a new server;
 *   @li Change the weight of an existing value to adjust the capability of the server;
 *   @li Set weight to 0 for an existing value to remove the server;
 * @tparam Key Type of keys
 * @tparam HashKey Hash function for @c Key
 */
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
    /**
     * @brief Get consistent hash result of a key.
     * @param key A key
     * @return An integer denoting the hash result of @c key
     */
    value_type hash(const key_type & key) const{
        assert(!ring_.empty());
        const value_type h = hasher()(key);
        typename __Map::const_iterator wh = ring_.lower_bound(h);
        if(ring_.end() == wh)
            wh = ring_.begin();
        return wh->second;
    }
    /**
     * @brief Add, remove or modify weight of a value.
     * @param value An integer denoting a value
     * @param weight New weight of @c value. If it's @c 0, @c value will be removed
     */
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
#ifdef UNIT_TEST
    // These are test APIs.
public:
    void actualWeight(std::vector<std::pair<value_type, value_type> > & results) const{
        assert(!ring_.empty());
        value_type last = 0;
        for(typename __Map::const_iterator it = ring_.begin();it != ring_.end();++it){
            addWeight(it->second, it->first - last, results);
            last = it->first;
        }
        addWeight(ring_.begin()->second, value_type(-1) - last, results);
    }
private:
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
#endif
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

