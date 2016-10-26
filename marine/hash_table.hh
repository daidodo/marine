#ifndef DOZERG_HASH_TABLE_H_20130327
#define DOZERG_HASH_TABLE_H_20130327

/*
    多阶Hash表，线程不安全
        CMultiRowHashTable      多阶hash表，支持定长key+value
        CMultiRowHashUpgrade    多阶hash表升级辅助类
    TODO:
        增加create/upgrade seq，避免访问过程中hash表被意外create或upgrade
*/

#include <algorithm>        //std::copy, std::sort
#include <functional>       //std::greater, std::equal_to
#include <cstring>
#include "tools/other.hh"   //PrimesGenerator
#include "tools/debug.hh"   //ToStringPtr
#include "impl/hash_table_impl.hh"

NS_SERVER_BEGIN

class CMultiRowHashUpgrade
{
public:
    explicit CMultiRowHashUpgrade(size_t capacity = 0, int row = 0)
        : capa_(capacity)
        , sz_(0)
        , row_(row)
    {}
    //设置/获取容量
    void capacity(size_t capa){capa_ = capa;}
    size_t capacity() const{return capa_;}
    //设置/获取行数
    void row(int r){row_ = r;}
    int row() const{return row_;}
    //设置/获取数据内存字节大小
    void setBufSize(size_t sz){sz_ = sz;}
    size_t bufSize() const{return sz_;}
    //设置head
    void setHeadBuf(std::vector<char> & buf){buf_.swap(buf);}
    //修改hash表
    void upgrade(char * buf) const{
        assert(buf && !buf_.empty());
        std::copy(buf_.begin(), buf_.end(), buf);
    }
private:
    std::vector<char> buf_;
    size_t capa_;
    size_t sz_;
    int row_;
};

//Value: 元素类型，必须是POD或者C struct类型
//KeyOfValue: 至少实现以下成员
//      typedef key_type result_type;       //key的类型
//      key_type & operator ()(value_type & v) const;               //从元素中取出key
//      const key_type & operator()(const value_type & v) const;    //从元素中取出key
//EmptyKey: 至少实现以下成员函数
//      bool isEmpty(const key_type & key) const;   //判断key是否未使用
//      void resetKey(key_type * key) const;        //将key设置成未使用
//HashKey: 至少实现以下成员函数
//      size_t operator ()(const key_type & key) const; //计算key对应的hash值
//EqualKey: 至少实现以下成员函数
//      bool operator ()(const key_type & key1, const key_type & key2) const;   //判断2个key是否相等
template<
    typename Value,
    class EmptyKey,
    class KeyOfValue = CIdentity<Value>,
    template<typename>class HashKey = CHashFn,
    template<typename>class EqualKey = std::equal_to
>class CMultiRowHashTable
{
private:
    //typedefs
    typedef CMultiRowHashTable<Value, EmptyKey, KeyOfValue, HashKey, EqualKey> __Myt;
    typedef NS_IMPL::CMultiRowHashHead          __Head;
public:
    typedef Value                               value_type;
    typedef value_type *                        pointer;
    typedef const value_type *                  const_pointer;
    typedef value_type &                        reference;
    typedef const value_type &                  const_reference;
    typedef size_t                              size_type;
    typedef ptrdiff_t                           difference_type;
    typedef KeyOfValue                          extract_key;
    typedef typename extract_key::result_type   key_type;
    typedef EmptyKey                            key_empty;
    typedef HashKey<
        typename COmitCV<key_type>::result_type>    hasher;
    typedef EqualKey<key_type>                  key_equal;
    typedef NS_IMPL::CMultiRowHashConstIterator<__Myt>  const_iterator;
    typedef NS_IMPL::CMultiRowHashIterator<__Myt>       iterator;
private:
    typedef typename const_iterator::__Rows     __Rows;
    typedef typename const_iterator::__RowInfo  __RowInfo;
    typedef typename const_iterator::__Iter     __Iter;
public:
    //constants
    static const uint16_t kCurVersion = 0;  //当前version
    static const int kRowMax = 256;         //允许的最多行数
    //根据capacity和row计算需要的内存字节大小
    //capacity: 需要容纳的元素个数
    //row: hash表的行数
    //return:
    //  0       失败
    //  其他    需要的内存字节大小
    static size_t CalcBufSize(size_t capacity, int row){
        if(check(capacity, row)){
            std::vector<uint32_t> cols;
            size_t realCapa = tools::PrimesGenerator(capacity, row, cols);
            if(realCapa >= capacity)
                return bufSize(row, realCapa);
        }
        return 0;
    }
    //判断value的key是否未使用
    static bool isEmptyKey(const_reference value){
        return key_empty().isEmpty(extract_key()(value));
    }
    //构造/初始化
    //buf: 数据内存，由调用者保证有效性
    //sz: buf的字节大小
    //capacity: 需要容纳的元素个数
    //row: hash表的行数
    //create:
    //  true    创建hash表，清空旧数据
    //  false   使用已有hash表，检查buf内容的有效性
    CMultiRowHashTable()
        : head_(NULL)
    {}
    CMultiRowHashTable(char * buf, size_t sz)
        : head_(NULL)
    {
        init(buf, sz);
    }
    CMultiRowHashTable(char * buf, size_t sz, size_t capacity, int row, bool create = false)
        : head_(NULL)
    {
        init(buf, sz, capacity, row, create);
    }
    //return:
    //  true    成功
    //  false   失败
    bool init(char * buf, size_t sz){
        return initAux(buf, sz, false, 0, 0);
    }
    bool init(char * buf, size_t sz, size_t capacity, int row, bool create = false){
        if(create)
            return this->create(buf, sz, capacity, row);
        return initAux(buf, sz, true, capacity, row);
    }
    //重置当前对象，hash表数据不受影响
    void uninit(){
        head_ = NULL;
        rows_.clear();
    }
    //是否初始化
    bool valid() const{return (NULL != head_ && !rows_.empty());}
    //获取行数
    int rowSize() const{return (head_ ? head_->row_ : 0);}
    //获取每行可容纳的元素个数
    size_t capacityOfRow(int row) const{return rows_[row].capacity();}
    //获取可容纳的元素总数
    size_t capacity() const{return (head_ ? head_->realCapa_ : 0);}
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
    time_t createTime() const{return (head_ ? head_->creatTime_ : 0);}
    //获取最近修改时间
    time_t updateTime() const{return (head_ ? head_->modTime_ : 0);}
    //获取最近升级时间
    time_t upgradeTime() const{return (head_ ? head_->upgradeTime_ : 0);}
    //输出hash表可读描述
    std::string toString(){
        CToString oss;
        oss<<"{\n"
            <<"  head="<<tools::ToStringPtr(head_)<<'\n';
        for(int i = 0;i < rowSize();++i)
            oss<<"  row["<<i<<"]="<<rows_[i].toString(usedArray(head_))<<'\n';
        oss<<"}";
        return oss.str();
    }
    //搜索元素
    //return:
    //  end()   没有找到
    //  其他    元素对应的迭代器
    iterator find(const key_type & key){
        __Iter it;
        uint32_t index;
        if(findAux(key, it, index, false))
            return iterator(it, index);
        return end();
    }
    const_iterator find(const key_type & key) const{
        __Iter it;
        uint32_t index;
        if(findAux(key, it, index, false))
            return const_iterator(it, index);
        return end();
    }
    //插入元素
    //return:
    //  end()   插入失败
    //  其他    插入成功，元素对应的迭代器
    iterator insert(const_reference value){
        __Iter it;
        uint32_t index;
        if(!findAux(extract_key()(value), it, index, true))
            return end();
        it->setValue(index, value);
        update();
        return iterator(it, index);
    }
    //删除元素
    //key: 要删除的元素key
    //it: 要删除的iterator
    //return: 实际删除的元素个数
    size_type erase(const key_type & key){
        __Iter it;
        uint32_t index;
        if(!findAux(key, it, index, false))
            return 0;
        it->resetValue(index);
        update();
        return 1;
    }
    void erase(iterator it){
        it.resetValue();
        update();
    }
    //清空hash表
    void clear(){
        for(__Iter it = rows_.begin();it != rows_.end();++it)
            it->clear();
        update();
    }
    //获取迭代器
    iterator begin(){return iterator(rows_.begin(), 0);}
    iterator end(){return iterator(rows_.end(), 0);}
    const_iterator begin() const{return const_iterator(rows_.begin(), 0);}
    const_iterator end() const{return const_iterator(rows_.end(), 0);}

    //----hash表升级流程----
    //1. 设置升级后的容量和行数
    //  CMultiRowHashUpgrade up(newCapa, newRow);
    //2. 模拟升级
    //  if(!ht.upgradeCalc(up))
    //      cerr<<"ht cannot upgrade to newCapa and newRow!";
    //3. 伸缩数据内存(伪码)
    //  if(!expandOrShrink(buf, up.bufSize()))
    //      cerr<<"expand or shrink buf to newSize failed!";
    //4. 真正升级
    //  ht.upgradeCommit(buf, sz, up);
    //---------完成---------

    //将当前hash表模拟升级，不改变hash表数据
    //up: 升级的所有参数和计算结果，详见class CMultiRowHashUpgrade;
    //return:
    //  false   失败
    //  true    成功
    bool upgradeCalc(CMultiRowHashUpgrade & up) const{
        //check
        if(!valid())
            return false;
        const size_t newCapa = up.capacity();
        int row = up.row();
        if(!check(newCapa, row))
            return false;
        //calc new cols
        const uint32_t * const colsp = colsArray(head_);
        std::vector<uint32_t> cols(colsp, colsp + rowSize());
        size_t realCapa = capacity();
        assert(colsp);
        if(newCapa <= realCapa){    //shrink
            for(;cols.size() > size_t(row);cols.pop_back()){
                assert(!cols.empty());
                if(realCapa < newCapa + cols.back())
                    break;
                realCapa -= cols.back();
            }
            assert(!cols.empty());
        }else{  //expand
            realCapa = tools::PrimesGenerator(newCapa, row, cols);
            if(realCapa < newCapa)
                return false;
        }
        //calc new head
        //head
        row = cols.size();
        std::vector<char> headBuf(headSize(row));
        __Head & head = *reinterpret_cast<__Head *>(&headBuf[0]);
        initHead(head, row, up.capacity(), realCapa, false);
        head.creatTime_ = createTime();
        head.modTime_ = updateTime();
        //used
        const uint32_t * const oldUsed = usedArray(head_);
        std::copy(oldUsed, oldUsed + kRowMax, usedArray(&head));
        //cols
        std::copy(cols.begin(), cols.end(), colsArray(&head));
        //save head
        up.setBufSize(bufSize(row, realCapa));
        up.setHeadBuf(headBuf);
        return true;
    }
    //将当前hash表真正升级
    //buf: 升级后hash表的数据内存，调用者保证有效性和字节大小
    //up: 升级的所有参数和计算结果，必须是经过upgradeCalc()处理的
    void upgradeCommit(char * buf, const CMultiRowHashUpgrade & up){
        up.upgrade(buf);
        uninit();
        init(buf, up.bufSize());
    }
private:
    static bool check(size_t capacity, int row){return (0 < row && row <= kRowMax && size_t(row) <= capacity);}
    static bool check(const __Head & head){
        return (kCurVersion == head.version_
                && sizeof(value_type) == head.valueSz_
                && check(head.capacity_, head.row_)
                && head.capacity_ <= head.realCapa_
                && 0 < head.creatTime_
                && head.modTime_ >= head.creatTime_);
    }
    static bool check(const __Head & head, size_t capacity, int row){
        return (capacity == head.capacity_ && row == head.row_);
    }
    static bool check(const uint32_t * cols, const uint32_t * used, const __Head & head){
        assert(cols && used);
        size_t total = 0;
        for(uint16_t i = 0;i < head.row_;++i){
            if(cols[i] < used[i])
                return false;
            total += cols[i];
        }
        return (total == head.realCapa_);
    }
    static size_t headSize(int row){return 4096;}
    static size_t bufSize(int row, size_t realCapa){return (headSize(row) + sizeof(value_type) * realCapa);}
    static uint32_t * usedArray(__Head * head){
        assert(head);
        return reinterpret_cast<uint32_t *>(head + 1);
    }
    static const uint32_t * usedArray(const __Head * head){
        assert(head);
        return reinterpret_cast<const uint32_t *>(head + 1);
    }
    static uint32_t * colsArray(__Head * head){
        assert(head);
        return (usedArray(head) + kRowMax);
    }
    static const uint32_t * colsArray(const __Head * head){
        assert(head);
        return (usedArray(head) + kRowMax);
    }
    static pointer dataArray(__Head * head){
        assert(head);
        return reinterpret_cast<pointer>(reinterpret_cast<char *>(head) + headSize(head->row_));
    }
    static void initHead(__Head & head, int row, size_t capacity, size_t realCapa, bool create){
        head.version_ = kCurVersion;
        head.row_ = row;
        head.valueSz_ = sizeof(value_type);
        head.capacity_ = capacity;
        head.realCapa_ = realCapa;
        if(create)
            head.modTime_ = head.creatTime_ = tools::Time(NULL);
        else
            head.upgradeTime_ = tools::Time(NULL);
    }
    bool initAux(char * buf, size_t sz, bool hasCR, size_t capacity, int row){
        //check
        if(valid())
            return false;
        if(NULL == buf || sz < sizeof(__Head))
            return false;
        __Head & head = *reinterpret_cast<__Head *>(buf);
        if(!check(head))
            return false;
        if(hasCR && !check(head, capacity, row))
            return false;
        if(sz < bufSize(head.row_, head.realCapa_))
            return false;
        uint32_t * const used = usedArray(&head);
        uint32_t * const cols = colsArray(&head);
        if(!check(cols, used, head))
            return false;
        //set
        head_ = &head;
        initRows(cols, used);
        return true;
    }
    bool create(char * buf, size_t sz, size_t capacity, int row){
        //check
        if(valid())
            return false;
        if(NULL == buf || sz < sizeof(__Head))
            return false;
        if(!check(capacity, row))
            return false;
        __Head & head = *reinterpret_cast<__Head *>(buf);
        std::vector<uint32_t> cols;
        const uint64_t realCapa = tools::PrimesGenerator(capacity, row, cols);
        if(realCapa < capacity || size_t(row) != cols.size())
            return false;
        if(sz < bufSize(row, realCapa))
            return false;
        //set
        memset(buf, 0, sz);
        //head
        head_ = &head;
        initHead(head, row, capacity, realCapa, true);
        //used
        uint32_t * const used = usedArray(head_);
        //cols
        uint32_t * const colp = colsArray(head_);
        std::sort(cols.begin(), cols.end(), std::greater<uint32_t>());
        std::copy(cols.begin(), cols.end(), colp);
        //rows
        initRows(colp, used);
        return true;
    }
    void initRows(uint32_t * cols, uint32_t * used){
        assert(cols && used && head_ && rows_.empty());
        pointer p = dataArray(head_);
        rows_.reserve(head_->row_);
        for(uint16_t i = 0;i < head_->row_;++i){
            rows_.push_back(__RowInfo(cols[i], &used[i], p));
            p += cols[i];
        }
        std::sort(rows_.begin(), rows_.end(), std::greater<__RowInfo>());
    }
    bool findAux(const key_type & key, __Iter & it, uint32_t & index, bool forInsert) const{
        if(!valid())
            return false;
        key_empty em;
        if(em.isEmpty(key))
            return false;
        uint32_t hash = hasher()(key);
        extract_key keyOf;
        key_equal equal;
        for(it = rows_.begin();it != rows_.end();++it){
            index = it->indexOf(hash);
            if(forInsert){
                if(em.isEmpty(keyOf(it->valueOf(index))))
                    return true;
            }else{
                if(equal(key, keyOf(it->valueOf(index))))
                    return true;
            }
        }
        return false;
    }
    void update(){
        if(head_)
            head_->update();
    }
    //fields
    mutable __Rows rows_;   //每行的信息
    __Head * head_;         //hash表头
};

NS_SERVER_END

#endif

