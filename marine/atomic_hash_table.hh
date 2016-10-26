#ifndef DOZERG_ATOMIC_HASH_TABLE_H_20130812
#define DOZERG_ATOMIC_HASH_TABLE_H_20130812

/*
    可多线程操作的多阶Hash表
    多线程说明：对于同一个key，支持单线程写，多线程读；对于不同的key，支持多线程读写
    数据说明：支持定长key+变长value，总长度最大16MB
        CAtomicHashTable        多阶hash表
*/

#include <vector>
#include "tools/debug.hh"
#include "impl/atomic_hash_table_impl.hh"

NS_SERVER_BEGIN

//Key: key类型，必须是POD或者C struct类型
//HashKey: 至少实现以下成员函数
//      size_t operator ()(const key_type & key) const; //计算key对应的hash值
//EqualKey: 至少实现以下成员函数
//      bool operator ()(const key_type & key1, const key_type & key2) const;   //判断2个key是否相等
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
    static const size_t kValueLenMax = (1UL << 24);  //key+value的最大字节长度
    //functions:
    //根据valueLen, capacity和row计算需要的内存字节大小
    //valueLen: value的估计字节长度
    //capacity: 需要容纳的元素个数
    //row: hash表的行数
    //return:
    //  0       失败
    //  其他    需要的内存字节大小
    static size_t CalcBufSize(size_t capacity, int row, size_t valueLen){
        if(__Head::Check(capacity, row)){
            std::vector<uint32_t> cols;
            size_t realCapa = tools::PrimesGenerator(capacity, row, cols);
            if(realCapa >= capacity)
                return bufSize(row, realCapa, sizeof(key_type) + alignLen(valueLen));
        }
        return 0;
    }
    //构造/初始化
    //buf: 数据内存，由调用者保证有效性
    //sz: buf的字节大小
    //capacity: 需要容纳的元素个数
    //valueLen: value的估计字节长度(<16MB)
    //row: hash表的行数
    //create:
    //  true    创建hash表，清空旧数据
    //  false   使用已有hash表，检查buf内容的有效性
    CAtomicHashTable():head_(NULL){}
    CAtomicHashTable(char * buf, size_t sz)
        : head_(NULL)
    {
        init(buf, sz);
    }
    CAtomicHashTable(char * buf, size_t sz, size_t capacity, int row, size_t valueLen, bool create = false)
        : head_(NULL)
    {
        init(buf, sz, capacity, row, valueLen, create);
    }
    //return:
    //  true    成功
    //  false   失败
    bool init(char * buf, size_t sz){
        return initAux(buf, sz, false, 0, 0, 0);
    }
    bool init(char * buf, size_t sz, size_t capacity, int row, size_t valueLen, bool create = false){
        valueLen = alignLen(valueLen);
        if(create)
            return this->create(buf, sz, capacity, row, valueLen);
        return initAux(buf, sz, true, capacity, row, valueLen);
    }
    //重置当前对象，hash表数据不受影响
    void uninit(){
        head_ = NULL;
        rows_.clear();
    }
    //是否初始化
    bool valid() const{return (NULL != head_ && !rows_.empty());}
    //获取行数
    int rowSize() const{return (head_ ? head_->row() : 0);}
    //获取每行可容纳的元素个数
    size_t capacityOfRow(int row) const{return rows_[row].capacity();}
    //获取可容纳的元素总数
    size_t capacity() const{return (head_ ? head_->realCapa() : 0);}
    //获取每行已使用的元素个数
    size_t sizeOfRow(int row) const{return rows_[row].used();}
    //获取已使用的元素总数
    size_t size() const{
        size_t ret = 0;
        for(int i = 0;i < rowSize();++i)
            ret += sizeOfRow(i);
        return ret;
    }
    //是否为空
    bool empty() const{
        for(int i = 0;i < rowSize();++i)
            if(0 != sizeOfRow(i))
                return false;
        return true;
    }
    //获取创建时间
    time_t createTime() const{return (head_ ? head_->createTime() : 0);}
    //获取最近修改时间
    time_t updateTime() const{return (head_ ? head_->modTime() : 0);}
    //获取最近升级时间
    time_t upgradeTime() const{return (head_ ? head_->upgradeTime() : 0);}
    //输出hash表可读描述
    std::string toString() const{
        CToString oss;
        oss<<"{\n"
            <<"  head="<<tools::ToStringPtr(head_)<<'\n';
        for(int i = 0;i < rowSize();++i)
            oss<<"  row["<<i<<"]="<<rows_[i].toString(head_->usedArray())<<'\n';
        oss<<"}";
        return oss.str();
    }
    //增加key和value，如果key已存在，可能导致重复key
    //len: value的字节长度
    //return:
    //  true    成功
    //  false   失败
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
    bool insert(const key_type & key, const std::string & value){
        return this->insert(key, value.c_str(), value.length());
    }
    //读取key对应的value
    //len: 传入value缓冲区的字节长度，传出实际数据的字节长度
    //return:
    //  true    找到key
    //  false   失败，或者未找到key
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
    //查找key是否存在
    //return:
    //  true    找到key
    //  false   失败，或者未找到key
    bool has(const key_type & key) const{
        if(!valid())
            return false;
        return (NULL != searchNode(hasher()(key), key));
    }
    //修改key对应的value，如果key不存在，则新增
    //len: value的字节长度
    //return:
    //  true    成功
    //  false   失败
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
    bool update(const key_type & key, const std::string & value){
        return this->update(key, value.c_str(), value.length());
    }
    //删除key和对应的数据
    //return: 实际释放的节点数
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
    //清除所有key和数据，只支持单线程
    void clear(){
        if(valid())
            for(__Iter it = rows_.begin();it != rows_.end();++it)
                it->clear();
    }
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
    mutable __Rows rows_;   //每行的信息
    __Head * head_;         //hash表头
};

NS_SERVER_END

#endif

