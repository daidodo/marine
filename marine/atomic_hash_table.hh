#ifndef DOZERG_ATOMIC_HASH_TABLE_H_20130812
#define DOZERG_ATOMIC_HASH_TABLE_H_20130812

/*
    �ɶ��̲߳����Ķ��Hash��
    ���߳�˵��������ͬһ��key��֧�ֵ��߳�д�����̶߳������ڲ�ͬ��key��֧�ֶ��̶߳�д
    ����˵����֧�ֶ���key+�䳤value���ܳ������16MB
        CAtomicHashTable        ���hash��
*/

#include <vector>
#include "tools/debug.hh"
#include "impl/atomic_hash_table_impl.hh"

NS_SERVER_BEGIN

//Key: key���ͣ�������POD����C struct����
//HashKey: ����ʵ�����³�Ա����
//      size_t operator ()(const key_type & key) const; //����key��Ӧ��hashֵ
//EqualKey: ����ʵ�����³�Ա����
//      bool operator ()(const key_type & key1, const key_type & key2) const;   //�ж�2��key�Ƿ����
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
    static const size_t kValueLenMax = (1UL << 24);  //key+value������ֽڳ���
    //functions:
    //����valueLen, capacity��row������Ҫ���ڴ��ֽڴ�С
    //valueLen: value�Ĺ����ֽڳ���
    //capacity: ��Ҫ���ɵ�Ԫ�ظ���
    //row: hash�������
    //return:
    //  0       ʧ��
    //  ����    ��Ҫ���ڴ��ֽڴ�С
    static size_t CalcBufSize(size_t capacity, int row, size_t valueLen){
        if(__Head::Check(capacity, row)){
            std::vector<uint32_t> cols;
            size_t realCapa = tools::PrimesGenerator(capacity, row, cols);
            if(realCapa >= capacity)
                return bufSize(row, realCapa, sizeof(key_type) + alignLen(valueLen));
        }
        return 0;
    }
    //����/��ʼ��
    //buf: �����ڴ棬�ɵ����߱�֤��Ч��
    //sz: buf���ֽڴ�С
    //capacity: ��Ҫ���ɵ�Ԫ�ظ���
    //valueLen: value�Ĺ����ֽڳ���(<16MB)
    //row: hash�������
    //create:
    //  true    ����hash����վ�����
    //  false   ʹ������hash�����buf���ݵ���Ч��
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
    //  true    �ɹ�
    //  false   ʧ��
    bool init(char * buf, size_t sz){
        return initAux(buf, sz, false, 0, 0, 0);
    }
    bool init(char * buf, size_t sz, size_t capacity, int row, size_t valueLen, bool create = false){
        valueLen = alignLen(valueLen);
        if(create)
            return this->create(buf, sz, capacity, row, valueLen);
        return initAux(buf, sz, true, capacity, row, valueLen);
    }
    //���õ�ǰ����hash�����ݲ���Ӱ��
    void uninit(){
        head_ = NULL;
        rows_.clear();
    }
    //�Ƿ��ʼ��
    bool valid() const{return (NULL != head_ && !rows_.empty());}
    //��ȡ����
    int rowSize() const{return (head_ ? head_->row() : 0);}
    //��ȡÿ�п����ɵ�Ԫ�ظ���
    size_t capacityOfRow(int row) const{return rows_[row].capacity();}
    //��ȡ�����ɵ�Ԫ������
    size_t capacity() const{return (head_ ? head_->realCapa() : 0);}
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
    time_t createTime() const{return (head_ ? head_->createTime() : 0);}
    //��ȡ����޸�ʱ��
    time_t updateTime() const{return (head_ ? head_->modTime() : 0);}
    //��ȡ�������ʱ��
    time_t upgradeTime() const{return (head_ ? head_->upgradeTime() : 0);}
    //���hash��ɶ�����
    std::string toString() const{
        CToString oss;
        oss<<"{\n"
            <<"  head="<<tools::ToStringPtr(head_)<<'\n';
        for(int i = 0;i < rowSize();++i)
            oss<<"  row["<<i<<"]="<<rows_[i].toString(head_->usedArray())<<'\n';
        oss<<"}";
        return oss.str();
    }
    //����key��value�����key�Ѵ��ڣ����ܵ����ظ�key
    //len: value���ֽڳ���
    //return:
    //  true    �ɹ�
    //  false   ʧ��
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
    //��ȡkey��Ӧ��value
    //len: ����value���������ֽڳ��ȣ�����ʵ�����ݵ��ֽڳ���
    //return:
    //  true    �ҵ�key
    //  false   ʧ�ܣ�����δ�ҵ�key
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
    //����key�Ƿ����
    //return:
    //  true    �ҵ�key
    //  false   ʧ�ܣ�����δ�ҵ�key
    bool has(const key_type & key) const{
        if(!valid())
            return false;
        return (NULL != searchNode(hasher()(key), key));
    }
    //�޸�key��Ӧ��value�����key�����ڣ�������
    //len: value���ֽڳ���
    //return:
    //  true    �ɹ�
    //  false   ʧ��
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
    //ɾ��key�Ͷ�Ӧ������
    //return: ʵ���ͷŵĽڵ���
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
    //�������key�����ݣ�ֻ֧�ֵ��߳�
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
    mutable __Rows rows_;   //ÿ�е���Ϣ
    __Head * head_;         //hash��ͷ
};

NS_SERVER_END

#endif

