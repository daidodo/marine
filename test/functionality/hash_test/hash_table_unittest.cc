#include <marine/hash_table.hh>

#include "../inc.h"

#include <set>

class CInt
{
public:
    CInt(uint64_t v = 0):v_(v){}
    operator uint64_t() const{return v_;}
    uint64_t fun() const{return v_;}
private:
    uint64_t v_;
};

template<class Key>
struct CHash: public std::unary_function<Key, size_t>
{
    size_t operator ()(const Key & k){
        return k;
    }
};

struct CEmptyKey
{
    typedef CInt key_type;
    bool isEmpty(const key_type & key) const{   //判断key是否未使用
        return (0 == key);
    }
    void resetKey(key_type * key) const{        //将key设置成未使用
        assert(key);
        *key = 0;
    }
};

typedef CMultiRowHashTable<CInt, CEmptyKey, CIdentity<CInt>, CHash> __HT;

const size_t capa = 10000;
const int row = 10;
const size_t realCapa = 10012;

TEST(CMultiRowHashTable, ctor)
{

    size_t sz = __HT::CalcBufSize(capa, row);
    ASSERT_LE(capa * 8, sz);
    std::string str(sz, 0);
    char * const buf = &str[0];
    {
        __HT ht;
        ASSERT_FALSE(ht.valid());
    }{
        __HT ht(buf, sz);
        ASSERT_FALSE(ht.valid());
    }{
        __HT ht(buf, sz, capa, row);
        ASSERT_FALSE(ht.valid());
    }{
        __HT ht(buf, sz, capa, row, false);
        ASSERT_FALSE(ht.valid());
    }{
        __HT ht(buf, sz, capa, row, true);
        ASSERT_TRUE(ht.valid());
    }
    //buf is init
    {
        __HT ht(buf, sz);
        ASSERT_TRUE(ht.valid());
    }{
        __HT ht(buf, sz, capa, row);
        ASSERT_TRUE(ht.valid());
    }{
        __HT ht(buf, sz, capa, row, false);
        ASSERT_TRUE(ht.valid());
    }{
        __HT ht(buf, sz, capa, row, true);
        ASSERT_TRUE(ht.valid());
    }
    //sz is not enough
    --sz;
    {
        __HT ht(buf, sz);
        ASSERT_FALSE(ht.valid());
    }{
        __HT ht(buf, sz, capa, row);
        ASSERT_FALSE(ht.valid());
    }{
        __HT ht(buf, sz, capa, row, false);
        ASSERT_FALSE(ht.valid());
    }{
        __HT ht(buf, sz, capa, row, true);
        ASSERT_FALSE(ht.valid());
    }
    //capa and row
    ++sz;
    for(size_t c = 0;c < capa + 100;++c){
        __HT ht(buf, sz, c, row, true);
        if(c < size_t(row)){
            ASSERT_FALSE(ht.valid())<<"c="<<c;
        }else if(c <= realCapa){
            ASSERT_TRUE(ht.valid());
        }else{
            ASSERT_FALSE(ht.valid())<<"c="<<c;
        }
    }
}

TEST(CMultiRowHashTable, init)
{
    size_t sz = __HT::CalcBufSize(capa, row);
    ASSERT_LE(capa * 8, sz);
    std::string str(sz, 0);
    char * const buf = &str[0];
    {
        __HT ht;
        ASSERT_FALSE(ht.valid());

        ASSERT_FALSE(ht.init(buf, sz));
        ASSERT_FALSE(ht.valid());

        ASSERT_FALSE(ht.init(buf, sz, capa, row));
        ASSERT_FALSE(ht.valid());

        ASSERT_FALSE(ht.init(buf, sz, capa, row, false));
        ASSERT_FALSE(ht.valid());

        ASSERT_TRUE(ht.init(buf, sz, capa, row, true));
        ASSERT_TRUE(ht.valid());

        //re-init
        ASSERT_FALSE(ht.init(buf, sz, capa, row, true));
        ASSERT_TRUE(ht.valid());
    }
    //buf is init
    {
        __HT ht;
        ASSERT_FALSE(ht.valid());

        ASSERT_TRUE(ht.init(buf, sz));
        ASSERT_TRUE(ht.valid());

        //re-init
        ASSERT_FALSE(ht.init(buf, sz));
        ASSERT_TRUE(ht.valid());
    }{
        __HT ht;
        ASSERT_FALSE(ht.valid());

        ASSERT_TRUE(ht.init(buf, sz, capa, row));
        ASSERT_TRUE(ht.valid());

        //re-init
        ASSERT_FALSE(ht.init(buf, sz, capa, row));
        ASSERT_TRUE(ht.valid());
    }{
        __HT ht;
        ASSERT_FALSE(ht.valid());

        ASSERT_TRUE(ht.init(buf, sz, capa, row, false));
        ASSERT_TRUE(ht.valid());

        //re-init
        ASSERT_FALSE(ht.init(buf, sz, capa, row, false));
        ASSERT_TRUE(ht.valid());
    }{
        __HT ht;
        ASSERT_FALSE(ht.valid());

        ASSERT_TRUE(ht.init(buf, sz, capa, row, true));
        ASSERT_TRUE(ht.valid());

        //re-init
        ASSERT_FALSE(ht.init(buf, sz, capa, row, true));
        ASSERT_TRUE(ht.valid());
    }
    //sz is not enough
    --sz;
    {
        __HT ht;
        ASSERT_FALSE(ht.valid());

        ASSERT_FALSE(ht.init(buf, sz));
        ASSERT_FALSE(ht.valid());

        ASSERT_FALSE(ht.init(buf, sz, capa, row));
        ASSERT_FALSE(ht.valid());

        ASSERT_FALSE(ht.init(buf, sz, capa, row, false));
        ASSERT_FALSE(ht.valid());

        ASSERT_FALSE(ht.init(buf, sz, capa, row, true));
        ASSERT_FALSE(ht.valid());
    }
    //capa and row
    ++sz;
    for(size_t c = 0;c < capa + 100;++c){
        __HT ht;
        ASSERT_FALSE(ht.valid());
        if(c < size_t(row)){
            ASSERT_FALSE(ht.init(buf, sz, c, row, true));
            ASSERT_FALSE(ht.valid())<<"c="<<c;
        }else if(c <= realCapa){
            ASSERT_TRUE(ht.init(buf, sz, c, row, true));
            ASSERT_TRUE(ht.valid());
        }else{
            ASSERT_FALSE(ht.init(buf, sz, c, row, true));
            ASSERT_FALSE(ht.valid())<<"c="<<c;
        }
    }
}

TEST(CMultiRowHashTable, uninit)
{
    size_t sz = __HT::CalcBufSize(capa, row);
    ASSERT_LE(capa * 8, sz);
    std::string str(sz, 0);
    char * const buf = &str[0];
    {
        __HT ht;
        ASSERT_FALSE(ht.valid());

        ht.uninit();
        ASSERT_FALSE(ht.valid());

        ASSERT_TRUE(ht.init(buf, sz, capa, row, true));
        ASSERT_TRUE(ht.valid());

        ht.uninit();
        ASSERT_FALSE(ht.valid());
    }
}

TEST(CMultiRowHashTable, rowSize)
{
    size_t sz = __HT::CalcBufSize(capa, row);
    ASSERT_LE(capa * 8, sz);
    std::string str(sz, 0);
    char * const buf = &str[0];
    {
        __HT ht;
        ASSERT_FALSE(ht.valid());

        ASSERT_EQ(0, ht.rowSize());
    }{
        __HT ht(buf, sz, capa, row, true);
        ASSERT_TRUE(ht.valid());

        ASSERT_EQ(row, ht.rowSize());
    }
}

TEST(CMultiRowHashTable, capacity)
{
    size_t sz = __HT::CalcBufSize(capa, row);
    ASSERT_LE(capa * 8, sz);
    std::string str(sz, 0);
    char * const buf = &str[0];
    {
        __HT ht;
        ASSERT_FALSE(ht.valid());

        ASSERT_EQ(size_t(0), ht.capacity());
        for(int i = 0;i < ht.rowSize();++i)
            ASSERT_LT(size_t(0), ht.capacityOfRow(i));
    }{
        __HT ht(buf, sz, capa, row, true);
        ASSERT_TRUE(ht.valid());

        ASSERT_LE(capa, ht.capacity());
        size_t sum = 0;
        for(int i = 0;i < ht.rowSize();++i){
            ASSERT_LT(size_t(0), ht.capacityOfRow(i));
            sum += ht.capacityOfRow(i);
        }
        ASSERT_EQ(sum, ht.capacity());
    }
}

TEST(CMultiRowHashTable, size)
{
    size_t sz = __HT::CalcBufSize(capa, row);
    ASSERT_LE(capa * 8, sz);
    std::string str(sz, 0);
    char * const buf = &str[0];
    {
        __HT ht;
        ASSERT_FALSE(ht.valid());

        ASSERT_EQ(size_t(0), ht.size());
        for(int i = 0;i < ht.rowSize();++i)
            ASSERT_EQ(size_t(0), ht.sizeOfRow(i));
        ASSERT_TRUE(ht.empty());
    }{
        __HT ht(buf, sz, capa, row, true);
        ASSERT_TRUE(ht.valid());

        ASSERT_EQ(size_t(0), ht.size());
        for(int i = 0;i < ht.rowSize();++i)
            ASSERT_EQ(size_t(0), ht.sizeOfRow(i));
        ASSERT_TRUE(ht.empty());

        for(int v = 1;v <= int(capa) / 100 * 45;++v){
            ASSERT_NE(ht.end(), ht.insert(12345678 + v * v));
            ASSERT_EQ(size_t(v), ht.size())<<"ht="<<ht.toString();
            size_t sum = 0;
            for(int i = 0;i < ht.rowSize();++i)
                sum += ht.sizeOfRow(i);
            ASSERT_EQ(sum, ht.size());
            ASSERT_FALSE(ht.empty());
        }
    }
}

TEST(CMultiRowHashTable, time)
{
    size_t sz = __HT::CalcBufSize(capa, row);
    ASSERT_LE(capa * 8, sz);
    std::string str(sz, 0);
    char * const buf = &str[0];
    {
        __HT ht;
        ASSERT_FALSE(ht.valid());

        ASSERT_EQ(time_t(0), ht.createTime());
        ASSERT_EQ(time_t(0), ht.updateTime());
    }{
        __HT ht(buf, sz, capa, row, true);
        ASSERT_TRUE(ht.valid());
        time_t CT = time(NULL);

        ASSERT_EQ(CT, ht.createTime());
        ASSERT_EQ(CT, ht.updateTime());

        for(int v = 1;v <= 5;++v){
            sleep(1);
            ASSERT_NE(ht.end(), ht.insert(12345678 + v * v));
            ASSERT_EQ(CT, ht.createTime());
            ASSERT_EQ(time(NULL), ht.updateTime());
        }
    }
}

TEST(CMultiRowHashTable, find_insert_erase_clear)
{
    size_t sz = __HT::CalcBufSize(capa, row);
    ASSERT_LE(capa * 8, sz);
    std::string str(sz, 0);
    char * const buf = &str[0];
    const int count = capa / 100 * 45;
    {
        __HT ht(buf, sz, capa, row, true);
        const __HT & cht = ht;
        ASSERT_TRUE(ht.valid());

        for(int v = 1;v <= count;++v){
            const uint32_t k = 12345678 + v * v;

            ASSERT_EQ(ht.end(), ht.find(k));
            ASSERT_EQ(cht.end(), cht.find(k));

            __HT::iterator it1 = ht.insert(k);
            ASSERT_NE(ht.end(), it1);
            ASSERT_EQ(k, *it1);

            __HT::iterator it2 = ht.find(k);
            ASSERT_EQ(it1, it2);
            ASSERT_EQ(k, *it2);
            __HT::const_iterator it3 = cht.find(k);
            ASSERT_EQ(it1, it3);
            ASSERT_EQ(k, *it3);
        }
        ASSERT_EQ(size_t(count), ht.size());

        for(int v = 1;v <= count / 2;++v){
            const uint32_t k = 12345678 + v * v;

            __HT::iterator it1 = ht.find(k);
            ASSERT_NE(ht.end(), it1);
            ASSERT_EQ(k, *it1);
            __HT::const_iterator it2 = cht.find(k);
            ASSERT_NE(cht.end(), it2);
            ASSERT_EQ(k, *it2);

            if(0 == (v & 3) && 1 == (v & 3)){
                ASSERT_EQ(size_t(1), ht.erase(k));
            }else if(2 == (v & 3)){
                ht.erase(it1);
            }else{
                it1.resetValue();
            }

            ASSERT_EQ(ht.end(), ht.find(k));
            ASSERT_EQ(cht.end(), cht.find(k));

            ASSERT_EQ(size_t(0), ht.erase(k));
        }
        ASSERT_EQ(size_t(count / 2), ht.size());

        ht.clear();
        ASSERT_EQ(size_t(0), ht.size());
        for(int v = count / 2 + 1;v <= count;++v){
            const uint32_t k = 12345678 + v * v;

            ASSERT_EQ(ht.end(), ht.find(k));
            ASSERT_EQ(cht.end(), cht.find(k));
            ASSERT_EQ(size_t(0), ht.erase(k));
        }
    }{
        __HT ht;
        const __HT & cht = ht;
        ASSERT_FALSE(ht.valid());

        for(int v = 1;v <= count;++v){
            const uint32_t k = 12345678 + v * v;

            ASSERT_EQ(ht.end(), ht.find(k));
            ASSERT_EQ(cht.end(), cht.find(k));

            ASSERT_EQ(ht.end(), ht.insert(k));

            ASSERT_EQ(ht.end(), ht.find(k));
            ASSERT_EQ(cht.end(), cht.find(k));
        }
        ASSERT_EQ(size_t(0), ht.size());

        for(int v = 1;v <= count / 2;++v){
            const uint32_t k = 12345678 + v * v;

            ASSERT_EQ(ht.end(), ht.find(k));
            ASSERT_EQ(cht.end(), cht.find(k));

            ASSERT_EQ(size_t(0), ht.erase(k));

            ASSERT_EQ(ht.end(), ht.find(k));
            ASSERT_EQ(cht.end(), cht.find(k));
        }
        ASSERT_EQ(size_t(0), ht.size());

        ht.clear();
        ASSERT_EQ(size_t(0), ht.size());
        for(int v = count / 2 + 1;v <= count;++v){
            const uint32_t k = 12345678 + v * v;

            ASSERT_EQ(ht.end(), ht.find(k));
            ASSERT_EQ(cht.end(), cht.find(k));
            ASSERT_EQ(size_t(0), ht.erase(k));
        }
    }
}

TEST(CMultiRowHashTable, iterator)
{
    size_t sz = __HT::CalcBufSize(capa, row);
    ASSERT_LE(capa * 8, sz);
    std::string str(sz, 0);
    char * const buf = &str[0];
    const int count = capa / 100 * 45;
    {
        __HT ht(buf, sz, capa, row, true);
        const __HT & cht = ht;
        ASSERT_TRUE(ht.valid());

        std::set<uint64_t> s1;
        for(int v = 1;v <= count;++v){
            const uint32_t k = 12345678 + v * v;

            ASSERT_NE(ht.end(), ht.insert(k));
            s1.insert(k);
        }
        ASSERT_EQ(size_t(count), ht.size());
        ASSERT_EQ(size_t(count), s1.size());

        std::set<uint64_t> s2(s1);
        int i = 0;
        for(__HT::const_iterator it = cht.begin();it != cht.end();++i){
            __HT::const_iterator it2;
            if(1 == (i & 1))
                it2 = it++;
            else
                it2 = it;
            if(__HT::isEmptyKey(*it2))
                continue;
            ASSERT_EQ(*it2, it2->fun());
            ASSERT_EQ(size_t(1), s1.erase(*it2));
            if(0 == (i & 1))
                it2 = ++it;
        }
        ASSERT_EQ(size_t(0), s1.size());

        i = 0;
        for(__HT::iterator it = ht.begin();it != ht.end();++i){
            __HT::iterator it2;
            if(1 == (i & 1))
                it2 = it++;
            else
                it2 = it;
            if(__HT::isEmptyKey(*it2))
                continue;
            ASSERT_EQ(*it2, it2->fun());
            ASSERT_EQ(size_t(1), s2.erase(*it2));
            if(0 == (i & 1))
                it2 = ++it;
        }
        ASSERT_EQ(size_t(0), s2.size());
    }{
        __HT ht;
        const __HT & cht = ht;
        ASSERT_FALSE(ht.valid());

        ASSERT_EQ(ht.begin(), ht.end());
        ASSERT_EQ(cht.begin(), cht.end());

        for(int v = 1;v <= count;++v){
            const uint32_t k = 12345678 + v * v;
            ASSERT_EQ(ht.end(), ht.insert(k));
        }

        ASSERT_EQ(ht.begin(), ht.end());
        ASSERT_EQ(cht.begin(), cht.end());
    }
}

TEST(CMultiRowHashTable, upgrade)
{
    size_t sz = __HT::CalcBufSize(capa, row);
    ASSERT_LE(capa * 8, sz);
    std::string str(sz, 0);
    char * buf = &str[0];
    {   //expand
        const int count = capa / 100 * 45;
        __HT ht(buf, sz, capa, row, true);
        ASSERT_TRUE(ht.valid());

        for(int v = 1;v <= count;++v){
            const uint32_t k = 12345678 + v * v;

            __HT::iterator it1 = ht.insert(k);
            ASSERT_NE(ht.end(), it1);
            ASSERT_EQ(k, *it1);

            __HT::iterator it2 = ht.find(k);
            ASSERT_EQ(it1, it2);
            ASSERT_EQ(k, *it2);
        }
        ASSERT_EQ(size_t(count), ht.size());
        cout<<"before upgrade, ht="<<ht.toString()<<endl;

        CMultiRowHashUpgrade up(capa, row);
        ASSERT_EQ(capa, up.capacity());
        ASSERT_EQ(row, up.row());

        const size_t newCapa = 2 * capa;
        const int newRow = row + (row >> 1);
        up.capacity(newCapa);
        up.row(newRow);
        ASSERT_EQ(newCapa, up.capacity());
        ASSERT_EQ(newRow, up.row());

        ASSERT_TRUE(ht.upgradeCalc(up));
        ASSERT_LT(size_t(0), up.bufSize());

        str.resize(up.bufSize());
        buf = &str[0];

        ASSERT_EQ(0, ht.upgradeTime());
        ht.upgradeCommit(buf, up);
        ASSERT_TRUE(ht.valid());
        ASSERT_EQ(time(NULL), ht.upgradeTime());

        ASSERT_EQ(size_t(count), ht.size());
        for(int v = 1;v <= 10;++v){
            const uint32_t k = 125678 + v * v * v;

            __HT::iterator it1 = ht.insert(k);
            ASSERT_NE(ht.end(), it1);
            ASSERT_EQ(k, *it1);

            __HT::iterator it2 = ht.find(k);
            ASSERT_EQ(it1, it2);
            ASSERT_EQ(k, *it2);
        }
        cout<<"after upgrade, ht="<<ht.toString()<<endl;

        ASSERT_EQ(size_t(count + 10), ht.size());
        for(int v = 1;v <= count;++v){
            const uint32_t k = 12345678 + v * v;

            __HT::iterator it2 = ht.find(k);
            ASSERT_NE(ht.end(), it2);
            ASSERT_EQ(k, *it2);

            ht.erase(it2);
            __HT::iterator it1 = ht.find(k);
            ASSERT_EQ(ht.end(), it1);
        }
        ASSERT_EQ(size_t(10), ht.size());
    }{  //shrink
        const int count = capa / 100 * 30;
        __HT ht(buf, sz, capa, row, true);
        ASSERT_TRUE(ht.valid());

        for(int v = 1;v <= count;++v){
            const uint32_t k = 12345678 + v * v;

            __HT::iterator it1 = ht.insert(k);
            ASSERT_NE(ht.end(), it1);
            ASSERT_EQ(k, *it1);

            __HT::iterator it2 = ht.find(k);
            ASSERT_EQ(it1, it2);
            ASSERT_EQ(k, *it2);
        }
        ASSERT_EQ(size_t(count), ht.size());
        cout<<"before upgrade, ht="<<ht.toString()<<endl;

        CMultiRowHashUpgrade up(capa, row);
        ASSERT_EQ(capa, up.capacity());
        ASSERT_EQ(row, up.row());

        const size_t newCapa = capa / 10 * 7;
        const int newRow = row / 2;
        up.capacity(newCapa);
        up.row(newRow);
        ASSERT_EQ(newCapa, up.capacity());
        ASSERT_EQ(newRow, up.row());

        ASSERT_TRUE(ht.upgradeCalc(up));
        ASSERT_LT(size_t(0), up.bufSize());

        str.resize(up.bufSize());
        buf = &str[0];

        ASSERT_EQ(0, ht.upgradeTime());
        ht.upgradeCommit(buf, up);
        ASSERT_TRUE(ht.valid());
        ASSERT_EQ(time(NULL), ht.upgradeTime());

        ASSERT_EQ(size_t(count), ht.size());
        for(int v = 1;v <= 10;++v){
            const uint32_t k = 125678 + v * v * v;

            __HT::iterator it1 = ht.insert(k);
            ASSERT_NE(ht.end(), it1);
            ASSERT_EQ(k, *it1);

            __HT::iterator it2 = ht.find(k);
            ASSERT_EQ(it1, it2);
            ASSERT_EQ(k, *it2);
        }
        cout<<"after upgrade, ht="<<ht.toString()<<endl;

        ASSERT_EQ(size_t(count + 10), ht.size());
        for(int v = 1;v <= count;++v){
            const uint32_t k = 12345678 + v * v;

            __HT::iterator it2 = ht.find(k);
            ASSERT_NE(ht.end(), it2);
            ASSERT_EQ(k, *it2);

            ht.erase(it2);
            __HT::iterator it1 = ht.find(k);
            ASSERT_EQ(ht.end(), it1);
        }
        ASSERT_EQ(size_t(10), ht.size());
    }{
        __HT ht;
        ASSERT_FALSE(ht.valid());
        cout<<"empty ht="<<ht.toString()<<endl;

        CMultiRowHashUpgrade up(capa, row);
        ASSERT_FALSE(ht.upgradeCalc(up));
        ASSERT_EQ(size_t(0), up.bufSize());
    }
}
