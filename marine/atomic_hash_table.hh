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
 * @brief Lock-free and process/thread-safe hash table implementation.
 * @author Zhao DAI
 */

#ifndef DOZERG_ATOMIC_HASH_TABLE_H_20130812
#define DOZERG_ATOMIC_HASH_TABLE_H_20130812

#include <vector>
#include "tools/debug.hh"
#include "impl/atomic_hash_table_impl.hh"

NS_SERVER_BEGIN

/// A lock-free hash table that can be used in multi-thread or multi-process programs.
/// A common usage of CAtomicHashTable is for multiple threads or processes to operate on the same
/// hash table efficiently.
/// @n It must reside in continuous memory pre-allocated by the user.
/// Hence, key and value types must be [trivially copyable]
/// (http://en.cppreference.com/w/cpp/types/is_trivially_copyable), i.e. can be copied using @c
/// std::memcpy.
/// @par Multi-Thread/Process Access Safety Guide
/// |           | Same Key | Different Keys |
/// | ---       | ---      |---             |
/// | **Read**  | Safe     | Safe           |
/// | **Write** | Unsafe   | Safe           |
/// @par Multiple Rows Hash Table
/// CAtomicHashTable resolves key hash collisions by adding multiple rows. A row is a one-dimension
/// hash table.
/// @n If a key collides with another key in a row, CAtomicHashTable will search in the next row,
/// and so on until it finds a room.
/// @n The more number of rows, the less chance of failure when hash table capacity is about to
/// reach. The less number of rows, the better performance it can get.
/// @tparam Key Type of keys, must be POD or C struct compatible types
/// @tparam HashKey Hash function of @a Key, should implement:
///   @code size_t operator ()(const Key & key) const;  // compute hash value of key @endcode
/// @tparam EqualKey Equal predictor of @a Key, should implement:
///   @code
///   bool operator ()(const Key & key1, const Key & key2) const;  // predict if key1 == key2
///   @endcode
/// @par Value
/// Value type must be POD or C struct compatible types, or array of such types, with fixed or
/// variant length.
/// @note Size limitation of @a Key and @a Value is defined by @ref kValueLenMax.
template<
    typename Key,
    template<typename>class HashKey = CHashFn,
    template<typename>class EqualKey = std::equal_to
>class CAtomicHashTable
{
    typedef CAtomicHashTable<Key, HashKey, EqualKey>    __Myt;
    typedef NS_IMPL::CAtomicHashHead                    __Head;
    typedef NS_IMPL::CAtomicHashRowInfo                 __RowInfo;
    typedef __RowInfo::__Node                           __Node;
    typedef std::vector<__RowInfo>                      __Rows;
    typedef typename __Rows::iterator                   __Iter;
    typedef typename __Rows::const_iterator             __CIter;
public:
    typedef Key                 key_type;
    typedef size_t              size_type;
    typedef EqualKey<key_type>  key_equal;
    typedef HashKey<
        typename COmitCV<key_type>::result_type>    hasher;
    //constant
    /// The maximum size of @a Key plus @a Value, i.e. 16MB
    static const size_t kValueLenMax = (1UL << 24);
    //functions:
    /// Compute memory size needed for a hash table.
    /// This function pre-computes the size of pre-allocated memory buffer needed for a hash table.
    /// @param capacity The maximum number of key-value pairs the hash table wishes to hold
    /// @param row Number of rows the hash table wants to have
    /// @param valueLen Estimated byte size of key plus value, e.g. average size. This needs @em NOT
    /// to be the maximum.
    /// @return
    ///   @li Number of bytes needed for a memory buffer to hold the hash table
    ///   @li Or 0 if such requirement is unable to meet
    static size_t CalcBufSize(size_t capacity, int row, size_t valueLen){
        if(__Head::Check(capacity, row)){
            std::vector<uint32_t> cols;
            size_t realCapa = tools::PrimesGenerator(capacity, row, cols);
            if(realCapa >= capacity)
                return bufSize(row, realCapa, sizeof(key_type) + alignLen(valueLen));
        }
        return 0;
    }
    /// Prepare an uninitialized object.
    /// You cannot use the object until it is initialized by @ref init.
    CAtomicHashTable():head_(NULL){}
    /// Attach an existing hash table hosted in a memory buffer.
    /// @sa init(char * buf, size_t sz)
    CAtomicHashTable(char * buf, size_t sz)
        : head_(NULL)
    {
        init(buf, sz);
    }
    /// Attach or create a hash table in a memory buffer.
    /// @sa init(char * buf, size_t sz, size_t capacity, int row, size_t valueLen, bool create)
    CAtomicHashTable(char * buf, size_t sz, size_t capacity, int row, size_t valueLen, bool create = false)
        : head_(NULL)
    {
        init(buf, sz, capacity, row, valueLen, create);
    }
    /// Attach an existing hash table hosted in a memory buffer.
    /// There must be an initialized hash table in the memory buffer.
    /// This function tries to attach the existing hash table to a local object.
    /// If failed, it won't modify the content of the memory buffer.
    /// @param buf Pointer to the memory buffer
    /// @param sz Byte size of the memory buffer
    /// @return @c true if succeeded; otherwise @c false
    bool init(char * buf, size_t sz){
        return initAux(buf, sz, false, 0, 0, 0);
    }
    /// Attach or create a hash table in a memory buffer.
    /// @li if `create = false`, this function tries to attach an existing hash table in the
    /// memory buffer to a local object, just like init(char * buf, size_t sz), except that it will
    /// also validate @c capacity, @c row and @c valueLen parameters if they are not 0;
    /// @li if `create = true`, this function will create a new hash table in the memory buffer,
    /// and erase any data existed. In this case, @c sz should be the return value of @ref
    /// CalcBufSize, otherwise this function may fail.
    ///
    /// @param buf Pointer to the memory buffer
    /// @param sz Byte size of the memory buffer
    /// @param capacity The maximum number of key-value pairs the hash table wishes to hold
    /// @param row Number of rows the hash table wants to have
    /// @param valueLen Estimated byte size of key plus value, e.g. average size. This needs @em NOT
    /// to be the maximum.
    /// @param create @c false if attach an existing hash table; @c true if create a new one
    /// @return @c true if succeeded; otherwise @c false
    /// @sa CalcBufSize
    bool init(char * buf, size_t sz, size_t capacity, int row, size_t valueLen, bool create = false){
        valueLen = alignLen(valueLen);
        if(create)
            return this->create(buf, sz, capacity, row, valueLen);
        return initAux(buf, sz, true, capacity, row, valueLen);
    }
    /// Reset current object.
    /// This function won't affect data in the real hash table memory buffer. After reset, the local
    /// object could be reused by @ref init.
    void uninit(){
        head_ = NULL;
        rows_.clear();
    }
    /// Test if current object is inilialized.
    /// @return @c true if current object is inilialized; otherwise @c false
    bool valid() const{return (NULL != head_ && !rows_.empty());}
    /// @name Capacity
    //@{
    /// Get number of rows in hash table.
    /// @return
    ///   @li Numer of rows
    ///   @li Or 0 if current object is NOT inilialized
    int rowSize() const{return (head_ ? head_->row() : 0);}
    /// Get capacity of a row in hash table.
    /// @param row Index of row, ranging from 0 to rowSize() - 1
    /// @return Number of key-value pairs that this row can hold at most
    size_t capacityOfRow(int row) const{return rows_[row].capacity();}
    /// Get capacity of the hash table.
    /// @return
    ///   @li Number of key-value pairs that this hash table can hold at most
    ///   @li Or 0 if current object is NOT inilialized
    size_t capacity() const{return (head_ ? head_->realCapa() : 0);}
    /// Get number of key-value pairs in a row.
    /// @param row Index of row, ranging from 0 to rowSize() - 1
    /// @return Number of key-value pairs hosted in this row
    size_t sizeOfRow(int row) const{return rows_[row].used();}
    /// Get number of key-value pair in the hash table
    /// @return Number of key-value pairs in this hash table
    size_t size() const{
        size_t ret = 0;
        for(int i = 0;i < rowSize();++i)
            ret += sizeOfRow(i);
        return ret;
    }
    /// Test if the hash table is empty
    /// @return @c true if this hash table is empty; otherwise @c false
    bool empty() const{
        for(int i = 0;i < rowSize();++i)
            if(0 != sizeOfRow(i))
                return false;
        return true;
    }
    //@}
    /// @name Brief Info
    //@{
    /// Get creation time of the hash table.
    /// @return
    ///   @li Creation time of this hash table
    ///   @li Or 0 if current object is NOT inilialized
    time_t createTime() const{return (head_ ? head_->createTime() : 0);}
    /// Get latest updating time of the hash table.
    /// @return
    ///   @li Latest updating time of this hash table
    ///   @li Or 0 if current object is NOT inilialized
    time_t updateTime() const{return (head_ ? head_->modTime() : 0);}
    /// Get latest upgrading time of the hash table.
    /// @return
    ///   @li Latest upgrading time of this hash table
    ///   @li Or 0 if current object is NOT inilialized
    time_t upgradeTime() const{return (head_ ? head_->upgradeTime() : 0);}
    /// Get a description of the hash table.
    /// @return A human readable description of this hash table
    std::string toString() const{
        CToString oss;
        oss<<"{\n"
            <<"  head="<<tools::ToStringPtr(head_)<<'\n';
        for(int i = 0;i < rowSize();++i)
            oss<<"  row["<<i<<"]="<<rows_[i].toString(head_->usedArray())<<'\n';
        oss<<"}";
        return oss.str();
    }
    //@}
    /// @name Data Access
    //@{
    /// Insert a key-value pair into the hash table.
    /// If @c key already exists, this function may result in multiple instances of @c key in the
    /// hash table.
    /// @param key Key to insert into the hash table
    /// @param value Pointer to bytes of value to insert into the hash table
    /// @param len Byte size of value
    /// @return @c true if succeeded; otherwise @c false
    bool insert(const key_type & key, const char * value, size_t len){
        //check
        if(!valid())
            return false;
        if(NULL == value)
            len = 0;
        if(sizeof key + len > kValueLenMax)
            return false;
        //insert
        if(!insertAux(key, hasher()(key), value, len))
            return false;
        //set meta
        head_->update();
        return true;
    }
    /// Insert a key-value pair into the hash table.
    /// If @c key already exists, this function may result in multiple instances of @c key in the
    /// hash table.
    /// @param key Key to insert into the hash table
    /// @param value Bytes of value to insert into the hash table
    /// @return @c true if succeeded; otherwise @c false
    bool insert(const key_type & key, const std::string & value){
        return this->insert(key, value.c_str(), value.length());
    }
    /// Obtain value of a key
    /// @param[in] key Key to search for
    /// @param[in] value Pointer to memory buffer that receives the value data
    /// @param[inout] len Passed in as bytes size of @c value buffer; and passed out as actual bytes
    ///   size of the data copied
    /// @return
    ///   @li @c true, if succeeded
    ///   @li @c false, otherwise, e.g. @c key doesn't exists, @c value buffer is insufficient
    bool get(const key_type & key, char * value, size_t & len) const{
        //check
        if(!valid())
            return false;
        //search first node
        const uint32_t hash = hasher()(key);
        const __Node * const p = searchNode(hash, key);
        if(NULL == p)
            return false;
        //copy data
        const size_t realLen = p->len();
        if(realLen < sizeof(key_type))
            return false;
        size_t cur = readNodes(p, value, len, hash);
        if(cur > len)
            return false;
        //check
        if(cur + sizeof(key_type) != realLen)
            return false;
        len = cur;
        return true;
    }
    /// Obtain value of a key.
    /// @param[in] key Key to search for
    /// @param[out] value String bytes to receive the value data
    /// @return @c true if succeeded; otherwise @c false
    bool get(const key_type & key, std::string & value) const{
        //check
        if(!valid())
            return false;
        //search first node
        const uint32_t hash = hasher()(key);
        const __Node * const p = searchNode(hash, key);
        if(NULL == p)
            return false;
        //reserve value
        const size_t realLen = p->len();
        if(realLen < sizeof(key_type))
            return false;
        value.resize(realLen - sizeof(key_type));
        //copy data
        size_t cur = readNodes(p, &value[0], value.size(), hash);
        return (cur == value.size());
    }
    /// Test if a key exists in the hash table
    /// @param key Key to search for
    /// @return @c true if @c key exists in this hash table; otherwise @c false
    bool has(const key_type & key) const{
        if(!valid())
            return false;
        return (NULL != searchNode(hasher()(key), key));
    }
    /// Update value of a key.
    /// If @c key doesn't exist, then insert a new key-value pair.
    /// @param key Key to search for
    /// @param value Pointer to bytes of value to update for
    /// @param len Byte size of value
    /// @return @c true if succeeded; otherwise @c false
    /// @sa insert
    bool update(const key_type & key, const char * value, size_t len){
        //check
        if(!valid())
            return false;
        if(NULL == value)
            len = 0;
        if(sizeof key + len > kValueLenMax)
            return false;
        //search old
        const uint32_t hash = hasher()(key);
        __Node * const p = searchNode(hash, key);
        //insert new
        if(!insertAux(key, hash, value, len))
            return false;
        //rm old
        removeAux(p, hash);
        //set meta
        head_->update();
        return true;
    }
    /// Update value of a key.
    /// If @c key doesn't exist, then insert a new key-value pair.
    /// @param key Key to search for
    /// @param value Bytes of value to update for
    /// @return @c true if succeeded; otherwise @c false
    /// @sa insert
    bool update(const key_type & key, const std::string & value){
        return this->update(key, value.c_str(), value.length());
    }
    /// Remove a key and its value.
    /// If there are multiple instances of @c key, only one will be removed.
    /// @param key Key to remove from this hash table
    /// @return Number of data blocks released; Or 0 if @c key doesn't exist
    size_t remove(const key_type & key){
        //check
        if(!valid())
            return false;
        //rm
        const uint32_t hash = hasher()(key);
        const size_t ret = removeAux(searchNode(hash, key), hash);
        if(ret > 0)
            head_->update();
        return ret;
    }
    /// Clear all data in the hash table.
    /// @warning This function is @em NOT thread-safe. Do @em NOT call it while other threads or
    ///   processes are accessing the hash table.
    void clear(){
        if(valid())
            for(__Iter it = rows_.begin();it != rows_.end();++it)
                it->clear();
    }
    //@}
private:
    static size_t alignLen(size_t len){return (len + 7) / 8 * 8;}
    static size_t bufSize(int row, size_t realCapa, size_t valueSz){
        return (__Head::HeadSize(row) + __Node::Offset(realCapa, valueSz));
    }
    void initRows(const uint32_t * cols, uint32_t * used){
        assert(cols && used && head_ && rows_.empty());
        char * p = head_->dataArray();
        rows_.reserve(head_->row());
        for(uint16_t i = 0;i < head_->row();++i)
            rows_.push_back(__RowInfo(cols[i], &used[i], p, head_->valueSz()));
        std::sort(rows_.begin(), rows_.end(), std::greater<__RowInfo>());
    }
    bool create(char * buf, size_t sz, size_t capacity, int row, size_t valueLen){
        //check
        if(valid())
            return false;
        if(NULL == buf || sz < sizeof(__Head))
            return false;
        if(!__Head::Check(capacity, row))
            return false;
        __Head & head = *reinterpret_cast<__Head *>(buf);
        std::vector<uint32_t> cols;
        const uint64_t realCapa = tools::PrimesGenerator(capacity, row, cols);
        if(realCapa < capacity || size_t(row) != cols.size())
            return false;
        if(sz < bufSize(row, realCapa, sizeof(key_type) + valueLen))
            return false;
        //reset head
        ::memset(buf, 0, __Head::HeadSize(row));
        //head
        head_ = &head;
        head_->init<key_type>(capacity, valueLen, row, realCapa, true);
        //used
        uint32_t * const used = head_->usedArray();
        //cols
        uint32_t * const colp = head_->colsArray();
        std::sort(cols.begin(), cols.end(), std::greater<uint32_t>());
        std::copy(cols.begin(), cols.end(), colp);
        //rows
        initRows(colp, used);
        //reset data
        clear();
        return true;
    }
    bool initAux(char * buf, size_t sz, bool hasFollowing, size_t capacity, int row, size_t valueLen){
        //check
        if(valid())
            return false;
        if(NULL == buf || sz < sizeof(__Head))
            return false;
        __Head & head = *reinterpret_cast<__Head *>(buf);
        if(!head.check<key_type>())
            return false;
        if(hasFollowing && !head.check<key_type>(capacity, valueLen, row))
            return false;
        if(sz < bufSize(head.row(), head.realCapa(), head.valueSz()))
            return false;
        uint32_t * const used = head.usedArray();
        const uint32_t * const cols = head.colsArray();
        if(!head.check(cols, used))
            return false;
        //set
        head_ = &head;
        initRows(cols, used);
        return true;
    }
    __Node * allocNode(uint32_t hash){
        assert(valid());
        for(__Iter it = rows_.begin();it != rows_.end();++it){
            __Node * const p = it->allocNode(hash);
            if(NULL != p)
                return p;
        }
        return NULL;
    }
    void allocNodes(uint32_t hash, size_t cnt, std::vector<__Node *> & ret){
        if(0 == cnt)
            return;
        ret.clear();
        ret.reserve(cnt);
        uint32_t h = hash;
        bool fail = false;
        for(size_t i = 0;i < cnt;++i, h = (h + 1) * (h + 2)){
            __Node * const p = allocNode(h);
            if(NULL == p){
                fail = true;
                break;
            }
            ret.push_back(p);
        }
        if(fail && !ret.empty()){
            deallocNodes(ret);
            ret.clear();
        }
    }
    void deallocNode(__Node * p){
        if(NULL != p && valid())
            for(__Iter it = rows_.begin();it != rows_.end() && !it->deallocNode(p);++it);
    }
    void deallocNodes(const std::vector<__Node *> & nodes){
        if(!nodes.empty())
            for(std::vector<__Node *>::const_iterator i = nodes.begin();i != nodes.end();++i)
                deallocNode(*i);
    }
    const __Node * searchNode(uint32_t hash, const key_type & key) const{
        if(!valid())
            return NULL;
        for(__CIter it = rows_.begin();it != rows_.end();++it){
            const __Node * const p = it->searchNode(hash);
            if(NULL != p && key_equal()(key, p->key<key_type>()))
                return p;
        }
        return NULL;
    }
    __Node * searchNode(uint32_t hash, const key_type & key){
        if(!valid())
            return NULL;
        for(__Iter it = rows_.begin();it != rows_.end();++it){
            __Node * const p = it->searchNode(hash);
            if(NULL != p && key_equal()(key, p->key<key_type>()))
                return p;
        }
        return NULL;
    }
    size_t readNodes(const __Node * p, char * value, size_t len, uint32_t hash) const{
        assert(p);
        size_t cur = p->getData<key_type>(value, len, head_->valueSz());
        if(cur > len)
            return -1;
        for(p = head_->nextNode(p, hash);NULL != p;p = head_->nextNode(p, hash))
            if(!p->getData(value, cur, len, head_->valueSz()))
                return -1;
        return cur;
    }
    bool insertAux(const key_type & key, uint32_t hash, const char * value, size_t len){
        assert(valid());
        //allocate nodes
        std::vector<__Node *> nodes;
        allocNodes(hash, head_->nodeCount<key_type>(len), nodes);
        if(nodes.empty())
            return false;
        //copy data
        std::vector<__Node *>::const_iterator i = nodes.begin();
        size_t cur = (*i)->setData(key, value, len, head_->valueSz());
        for(++i;i != nodes.end();++i){
            if(cur >= len){
                deallocNodes(nodes);
                return false;
            }
            (*i)->setData(value, cur, len, head_->valueSz());
        }
        //set meta
        i = nodes.begin();
        for(std::vector<__Node *>::const_iterator j = i + 1;j != nodes.end();++i, ++j)
            (*i)->setMetaMid(hash, head_->dataOffset(*j));
        (*i)->setMetaEnd(hash, value, len);
        nodes.front()->setFirst();
        return true;
    }
    size_t removeAux(__Node * p, uint32_t hash){
        assert(valid());
        size_t cnt = 0;
        for(;NULL != p;++cnt){
            __Node * const n = head_->nextNode(p, hash);
            deallocNode(p);
            p = n;
        }
        return cnt;
    }
    //fields
    mutable __Rows rows_;
    __Head * head_;         //hash table header
};

NS_SERVER_END

#endif

