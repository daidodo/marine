//TODO: not finish
#error "this file is not finish yet!"

#ifndef DOZERG_HASH_SET_H_20130403
#define DOZERG_HASH_SET_H_20130403

#include "hash_table.hh"

NS_SERVER_BEGIN

template<
    typename T,
    class EmptyKey,
    template<typename>class HashKey = CHashFn,
    template<typename>class EqualKey = std::equal_to
>class CMultiRowHashSet
{
    typedef CMultiRowHashSet<T, EmptyKey, HashKey, EqualKey> __Myt;
    typedef CMultiRowHashTable<T, EmptyKey, CIdentity<T>, HashKey, EqualKey> __HashTable;
public:
    typedef typename __HashTable::value_type    value_type;
    typedef typename __HashTable::pointer    pointer;
    typedef typename __HashTable::const_pointer    const_pointer;
    typedef typename __HashTable::reference    reference;
    typedef typename __HashTable::const_reference    const_reference;
    typedef typename __HashTable::size_type    size_type;
    typedef typename __HashTable::difference_type    difference_type;
    typedef typename __HashTable::key_type    key_type;
    typedef EmptyKey                            key_empty;
    typedef HashKey<
        typename COmitCV<key_type>::result_type>    hasher;
    typedef EqualKey<key_type>                  key_equal;
private:
    __HashTable ht_;
};

NS_SERVER_END

#endif

