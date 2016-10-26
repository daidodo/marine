#ifndef DOZERG_ATOMIC_HASH_TABLE_IMPL_H_20130812
#define DOZERG_ATOMIC_HASH_TABLE_IMPL_H_20130812

#include <string>
#include "../template.hh"
#include "../atomic_sync.hh"
#include "../to_string.hh"
#include "../tools/other.hh"
#include "../tools/time.hh"

NS_IMPL_BEGIN

// cppcheck-suppress noConstructor
class CAtomichHashNode
{
    typedef CAtomichHashNode        __Myt;
    typedef CAtomicSync<uint8_t>    __Used;
    typedef CAtomicSync<uint32_t>   __Hash;
    static const uint8_t kUsed = 1;
    static const uint8_t kFirst = 2;
    static const uint8_t kHasNext = 4;
public:
    //functions
    static size_t Offset(size_t cnt, uint32_t valueSz){
        return (cnt * (sizeof(__Myt) + valueSz));
    }
    static size_t NodeCnt(size_t off, uint32_t valueSz){
        return (off / (sizeof(__Myt) + valueSz));
    }
    bool alloc(){
        if(!__Used::compare_swap(&used_, 0, kUsed, NULL))
            return false;
        len_ = hash_ = next_ = 0;
        return true;
    }
    void dealloc(){__Used::store(&used_, 0);}
    void clear(){used_ = 0;}
    template<typename T>
    size_t setData(const T & key, const char * value, size_t len, size_t valueSz){
        assert(valueSz >= sizeof key);
        len_ = sizeof key + len;
        ::memcpy(data_, &key, sizeof key);
        if(NULL == value || 0 == len)
            return 0;
        valueSz -= sizeof key;
        if(valueSz > len)
            valueSz = len;
        ::memcpy(data_ + sizeof key, value, valueSz);
        return valueSz;
    }
    void setData(const char * value, size_t & cur, size_t len, size_t valueSz){
        assert(value && cur < len && valueSz);
        if(cur + valueSz > len)
            valueSz = len - cur;
        ::memcpy(data_, value + cur, valueSz);
        cur += (len_ = valueSz);
    }
    void setMetaMid(uint32_t hash, size_t next){
        next_ = next;
        __Used::or_fetch(&used_, kHasNext);
        hash_ = hash;
    }
    void setMetaEnd(uint32_t hash, const char * value, size_t len){
        crc_ = Crc(value, len);
        hash_ = hash;
    }
    void setFirst(){__Used::or_fetch(&used_, kFirst);}
    bool matchFirst(uint32_t hash) const{return matchFlag(hash, kUsed | kFirst);}
    bool matchNext(uint32_t hash) const{return matchFlag(hash, kUsed);}
    template<typename T>
    const T & key() const{
        const void * const p = data_;
        return *reinterpret_cast<const T *>(p);
    }
    template<typename T>
    size_t getData(char * value, size_t len, size_t valueSz) const{
        if(len + sizeof(T) < len_)
            return -1;
        const bool hasNext = (kHasNext == (kHasNext & __Used::load(&used_)));
        if(len_ > valueSz){
            if(!hasNext)
                return -1;
        }else if(len_ < valueSz){
            if(hasNext)
                return -1;
            valueSz = len_;
        }
        if(valueSz < sizeof(T))
            return -1;
        valueSz -= sizeof(T);
        if(valueSz){
            if(NULL == value)
                return -1;
            ::memcpy(value, data_ + sizeof(T), valueSz);
            if(!hasNext && crc_ != Crc(value, valueSz))
                return -1;
        }
        return valueSz;
    }
    bool getData(char * value, size_t & cur, size_t len, size_t valueSz) const{
        if(NULL == value || cur >= len)
            return false;
        if(0 == valueSz)
            return false;
        const bool hasNext = (kHasNext == (kHasNext & __Used::load(&used_)));
        if(!hasNext){
            if(len_ <= 0 || len_ > valueSz)
                return false;
            valueSz = len_;
        }
        if(cur + valueSz > len)
            return false;
        ::memcpy(value + cur, data_, valueSz);
        cur += valueSz;
        return (hasNext || crc_ == Crc(value, cur));
    }
    uint64_t next() const{
        return (kHasNext == (kHasNext & __Used::load(&used_)) ? next_ : -1);
    }
    uint32_t len() const{return len_;}
private:
    static uint32_t Crc(const char * value, size_t len){
        return tools::Crc<uint32_t>(0, value, len);
    }
    bool matchFlag(uint32_t hash, uint8_t flag) const{
        return (flag == (flag & __Used::load(&used_))
                && hash == hash_);
    }
    //fields
    uint8_t used_;
    uint32_t len_:24;
    uint32_t hash_;
    union{
        uint64_t next_;
        uint32_t crc_;
    };
    char data_[0];
};

CHECK_TYPE_SIZE(CAtomichHashNode, 16);

/*  atomic hash table memory layout:
        Head            --- 1024 B
        used array      --- 1024 B (= 4 * 256)
        column array    --- 1024 B (= 4 * 256)
        reserved        --- 1024 B
        data            --- valueLen * capacity()
*/

// cppcheck-suppress noConstructor
struct CAtomicHashHead
{
    typedef CAtomichHashNode __Node;
private:
    typedef CAtomicSync<uint32_t> __Time;
    static const uint16_t kMagic = 0xDE11;  //标识
    static const int kRowMax = 256;         //允许的最多行数
    static const size_t kValueLenMax = (1UL << 24);  //key+value的最大字节长度
public:
    static size_t HeadSize(int row){return 4096;}
    static bool Check(size_t capacity, int row){
        return (0 < row && row <= kRowMax
                && size_t(row) <= capacity);
    }
    template<typename T>
    bool check() const{
        return (kMagic == magic_
                && valueSz_ >= sizeof(T)
                && Check(capacity_, row_)
                && capacity_ <= realCapa_
                && 0 < creatTime_
                && modTime_ >= creatTime_);
    }
    template<typename T>
    bool check(size_t capacity, size_t valueLen, int row) const{
        return (capacity == capacity_
                && valueLen + sizeof(T) == valueSz_
                && row == row_);
    }
    bool check(const uint32_t * cols, const uint32_t * used) const{
        assert(cols && used);
        size_t total = 0;
        for(uint16_t i = 0;i < row_;++i){
            if(cols[i] < used[i])
                return false;
            total += cols[i];
        }
        return (total == realCapa_);
    }
    template<typename T>
    void init(size_t capacity, size_t valueLen, int row, size_t realCapa, bool create){
        magic_ = kMagic;
        row_ = row;
        valueSz_ = sizeof(T) + valueLen;
        capacity_ = capacity;
        realCapa_ = realCapa;
        if(create)
            modTime_ = creatTime_ = tools::Time(NULL);
        else
            upgradeTime_ = tools::Time(NULL);
    }
    uint32_t * usedArray(){return reinterpret_cast<uint32_t *>(this + 1);}
    uint32_t * colsArray(){return (usedArray() + kRowMax);}
    char * dataArray(){
        return (reinterpret_cast<char *>(this) + HeadSize(row_));
    }
    const char * dataArray() const{
        return (reinterpret_cast<const char *>(this) + HeadSize(row_));
    }
    template<typename T>
    size_t nodeCount(size_t valueLen) const{
        return (valueSz_ >= sizeof(T)
                ? ((valueLen + sizeof(T) + valueSz_ - 1) / valueSz_)
                : 0);
    }
    size_t dataOffset(const __Node * p) const{
        assert(p);
        const size_t off = reinterpret_cast<const char *>(p) - dataArray();
        assert(0 == off % (sizeof(__Node) + valueSz_));
        return __Node::NodeCnt(off, valueSz_);
    }
    const __Node * nodeAtIndex(uint64_t index) const{
        if(index >= realCapa_ || valueSz_ <= 0)
            return NULL;
        return reinterpret_cast<const __Node *>(dataArray() + __Node::Offset(index, valueSz_));
    }
    __Node * nodeAtIndex(uint64_t index){
        if(index >= realCapa_ || valueSz_ <= 0)
            return NULL;
        return reinterpret_cast<__Node *>(dataArray() + __Node::Offset(index, valueSz_));
    }
    const __Node * nextNode(const __Node * p, uint32_t hash) const{
        if(NULL == p)
            return NULL;
        const __Node * const ret = nodeAtIndex(p->next());
        return (NULL != ret && ret->matchNext(hash) ? ret : NULL);
    }
    __Node * nextNode(__Node * p, uint32_t hash){
        if(NULL == p)
            return NULL;
        __Node * const ret = nodeAtIndex(p->next());
        return (NULL != ret && ret->matchNext(hash) ? ret : NULL);
    }
    void update(){modTime_ = tools::Time(NULL);}    //NOTE: not so accurate
    uint64_t realCapa() const{return realCapa_;}
    uint16_t row() const{return row_;}
    uint32_t valueSz() const{return valueSz_;}
    uint32_t createTime() const{return creatTime_;}
    uint32_t modTime() const{return modTime_;}
    uint32_t upgradeTime() const{return upgradeTime_;}
    std::string toString() const{
        CToString oss;
        oss<<"{magic_="<<Manip::hex<<magic_<<Manip::dec
            <<", row_="<<row_
            <<", valueSz_="<<valueSz_
            <<", capacity_="<<capacity_
            <<", realCapa_="<<realCapa_
            <<", creatTime_="<<creatTime_<<'('<<tools::TimeString(creatTime_)<<')'
            <<", modTime_="<<modTime_<<'('<<tools::TimeString(modTime_)<<')'
            <<", upgradeTime_="<<upgradeTime_<<'('<<tools::TimeString(upgradeTime_)<<')'
            <<"}";
        return oss.str();
    }
private:
    //fields
    uint16_t magic_;        //标识
    uint16_t row_;          //行数
    uint32_t valueSz_;      //每个key+value字节长度
    uint64_t capacity_;     //需要容纳的元素个数
    uint64_t realCapa_;     //实际可容纳元素个数
    uint32_t creatTime_;    //创建时间
    uint32_t modTime_;      //上次修改时间
    uint32_t upgradeTime_;  //上次升级时间
    char reserved_[1024 - 36];
};

CHECK_TYPE_SIZE(CAtomicHashHead, 1024);

class CAtomicHashRowInfo
{
    typedef CAtomicHashRowInfo      __Myt;
    typedef CAtomicSync<uint32_t>   __Used;
public:
    typedef CAtomicHashHead::__Node __Node;
    CAtomicHashRowInfo():data_(NULL),used_(NULL),capa_(0),valueSz_(0){}
    CAtomicHashRowInfo(uint32_t capa, uint32_t * used, char *& data, uint32_t valueSz)
        : data_(data)
        , used_(used)
        , capa_(capa)
        , valueSz_(valueSz)
    {
        data += off(capa);
    }
    bool valid() const{return (NULL != data_ && NULL != used_ && capa_ > 0 && valueSz_ > 0);}
    bool operator <(const __Myt & a) const{return (capa_ < a.capa_);}
    bool operator >(const __Myt & a) const{return a < *this;}
    uint32_t capacity() const{return capa_;}
    uint32_t used() const{return __Used::load(used_);}
    __Node * allocNode(uint32_t hash){
        assert(valid());
        __Node * const p = nodeOfHash(hash);
        if(p->alloc()){
            ++u();
            return p;
        }
        return NULL;
    }
    bool deallocNode(__Node * p){
        if(NULL == p)
            return true;
        if(!valid())
            return false;
        char * a = reinterpret_cast<char *>(p);
        if(data_ <= a && a < data_ + off(capa_)){
            p->dealloc();
            --u();
            return true;
        }
        return false;
    }
    const __Node * searchNode(uint32_t hash) const{
        if(!valid())
            return NULL;
        const __Node * const p = nodeOfHash(hash);
        if(p->matchFirst(hash))
            return p;
        return NULL;
    }
    __Node * searchNode(uint32_t hash){
        if(!valid())
            return NULL;
        __Node * const p = nodeOfHash(hash);
        if(p->matchFirst(hash))
            return p;
        return NULL;
    }
    void clear(){
        if(valid()){
            for(uint32_t i = 0;i < capa_;++i)
                nodeAtIndex(i)->clear();
            *used_ = 0;
        }
    }
    std::string toString(const uint32_t * usedp) const{
        assert(usedp);
        CToString oss;
        oss<<"{capa_="<<capa_
            <<", valueSz_="<<valueSz_
            <<", used_=";
        if(used_){
            oss<<*used_;
        }else
            oss<<"NULL";
        oss<<", "<<(used_ - usedp)  //real index
            <<"}";
        return oss.str();
    }
private:
    size_t off(size_t cnt) const{return __Node::Offset(cnt, valueSz_);}
    __Used & u(){return *reinterpret_cast<__Used *>(used_);}
    __Node * nodeAtIndex(size_t i){return reinterpret_cast<__Node *>(data_ + off(i));}
    const __Node * nodeAtIndex(size_t i) const{return reinterpret_cast<const __Node *>(data_ + off(i));}
    __Node * nodeOfHash(uint32_t hash){return nodeAtIndex(hash % capa_);}
    const __Node * nodeOfHash(uint32_t hash) const{return nodeAtIndex(hash % capa_);}
    //fields
    char * data_;
    uint32_t * used_;
    uint32_t capa_;
    uint32_t valueSz_;
};


NS_IMPL_END

#endif

