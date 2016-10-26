#ifndef DOZERG_HASH_TABLE_H_20130327
#define DOZERG_HASH_TABLE_H_20130327

/*
    ���Hash���̲߳���ȫ
        CMultiRowHashTable      ���hash��֧�ֶ���key+value
        CMultiRowHashUpgrade    ���hash������������
    TODO:
        ����create/upgrade seq��������ʹ�����hash������create��upgrade
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
    //����/��ȡ����
    void capacity(size_t capa){capa_ = capa;}
    size_t capacity() const{return capa_;}
    //����/��ȡ����
    void row(int r){row_ = r;}
    int row() const{return row_;}
    //����/��ȡ�����ڴ��ֽڴ�С
    void setBufSize(size_t sz){sz_ = sz;}
    size_t bufSize() const{return sz_;}
    //����head
    void setHeadBuf(std::vector<char> & buf){buf_.swap(buf);}
    //�޸�hash��
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

//Value: Ԫ�����ͣ�������POD����C struct����
//KeyOfValue: ����ʵ�����³�Ա
//      typedef key_type result_type;       //key������
//      key_type & operator ()(value_type & v) const;               //��Ԫ����ȡ��key
//      const key_type & operator()(const value_type & v) const;    //��Ԫ����ȡ��key
//EmptyKey: ����ʵ�����³�Ա����
//      bool isEmpty(const key_type & key) const;   //�ж�key�Ƿ�δʹ��
//      void resetKey(key_type * key) const;        //��key���ó�δʹ��
//HashKey: ����ʵ�����³�Ա����
//      size_t operator ()(const key_type & key) const; //����key��Ӧ��hashֵ
//EqualKey: ����ʵ�����³�Ա����
//      bool operator ()(const key_type & key1, const key_type & key2) const;   //�ж�2��key�Ƿ����
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
    static const uint16_t kCurVersion = 0;  //��ǰversion
    static const int kRowMax = 256;         //������������
    //����capacity��row������Ҫ���ڴ��ֽڴ�С
    //capacity: ��Ҫ���ɵ�Ԫ�ظ���
    //row: hash�������
    //return:
    //  0       ʧ��
    //  ����    ��Ҫ���ڴ��ֽڴ�С
    static size_t CalcBufSize(size_t capacity, int row){
        if(check(capacity, row)){
            std::vector<uint32_t> cols;
            size_t realCapa = tools::PrimesGenerator(capacity, row, cols);
            if(realCapa >= capacity)
                return bufSize(row, realCapa);
        }
        return 0;
    }
    //�ж�value��key�Ƿ�δʹ��
    static bool isEmptyKey(const_reference value){
        return key_empty().isEmpty(extract_key()(value));
    }
    //����/��ʼ��
    //buf: �����ڴ棬�ɵ����߱�֤��Ч��
    //sz: buf���ֽڴ�С
    //capacity: ��Ҫ���ɵ�Ԫ�ظ���
    //row: hash�������
    //create:
    //  true    ����hash����վ�����
    //  false   ʹ������hash�����buf���ݵ���Ч��
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
    //  true    �ɹ�
    //  false   ʧ��
    bool init(char * buf, size_t sz){
        return initAux(buf, sz, false, 0, 0);
    }
    bool init(char * buf, size_t sz, size_t capacity, int row, bool create = false){
        if(create)
            return this->create(buf, sz, capacity, row);
        return initAux(buf, sz, true, capacity, row);
    }
    //���õ�ǰ����hash�����ݲ���Ӱ��
    void uninit(){
        head_ = NULL;
        rows_.clear();
    }
    //�Ƿ��ʼ��
    bool valid() const{return (NULL != head_ && !rows_.empty());}
    //��ȡ����
    int rowSize() const{return (head_ ? head_->row_ : 0);}
    //��ȡÿ�п����ɵ�Ԫ�ظ���
    size_t capacityOfRow(int row) const{return rows_[row].capacity();}
    //��ȡ�����ɵ�Ԫ������
    size_t capacity() const{return (head_ ? head_->realCapa_ : 0);}
    //��ȡÿ����ʹ�õ�Ԫ�ظ���
    size_t sizeOfRow(int row) const{return rows_[row].used();}
    //��ȡ��ʹ�õ�Ԫ������
    size_t size() const{
        size_t ret = 0;
        for(int i = 0;i < rowSize();++i)
            ret += sizeOfRow(i);
        return ret;
    }
    //�Ƿ�Ϊ��
    bool empty() const{
        for(int i = 0;i < rowSize();++i)
            if(0 != sizeOfRow(i))
                return false;
        return true;
    }
    //��ȡ����ʱ��
    time_t createTime() const{return (head_ ? head_->creatTime_ : 0);}
    //��ȡ����޸�ʱ��
    time_t updateTime() const{return (head_ ? head_->modTime_ : 0);}
    //��ȡ�������ʱ��
    time_t upgradeTime() const{return (head_ ? head_->upgradeTime_ : 0);}
    //���hash��ɶ�����
    std::string toString(){
        CToString oss;
        oss<<"{\n"
            <<"  head="<<tools::ToStringPtr(head_)<<'\n';
        for(int i = 0;i < rowSize();++i)
            oss<<"  row["<<i<<"]="<<rows_[i].toString(usedArray(head_))<<'\n';
        oss<<"}";
        return oss.str();
    }
    //����Ԫ��
    //return:
    //  end()   û���ҵ�
    //  ����    Ԫ�ض�Ӧ�ĵ�����
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
    //����Ԫ��
    //return:
    //  end()   ����ʧ��
    //  ����    ����ɹ���Ԫ�ض�Ӧ�ĵ�����
    iterator insert(const_reference value){
        __Iter it;
        uint32_t index;
        if(!findAux(extract_key()(value), it, index, true))
            return end();
        it->setValue(index, value);
        update();
        return iterator(it, index);
    }
    //ɾ��Ԫ��
    //key: Ҫɾ����Ԫ��key
    //it: Ҫɾ����iterator
    //return: ʵ��ɾ����Ԫ�ظ���
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
    //���hash��
    void clear(){
        for(__Iter it = rows_.begin();it != rows_.end();++it)
            it->clear();
        update();
    }
    //��ȡ������
    iterator begin(){return iterator(rows_.begin(), 0);}
    iterator end(){return iterator(rows_.end(), 0);}
    const_iterator begin() const{return const_iterator(rows_.begin(), 0);}
    const_iterator end() const{return const_iterator(rows_.end(), 0);}

    //----hash����������----
    //1. ���������������������
    //  CMultiRowHashUpgrade up(newCapa, newRow);
    //2. ģ������
    //  if(!ht.upgradeCalc(up))
    //      cerr<<"ht cannot upgrade to newCapa and newRow!";
    //3. ���������ڴ�(α��)
    //  if(!expandOrShrink(buf, up.bufSize()))
    //      cerr<<"expand or shrink buf to newSize failed!";
    //4. ��������
    //  ht.upgradeCommit(buf, sz, up);
    //---------���---------

    //����ǰhash��ģ�����������ı�hash������
    //up: ���������в����ͼ����������class CMultiRowHashUpgrade;
    //return:
    //  false   ʧ��
    //  true    �ɹ�
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
    //����ǰhash����������
    //buf: ������hash��������ڴ棬�����߱�֤��Ч�Ժ��ֽڴ�С
    //up: ���������в����ͼ������������Ǿ���upgradeCalc()�����
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
    mutable __Rows rows_;   //ÿ�е���Ϣ
    __Head * head_;         //hash��ͷ
};

NS_SERVER_END

#endif

