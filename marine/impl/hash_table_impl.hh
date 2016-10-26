#ifndef DOZERG_HASH_TABLE_IMPL_H_20130327
#define DOZERG_HASH_TABLE_IMPL_H_20130327

#include <string>
#include <vector>
#include <cassert>
#include <iterator>         //std::forward_iterator_tag
#include "../template.hh"
#include "../to_string.hh"
#include "../tools/time.hh"

NS_IMPL_BEGIN

/*  multi-row hash table memory layout:
        Head            --- 1024 B
        used array      --- 1024 B (= 4 * 256)
        column array    --- 1024 B (= 4 * 256)
        reserved        --- 1024 B
        data            --- sizeof(value_type) * capacity()
*/

struct CMultiRowHashHead{
    uint16_t version_;      //版本号
    uint16_t row_;          //行数
    uint32_t valueSz_;      //每个元素字节长度
    uint64_t capacity_;     //需要容纳的元素个数
    uint64_t realCapa_;     //实际可容纳元素个数
    uint32_t creatTime_;    //创建时间
    uint32_t modTime_;      //上次修改时间
    uint32_t upgradeTime_;  //上次升级时间
    char reserved_[1024 - 36];
    std::string toString() const{
        CToString oss;
        oss<<"{version_="<<version_
            <<", row_="<<row_
            <<", valueSz_="<<valueSz_
            <<", capacity_="<<capacity_
            <<", realCapa_="<<realCapa_
            <<", creatTime_="<<tools::TimeString(creatTime_)<<"("<<creatTime_<<")"
            <<", modTime_="<<tools::TimeString(modTime_)<<"("<<modTime_<<")"
            <<", upgradeTime_="<<tools::TimeString(upgradeTime_)<<"("<<upgradeTime_<<")"
            <<"}";
        return oss.str();
    }
    void update(){modTime_ = tools::Time(NULL);}
};

CHECK_TYPE_SIZE(CMultiRowHashHead, 1024);

template<class HT>
class CMultiRowHashRowInfo
{
    typedef HT __HashTable;
    typedef CMultiRowHashRowInfo<HT>    __Myt;
    typedef typename __HashTable::value_type        value_type;
    typedef typename __HashTable::reference         reference;
    typedef typename __HashTable::const_reference   const_reference;
    typedef typename __HashTable::pointer           pointer;
    typedef typename __HashTable::extract_key       extract_key;
    typedef typename __HashTable::key_empty         key_empty;
public:
    CMultiRowHashRowInfo()
        : data_(NULL), used_(NULL), capa_(0)
    {}
    CMultiRowHashRowInfo(uint32_t capa, uint32_t * used, pointer data)
        : data_(data), used_(used), capa_(capa)
    {}
    bool operator <(const __Myt & a) const{return (capa_ < a.capa_);}
    bool operator >(const __Myt & a) const{return a < *this;}
    uint32_t capacity() const{return capa_;}
    uint32_t used() const{return *used_;}
    uint32_t indexOf(uint32_t hash) const{
        assert(capa_);
        return (hash % capa_);
    }
    reference valueOf(uint32_t index){
        assert(data_ && capa_ && index < capa_);
        return data_[index];
    }
    const_reference valueOf(uint32_t index) const{
        assert(data_ && index < capa_);
        return data_[index];
    }
    void setValue(uint32_t index, const_reference value){
        assert(data_ && used_ && index < capa_);
        data_[index] = value;
        if(++*used_ > capa_)
            *used_ = capa_;
    }
    void resetValue(uint32_t index){
        key_empty().resetKey(&extract_key()(valueOf(index)));
        assert(used_);
        if(*used_ > 0)
            --*used_;
    }
    void clear(){
        assert(data_ && used_ && capa_);
        if(0 != *used_){
            memset(data_, 0, sizeof(value_type) * capa_);
            *used_ = 0;
        }
    }
    std::string toString(const uint32_t * usedp) const{
        assert(usedp);
        CToString oss;
        oss<<"{capa_="<<capa_
            <<", used_=";
        if(used_){
            oss<<*used_;
        }else
            oss<<"NULL";
        oss<<", "<<(used_ - usedp)
            <<"}";
        return oss.str();
    }
private:
    pointer data_;
    uint32_t * used_;
    uint32_t capa_;
};

template<class HT>
class CMultiRowHashConstIterator
{
    typedef CMultiRowHashConstIterator<HT> __Myt;
protected:
    typedef HT __HashTable;
public:
    typedef std::forward_iterator_tag               iterator_category;
    typedef typename __HashTable::value_type        value_type;
    typedef typename __HashTable::const_reference   reference;
    typedef typename __HashTable::const_pointer     pointer;
    typedef typename __HashTable::difference_type   difference_type;
    typedef CMultiRowHashRowInfo<__HashTable>       __RowInfo;
    typedef std::vector<__RowInfo>                  __Rows;
    typedef typename __Rows::iterator               __Iter;
public:
    //functions
    CMultiRowHashConstIterator():index_(0){}
    CMultiRowHashConstIterator(__Iter it, size_t index)
        : it_(it), index_(index)
    {}
    reference operator *() const{return it_->valueOf(index_);}
    pointer operator ->() const{return &(operator *());}
    bool operator ==(const __Myt & other) const{return (it_ == other.it_ && index_ == other.index_);}
    bool operator !=(const __Myt & other) const{return !operator ==(other);}
    __Myt & operator ++(){
        if(++index_ >= it_->capacity()){
            ++it_;
            index_ = 0;
        }
        return *this;
    }
    __Myt operator ++(int){
        __Myt tmp(*this);
        ++*this;
        return tmp;
    }
protected:
    //fields
    __Iter it_;
    size_t index_;
};

template<class HT>
class CMultiRowHashIterator : public CMultiRowHashConstIterator<HT>
{
    typedef CMultiRowHashConstIterator<HT>  __MyBase;
    typedef CMultiRowHashIterator<HT>       __Myt;
    typedef typename __MyBase::__HashTable  __HashTable;
    typedef typename __MyBase::__Iter       __Iter;
public:
    typedef typename __MyBase::iterator_category    iterator_category;
    typedef typename __MyBase::value_type           value_type;
    typedef typename __HashTable::reference         reference;
    typedef typename __HashTable::pointer           pointer;
    typedef typename __MyBase::difference_type      difference_type;
    CMultiRowHashIterator(){}
    CMultiRowHashIterator(__Iter it, size_t index)
        : __MyBase(it, index)
    {}
    reference operator *() const{return __MyBase::it_->valueOf(__MyBase::index_);}
    pointer operator ->() const{return &(operator *());}
    __Myt & operator ++(){
        __MyBase::operator ++();
        return *this;
    }
    __Myt operator ++(int){
        __Myt tmp(*this);
        ++*this;
        return tmp;
    }
    void resetValue(){__MyBase::it_->resetValue(__MyBase::index_);}
};

NS_IMPL_END

#endif
