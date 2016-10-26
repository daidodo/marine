#include <marine/atomic_hash_table.hh>

#include <vector>
#include <iostream>
#include <marine/threads.hh>
#include <marine/logger.hh>
#include "../inc.h"

const size_t kCapacity = 1000000;
const int kRow = 10;
#define kValueLen 13

#define ENDLESS 0
#define THREAD_COUNT_R    4
#define THREAD_COUNT_W    4
const size_t kCapa2 = 10 * kCapacity;
const size_t kKeyRange = kCapa2 / 3;


const std::string kEmpty = "{\n  head=NULL\n}";

class CKey
{
    uint64_t v_;
public:
    CKey():v_(0){}
    CKey(uint64_t v):v_(v){}
    size_t hash() const{return (v_ * v_);}
    bool operator ==(const CKey & a) const{
        return (v_ == a.v_);
    }
};

namespace marine{
    template<>
    struct CHashFn<CKey> : public std::unary_function<CKey, size_t>
    {
        size_t operator()(const CKey & v) const{
            return v.hash();
        }
    };
}

typedef CKey __Key;

//test valueLen=0
typedef CAtomicHashTable<__Key> __HT;

TEST(CAtomicHashTable, init)
{
    const size_t bufSz = __HT::CalcBufSize(kCapacity, kRow, kValueLen);
    ASSERT_LE(kCapacity * (sizeof(__Key) + kValueLen), bufSz);
    {
        __HT ht;
        ASSERT_FALSE(ht.valid());
        ASSERT_EQ(0, ht.rowSize());
        ASSERT_EQ(size_t(0), ht.capacity());
        for(int i = 0;i < ht.rowSize();++i)
            ASSERT_EQ(size_t(0), ht.sizeOfRow(i));
        ASSERT_EQ(size_t(0), ht.size());
        ASSERT_TRUE(ht.empty());
        ASSERT_EQ(0, ht.createTime());
        ASSERT_EQ(0, ht.updateTime());
        ASSERT_EQ(0, ht.upgradeTime());
        ASSERT_EQ(kEmpty, ht.toString())<<ht.toString();

        ht.uninit();
        ASSERT_FALSE(ht.valid());
    }{
        __HT ht(NULL, 0);
        ASSERT_FALSE(ht.valid());
        ASSERT_EQ(0, ht.rowSize());
        ASSERT_EQ(size_t(0), ht.capacity());
        for(int i = 0;i < ht.rowSize();++i)
            ASSERT_EQ(size_t(0), ht.sizeOfRow(i));
        ASSERT_EQ(size_t(0), ht.size());
        ASSERT_TRUE(ht.empty());
        ASSERT_EQ(0, ht.createTime());
        ASSERT_EQ(0, ht.updateTime());
        ASSERT_EQ(0, ht.upgradeTime());
        ASSERT_EQ(kEmpty, ht.toString())<<ht.toString();

        ht.uninit();
        ASSERT_FALSE(ht.valid());
    }{
        __HT ht(NULL, 0, kCapacity, kRow, kValueLen);
        ASSERT_FALSE(ht.valid());
        ASSERT_EQ(0, ht.rowSize());
        ASSERT_EQ(size_t(0), ht.capacity());
        for(int i = 0;i < ht.rowSize();++i)
            ASSERT_EQ(size_t(0), ht.sizeOfRow(i));
        ASSERT_EQ(size_t(0), ht.size());
        ASSERT_TRUE(ht.empty());
        ASSERT_EQ(0, ht.createTime());
        ASSERT_EQ(0, ht.updateTime());
        ASSERT_EQ(0, ht.upgradeTime());
        ASSERT_EQ(kEmpty, ht.toString())<<ht.toString();

        ht.uninit();
        ASSERT_FALSE(ht.valid());
    }{
        vector<char> buf(bufSz, 'a');

        __HT ht(&buf[0], buf.size());
        ASSERT_FALSE(ht.valid());
        ASSERT_EQ(0, ht.rowSize());
        ASSERT_EQ(size_t(0), ht.capacity());
        for(int i = 0;i < ht.rowSize();++i)
            ASSERT_EQ(size_t(0), ht.sizeOfRow(i));
        ASSERT_EQ(size_t(0), ht.size());
        ASSERT_TRUE(ht.empty());
        ASSERT_EQ(0, ht.createTime());
        ASSERT_EQ(0, ht.updateTime());
        ASSERT_EQ(0, ht.upgradeTime());
        ASSERT_EQ(kEmpty, ht.toString())<<ht.toString();

        ht.uninit();
        ASSERT_FALSE(ht.valid());
    }{
        vector<char> buf(bufSz, 'a');

        __HT ht(&buf[0], buf.size(), kCapacity, kRow, kValueLen);
        ASSERT_FALSE(ht.valid());
        ASSERT_EQ(0, ht.rowSize());
        ASSERT_EQ(size_t(0), ht.capacity());
        for(int i = 0;i < ht.rowSize();++i)
            ASSERT_EQ(size_t(0), ht.sizeOfRow(i));
        ASSERT_EQ(size_t(0), ht.size());
        ASSERT_TRUE(ht.empty());
        ASSERT_EQ(0, ht.createTime());
        ASSERT_EQ(0, ht.updateTime());
        ASSERT_EQ(0, ht.upgradeTime());
        ASSERT_EQ(kEmpty, ht.toString())<<ht.toString();

        ht.uninit();
        ASSERT_FALSE(ht.valid());
    }{
        vector<char> buf(bufSz, 'a');

        __HT ht(&buf[0], buf.size(), kCapacity, kRow, kValueLen, true);
        ASSERT_TRUE(ht.valid());
        ASSERT_EQ(kRow, ht.rowSize());
        ASSERT_LE(kCapacity, ht.capacity());
        for(int i = 0;i < ht.rowSize();++i)
            ASSERT_EQ(size_t(0), ht.sizeOfRow(i));
        ASSERT_EQ(size_t(0), ht.size());
        ASSERT_TRUE(ht.empty());
        ASSERT_EQ(time(NULL), ht.createTime());
        ASSERT_EQ(time(NULL), ht.updateTime());
        ASSERT_EQ(0, ht.upgradeTime());
        ASSERT_NE(kEmpty, ht.toString())<<ht.toString();

        ht.uninit();
        ASSERT_FALSE(ht.valid());
        {
            __HT ht2(&buf[0], buf.size());
            ASSERT_TRUE(ht2.valid());
            ASSERT_EQ(kRow, ht2.rowSize());
            ASSERT_LE(kCapacity, ht2.capacity());
            for(int i = 0;i < ht2.rowSize();++i)
                ASSERT_EQ(size_t(0), ht2.sizeOfRow(i));
            ASSERT_EQ(size_t(0), ht2.size());
            ASSERT_TRUE(ht2.empty());
            ASSERT_EQ(time(NULL), ht2.createTime());
            ASSERT_EQ(time(NULL), ht2.updateTime());
            ASSERT_EQ(0, ht2.upgradeTime());
            ASSERT_NE(kEmpty, ht2.toString())<<ht2.toString();

            ht2.uninit();
            ASSERT_FALSE(ht2.valid());
        }{
            __HT ht2(&buf[0], buf.size(), kCapacity, kRow, kValueLen, false);
            ASSERT_TRUE(ht2.valid());
            ASSERT_EQ(kRow, ht2.rowSize());
            ASSERT_LE(kCapacity, ht2.capacity());
            for(int i = 0;i < ht2.rowSize();++i)
                ASSERT_EQ(size_t(0), ht2.sizeOfRow(i));
            ASSERT_EQ(size_t(0), ht2.size());
            ASSERT_TRUE(ht2.empty());
            ASSERT_EQ(time(NULL), ht2.createTime());
            ASSERT_EQ(time(NULL), ht2.updateTime());
            ASSERT_EQ(0, ht2.upgradeTime());
            ASSERT_NE(kEmpty, ht2.toString())<<ht2.toString();

            ht2.uninit();
            ASSERT_FALSE(ht2.valid());
        }
    }{
        vector<char> buf(bufSz, 'a');
        {
            __HT ht;
            ASSERT_FALSE(ht.valid());

            ASSERT_FALSE(ht.init(&buf[0], buf.size()));
            ASSERT_FALSE(ht.valid());
            ASSERT_EQ(0, ht.rowSize());
            ASSERT_EQ(size_t(0), ht.capacity());
            for(int i = 0;i < ht.rowSize();++i)
                ASSERT_EQ(size_t(0), ht.sizeOfRow(i));
            ASSERT_EQ(size_t(0), ht.size());
            ASSERT_TRUE(ht.empty());
            ASSERT_EQ(0, ht.createTime());
            ASSERT_EQ(0, ht.updateTime());
            ASSERT_EQ(0, ht.upgradeTime());
            ASSERT_EQ(kEmpty, ht.toString())<<ht.toString();

            ht.uninit();
            ASSERT_FALSE(ht.valid());
        }{
            __HT ht;
            ASSERT_FALSE(ht.valid());

            ASSERT_FALSE(ht.init(&buf[0], buf.size(), kCapacity, kRow, kValueLen));
            ASSERT_FALSE(ht.valid());
            ASSERT_EQ(0, ht.rowSize());
            ASSERT_EQ(size_t(0), ht.capacity());
            for(int i = 0;i < ht.rowSize();++i)
                ASSERT_EQ(size_t(0), ht.sizeOfRow(i));
            ASSERT_EQ(size_t(0), ht.size());
            ASSERT_TRUE(ht.empty());
            ASSERT_EQ(0, ht.createTime());
            ASSERT_EQ(0, ht.updateTime());
            ASSERT_EQ(0, ht.upgradeTime());
            ASSERT_EQ(kEmpty, ht.toString())<<ht.toString();

            ht.uninit();
            ASSERT_FALSE(ht.valid());
        }
    }{
        vector<char> buf(bufSz, 'a');

        __HT ht;
        ASSERT_FALSE(ht.valid());

        ASSERT_TRUE(ht.init(&buf[0], buf.size(), kCapacity, kRow, kValueLen, true));
        ASSERT_TRUE(ht.valid());
        ASSERT_EQ(kRow, ht.rowSize());
        ASSERT_LE(kCapacity, ht.capacity());
        for(int i = 0;i < ht.rowSize();++i)
            ASSERT_EQ(size_t(0), ht.sizeOfRow(i));
        ASSERT_EQ(size_t(0), ht.size());
        ASSERT_TRUE(ht.empty());
        ASSERT_EQ(time(NULL), ht.createTime());
        ASSERT_EQ(time(NULL), ht.updateTime());
        ASSERT_EQ(0, ht.upgradeTime());
        ASSERT_NE(kEmpty, ht.toString())<<ht.toString();

        ht.uninit();
        ASSERT_FALSE(ht.valid());
        {
            __HT ht2;
            ASSERT_FALSE(ht2.valid());

            ASSERT_TRUE(ht2.init(&buf[0], buf.size()));
            ASSERT_TRUE(ht2.valid());
            ASSERT_EQ(kRow, ht2.rowSize());
            ASSERT_LE(kCapacity, ht2.capacity());
            for(int i = 0;i < ht2.rowSize();++i)
                ASSERT_EQ(size_t(0), ht2.sizeOfRow(i));
            ASSERT_EQ(size_t(0), ht2.size());
            ASSERT_TRUE(ht2.empty());
            ASSERT_EQ(time(NULL), ht2.createTime());
            ASSERT_EQ(time(NULL), ht2.updateTime());
            ASSERT_EQ(0, ht2.upgradeTime());
            ASSERT_NE(kEmpty, ht2.toString())<<ht2.toString();

            ht2.uninit();
            ASSERT_FALSE(ht2.valid());
        }{
            __HT ht2;
            ASSERT_FALSE(ht2.valid());

            ASSERT_TRUE(ht2.init(&buf[0], buf.size(), kCapacity, kRow, kValueLen, false));
            ASSERT_TRUE(ht2.valid());
            ASSERT_EQ(kRow, ht2.rowSize());
            ASSERT_LE(kCapacity, ht2.capacity());
            for(int i = 0;i < ht2.rowSize();++i)
                ASSERT_EQ(size_t(0), ht2.sizeOfRow(i));
            ASSERT_EQ(size_t(0), ht2.size());
            ASSERT_TRUE(ht2.empty());
            ASSERT_EQ(time(NULL), ht2.createTime());
            ASSERT_EQ(time(NULL), ht2.updateTime());
            ASSERT_EQ(0, ht2.upgradeTime());
            ASSERT_NE(kEmpty, ht2.toString())<<ht2.toString();

            ht2.uninit();
            ASSERT_FALSE(ht2.valid());
        }
    }
}

TEST(CAtomicHashTable, basic)
{
    const size_t bufSz = __HT::CalcBufSize(kCapacity, kRow, kValueLen);
    ASSERT_LE(kCapacity * (sizeof(__Key) + kValueLen), bufSz);
    const int kRealLen = (kValueLen + 7) / 8 * 8;
    const int kCheckLen[] = {
        0,
        1,
        kRealLen,
        kRealLen + 1,
        2 * kRealLen + sizeof(__Key),
        2 * kRealLen + sizeof(__Key) + 1,
        3 * kRealLen + 2 * sizeof(__Key),
        3 * kRealLen + 2 * sizeof(__Key) + 1,
        4 * kRealLen + 3 * sizeof(__Key),
        4 * kRealLen + 3 * sizeof(__Key) + 1,
        5 * kRealLen + 4 * sizeof(__Key),
    };
    const int kCheckNode[] = {
        1, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5
    };
    ASSERT_TRUE(ARRAY_SIZE(kCheckLen) == ARRAY_SIZE(kCheckNode));

    for(size_t i = 0;i < ARRAY_SIZE(kCheckLen);++i){
        const int len = kCheckLen[i];
        const int nodes = kCheckNode[i];
        const string value(len, 'a');

        vector<char> buf(bufSz, 'a');
        const time_t ctime = time(NULL);
        __HT ht(&buf[0], buf.size(), kCapacity, kRow, kValueLen, true);

        ASSERT_TRUE(ht.valid());
        ASSERT_EQ(size_t(0), ht.size());
        ASSERT_TRUE(ht.empty());

        size_t sz = 0;
        int j = 0;
        for(;sz < kCapacity / 3;++j){
            ASSERT_TRUE(ht.insert(j, value))<<"insert i="<<i<<", j="<<j<<", ht="<<ht.toString();
            sz += nodes;
            ASSERT_EQ(sz, ht.size());
            size_t sum = 0;
            for(int k = 0;k < ht.rowSize();++k)
                sum += ht.sizeOfRow(k);
            ASSERT_EQ(sz, sum);
        }

        ASSERT_FALSE(ht.empty());

        for(--j;sz > 0;--j){
            ASSERT_EQ(size_t(nodes), ht.remove(j))<<"remove i="<<i<<", j="<<j<<", ht="<<ht.toString();
            sz -= nodes;
            ASSERT_EQ(sz, ht.size())<<"remove i="<<i<<", j="<<j<<", ht="<<ht.toString();
            size_t sum = 0;
            for(int k = 0;k < ht.rowSize();++k)
                sum += ht.sizeOfRow(k);
            ASSERT_EQ(sz, sum);
        }

        ASSERT_EQ(size_t(0), ht.size());
        ASSERT_TRUE(ht.empty());
        ASSERT_EQ(ctime, ht.createTime());
        ASSERT_EQ(time(NULL), ht.updateTime());
    }
}

static void next1(string & v, char & c)
{
    if(v.size() > 1000)
        v.clear();
    else{
        v.push_back(c);
        ++c;
    }
}

static void next2(string & v, char & c)
{
    if(v.size() > 321)
        v.clear();
    else{
        v.push_back(c);
        c = (c + 1) * (c + 2);
    }
}

TEST(CAtomicHashTable, ST)
{
    const size_t bufSz = __HT::CalcBufSize(kCapacity, kRow, kValueLen);
    ASSERT_LE(kCapacity * (sizeof(__Key) + kValueLen), bufSz);

    vector<char> buf(bufSz, 'a');
    __HT htt(&buf[0], buf.size(), kCapacity, kRow, kValueLen, true);
    ASSERT_TRUE(htt.valid());
    ASSERT_EQ(size_t(0), htt.size());
    ASSERT_TRUE(htt.empty());
    int key = 0;
    size_t sz = 0;
    {   //insert
        __HT & ht = htt;
        string value;
        char ch = 'a';
        for(;;++key, next1(value, ch)){
            bool ret = (0 == (key % 2) ?
                    ht.insert(key, value) :
                    ht.insert(key, value.c_str(), value.size()));
            if(!ret)
                break;
        }
        ASSERT_LT(0, key);
        ASSERT_TRUE(ht.valid());
        ASSERT_NE(size_t(0), ht.size());
        ASSERT_FALSE(ht.empty());
        sz = ht.size();
    }{  //check 1
        const __HT & ht = htt;
        string value;
        char ch = 'a';
        for(int k = 0;k < key;++k, next1(value, ch)){
            ASSERT_TRUE(ht.has(k));
            string v;
            if(0 == (k % 3)){
                v.resize(value.size() + (k % 5));
                size_t len = v.size();
                ASSERT_TRUE(ht.get(k, &v[0], len))<<"k="<<k<<", ht="<<ht.toString();
                v.resize(len);
            }else{
                ASSERT_TRUE(ht.get(k, v));
            }
            ASSERT_EQ(value.size(), v.size());
            ASSERT_EQ(value, v);
        }
        ASSERT_TRUE(ht.valid());
        ASSERT_EQ(sz, ht.size());
        ASSERT_FALSE(ht.empty());
    }{  //update
        __HT & ht = htt;
        string value = "12345";
        char ch = 'z';
        for(int k = 0;k < key;++k, next2(value, ch)){
            if(0 == (k % 7)){
                ASSERT_TRUE(ht.update(k, value))<<"k="<<k<<", value.size()="<<value.size()<<"< ht="<<ht.toString();
            }else
                ASSERT_TRUE(ht.update(k, value.c_str(), value.size()))<<"k="<<k<<", value.size()="<<value.size()<<", ht="<<ht.toString();
        }
        ASSERT_TRUE(ht.valid());
        ASSERT_NE(size_t(0), ht.size());
        ASSERT_FALSE(ht.empty());
        sz = ht.size();
    }{  //check 2
        const __HT & ht = htt;
        string value = "12345";
        char ch = 'z';
        for(int k = 0;k < key;++k, next2(value, ch)){
            ASSERT_TRUE(ht.has(k));
            string v;
            if(0 == (k % 11)){
                v.resize(value.size() + (k % 13));
                size_t len = v.size();
                ASSERT_TRUE(ht.get(k, &v[0], len))<<"k="<<k<<", ht="<<ht.toString();
                v.resize(len);
            }else{
                ASSERT_TRUE(ht.get(k, v));
            }
            ASSERT_EQ(value, v)<<"k="<<k<<", value="<<tools::Dump(value)<<", v="<<tools::Dump(v);
        }
        ASSERT_TRUE(ht.valid());
        ASSERT_EQ(sz, ht.size());
        ASSERT_FALSE(ht.empty());
    }{  //rm
        __HT & ht = htt;
        for(int k = 0;k < key;++k)
            ht.remove(k);
        ASSERT_TRUE(ht.valid());
        ASSERT_EQ(size_t(0), ht.size());
        ASSERT_TRUE(ht.empty());
    }{  //check 3
        const __HT & ht = htt;
        for(int k = 0;k < key;++k){
            ASSERT_FALSE(ht.has(k));
            string v;
            if(0 == (k % 17)){
                v.resize(k % 19);
                size_t len = v.size();
                ASSERT_FALSE(ht.get(k, &v[0], len))<<"k="<<k<<", ht="<<ht.toString();
            }else{
                ASSERT_FALSE(ht.get(k, v));
            }
        }
    }
}

struct CTask
{
    string value_;
    int op_;    //0: insert; 1: update; 2: remove
    int key_;
    //functions
    CTask(){}
    CTask(int op, int k, const string & v)
        : value_(v)
        , op_(op)
        , key_(k)
    {}
    CTask(int op, int k)
        : op_(op)
        , key_(k)
    {}
    string toString() const{
        std::ostringstream oss;
        oss<<"{op_="<<op_
            <<", key_="<<key_
            <<", value_.size()="<<value_.size()
            <<"}";
        return oss.str();
    }
};

typedef CLockQueue<CTask> __Queue;

static vector<char> g_buf;

class CWriteThreads : public CThreadManager<CTask>
{
    typedef CThreadManager<CTask> __MyBase;
public:
    explicit CWriteThreads()
        : ht_(&g_buf[0], g_buf.size())
        , icnt_(0)
        , ucnt_(0)
        , rcnt_(0)
        , assert_(false)
    {
        __MyBase::threadCountMin(1);
        __MyBase::threadCountMax(1);
    }
    void noAssert(){assert_ = false;}
protected:
    virtual void run(__Task & task){
        if(assert_){
            switch(task.op_){
                case 0:
                    if(0 == (task.key_ % 2))
                        ASSERT_TRUE(ht_.insert(task.key_, task.value_));
                    else
                        ASSERT_TRUE(ht_.insert(task.key_, task.value_.c_str(), task.value_.size()));
                    break;
                case 1:
                    if(0 == (task.key_ % 3))
                        ASSERT_TRUE(ht_.update(task.key_, task.value_));
                    else
                        ASSERT_TRUE(ht_.update(task.key_, task.value_.c_str(), task.value_.size()))<<"task="<<task.toString()<<", ht="<<ht_.toString();
                    break;
                case 2:
                    ht_.remove(task.key_);
                    break;
                default:
                    FAIL()<<"task="<<task.toString();
            }
        }else{
            if(task.key_ > int(kKeyRange))
                WARN("kKeyRange="<<kKeyRange<<", invalid task="<<task.toString());
            bool succ = false;
            switch(task.op_){
                case 0:
                    if(ht_.has(task.key_))
                        break;
                    if(0 == (task.key_ % 2)){
                        succ = ht_.insert(task.key_, task.value_);
                    }else
                        succ = ht_.insert(task.key_, task.value_.c_str(), task.value_.size());
                    if(succ)
                        if(0 == (++icnt_ & ((1 << 20) - 1)))
                            INFO("insert cnt="<<icnt_);
                    break;
                case 1:
                    if(0 == (task.key_ % 3))
                        succ = ht_.update(task.key_, task.value_);
                    else
                        succ = ht_.update(task.key_, task.value_.c_str(), task.value_.size());
                    if(succ)
                        if(0 == (++ucnt_ & ((1 << 20) - 1)))
                            INFO("update cnt="<<ucnt_);
                    break;
                case 2:
                    succ = ht_.has(task.key_);
                    ht_.remove(task.key_);
                    if(succ)
                        if(0 == (++rcnt_ & ((1 << 20) - 1)))
                            INFO("remove cnt="<<rcnt_);
                    break;
                default:
                    ASSERT(false, "task="<<task.toString());
            }
        }
    }
private:
    __HT ht_;
    size_t icnt_, ucnt_, rcnt_;
    bool assert_;
};

class CReadThreads : public CThreadManager<CTask>
{
    typedef CThreadManager<CTask> __MyBase;
public:
    explicit CReadThreads()
        : ht_(&g_buf[0], g_buf.size())
    {
        __MyBase::threadCountMin(1);
        __MyBase::threadCountMax(1);
    }
protected:
    virtual void run(__Task & task){
        const int k = task.key_;
        const string & value = task.value_;
        const __HT & ht = ht_;
        string v;
        switch(task.op_){
            case 0:
            case 1:
                ASSERT_TRUE(ht.has(k))<<"k="<<k;
                if(0 == (k % 5)){
                    ASSERT_TRUE(ht.get(k, v));
                }else{
                    v.resize(value.size() + (k % 7));
                    size_t len = v.size();
                    ASSERT_TRUE(ht.get(k, &v[0], len))<<"k="<<k<<", ht="<<ht.toString();
                    v.resize(len);
                }
                ASSERT_EQ(value, v)<<"k="<<k<<", value="<<tools::Dump(value)<<", v="<<tools::Dump(v);
                break;
            case 2:
                ASSERT_FALSE(ht.has(k));
                if(0 == (k % 11)){
                    ASSERT_FALSE(ht.get(k, v));
                }else{
                    v.resize(k % 13);
                    size_t len = v.size();
                    ASSERT_FALSE(ht.get(k, &v[0], len))<<"k="<<k<<", ht="<<ht.toString();
                }
                break;
            default:
                FAIL();
        }
    }
private:
    __HT ht_;
};

struct CKeyValue
{
    string value_;
    int key_;
    char ch_;
    //functions
    CKeyValue(const string & v, int k, char c)
        : value_(v)
        , key_(k)
        , ch_(c)
    {}
    CKeyValue(int k, char c)
        : value_(Gen(k, c))
        , key_(k)
        , ch_(c)
    {}
    void next(){
        ++key_;
        next2(value_, ch_);
    }
    void gen(bool v = true){
        if(++key_ > int(kKeyRange))
            key_ = 0;
        if(v)
            value_ = Gen(key_, ch_);
        ch_ = (ch_ - 1) * (ch_ - 2);
    }
    static string Gen(int k, char c){
        const int len = ((k + (int(c) & 0xFF)) & 0x7F);
        string v(len, 0);
        for(int i = 0;i < len;++i)
            v[i] = c + i;
        return v;
    }
    static bool Test(int k, const string & v){
        if(v.empty())
            return true;
        const char c = v[0];
        const int len = ((k + (int(c) & 0xFF)) & 0x7F);
        if(len != int(v.size()))
            return false;
        for(int i = 0;i < len;++i)
            if((int(v[i]) & 0xFF) != ((c + i) & 0xFF))
                return false;
        return true;
    }
};

static __Queue g_wque[THREAD_COUNT_W];
static __Queue g_rque[THREAD_COUNT_R];
static CAtomicSync<int> g_index;

static void readTest()
{
    const __HT ht(&g_buf[0], g_buf.size());
    size_t cnt = 0;
    for(int k = 0;;++k){
        if(k > int(kKeyRange))
            k = 0;
        string v;
        if(ht.get(k, v)){
            if(v.empty())
                continue;
            if(!CKeyValue::Test(k, v))
                ERROR("k="<<k<<", v="<<tools::Dump(v)<<", v2="<<tools::Dump(CKeyValue::Gen(k, v[0])));
            ++cnt;
            if(0 == (cnt & ((1 << 23) - 1))){
                INFO("get cnt="<<cnt);
                sleep(0);
            }
        }
    }
}

static void * readProc(void * arg)
{
    readTest();
    return NULL;
}

TEST(CAtomicHashTable, MT)
{
    {//init
        const size_t bufSz = __HT::CalcBufSize(kCapa2, kRow, kValueLen);
        ASSERT_LE(kCapa2 * (sizeof(__Key) + kValueLen), bufSz);
        g_buf.resize(bufSz, 'a');
        __HT ht(&g_buf[0], g_buf.size(), kCapa2, kRow, kValueLen, true);
        ASSERT_TRUE(ht.valid());
        ASSERT_EQ(size_t(0), ht.size());
        ASSERT_TRUE(ht.empty());
    }
    //launch read & write threads
    CWriteThreads * thw[THREAD_COUNT_W];
    for(int i = 0;i < THREAD_COUNT_W;++i){
        thw[i] = new CWriteThreads();
        thw[i]->startThreads(g_wque[i]);
    }
    CReadThreads * thr[THREAD_COUNT_R];
    for(int i = 0;i < THREAD_COUNT_R;++i){
        thr[i] = new CReadThreads();
        thr[i]->startThreads(g_rque[i]);
    }
    sleep(1);
    int count = 0;
    {   //insert
        __HT ht(&g_buf[0], g_buf.size());
        CKeyValue ikv("213234", 0, 'Z');
        for(;;ikv.next())
            if(!ht.insert(ikv.key_, ikv.value_))
                break;
        ASSERT_FALSE(ht.empty());
        count = ikv.key_;
        for(int k = 0;k < count;++k)
            ht.remove(k);
        ASSERT_TRUE(ht.empty());
        count /= 4;
        count *= 3;
    }{//insert
        const int OP = 0;
        CKeyValue ikv("213", 0, 'Z'), ikv2(ikv);
        for(int i = 0;i < count;ikv.next(), ++i)
            ASSERT_TRUE(g_wque[ikv.key_ % THREAD_COUNT_W].push(CTask(OP, ikv.key_, ikv.value_)));

        const __HT ht(&g_buf[0], g_buf.size());
        for(int i = 0;i < count;ikv2.next(), ++i){
            for(int j = 0;j < THREAD_COUNT_R;++j)
                ASSERT_TRUE(g_rque[j].push(CTask(OP, ikv2.key_, ikv2.value_)));
            const int k = ikv2.key_;
            const string & value = ikv2.value_;
            string v;
            ASSERT_TRUE(ht.has(k))<<"k="<<k;
            ASSERT_TRUE(ht.get(k, v));
            ASSERT_EQ(value, v)<<"k="<<k<<", value="<<tools::Dump(value)<<", v="<<tools::Dump(v);
        }
        bool left = false;
        do{
            left = false;
            for(int i = 0;i < THREAD_COUNT_R;++i)
                if(!g_rque[i].empty()){
                    sleep(0);
                    left = true;
                    break;
                }
        }while(left);
        ASSERT_FALSE(ht.empty());
    }{//update
        const int OP = 1;
        CKeyValue ikv("ggdag", 0, 'a'), ikv2(ikv);
        for(int i = 0;i < count;ikv.next(), ++i)
            ASSERT_TRUE(g_wque[ikv.key_ % THREAD_COUNT_W].push(CTask(OP, ikv.key_, ikv.value_)));

        const __HT ht(&g_buf[0], g_buf.size());
        for(int i = 0;i < count;ikv2.next(), ++i){
            for(int j = 0;j < THREAD_COUNT_R;++j)
                ASSERT_TRUE(g_rque[j].push(CTask(OP, ikv2.key_, ikv2.value_)));
            const int k = ikv2.key_;
            const string & value = ikv2.value_;
            string v;
            ASSERT_TRUE(ht.has(k))<<"k="<<k;
            ASSERT_TRUE(ht.get(k, v));
            ASSERT_EQ(value, v)<<"k="<<k<<", value="<<tools::Dump(value)<<", v="<<tools::Dump(v);
        }
        bool left = false;
        do{
            left = false;
            for(int i = 0;i < THREAD_COUNT_R;++i)
                if(!g_rque[i].empty()){
                    sleep(0);
                    left = true;
                    break;
                }
        }while(left);
        ASSERT_FALSE(ht.empty());
    }{//remove
        const int OP = 2;
        for(int k = 0;k < count;++k)
            ASSERT_TRUE(g_wque[k % THREAD_COUNT_W].push(CTask(OP, k)));

        const __HT ht(&g_buf[0], g_buf.size());
        for(int k = 0;k < count;++k){
            for(int j = 0;j < THREAD_COUNT_R;++j)
                ASSERT_TRUE(g_rque[j].push(CTask(OP, k)));
            string v;
            ASSERT_FALSE(ht.has(k))<<"k="<<k;
            ASSERT_FALSE(ht.get(k, v));
        }
        bool left = false;
        do{
            left = false;
            for(int i = 0;i < THREAD_COUNT_R;++i)
                if(!g_rque[i].empty()){
                    sleep(0);
                    left = true;
                    break;
                }
        }while(left);
        ASSERT_TRUE(ht.empty());
    }
    //endless test
    if(ENDLESS){
        LOGGER_INIT("./atomic_hash_table_unittest.log", "INFO", 10 << 20, 10);
        LOGGER_CRASH_HANDLER();
        {
            CKeyValue kv(0, 'x');
            for(int i = 0;i < int(kKeyRange);++i)
                ASSERT_TRUE(g_wque[kv.key_ % THREAD_COUNT_W].push(CTask(0, kv.key_, kv.value_)));
        }
        sleep(1);
        for(int i = 0;i < THREAD_COUNT_W;++i)
            thw[i]->noAssert();
        {
            cout<<"start endless test\n";
            const __HT ht(&g_buf[0], g_buf.size());
            CThreadAttr attr;
            attr.detach(true);
            std::vector<CThread> thr(THREAD_COUNT_R);
            for(size_t j = 0;j < thr.size();++j)
                thr[j].start(readProc, NULL, attr);
            CKeyValue ikv(kKeyRange - 1, 'd');
            CKeyValue ukv(kKeyRange / 2, 'e');
            CKeyValue rkv(kCapa2 / 10, 'f');
            for(unsigned int i = 0;;++i, ikv.gen(), ukv.gen(), rkv.gen(false)){
                ASSERT_TRUE(g_wque[ikv.key_ % THREAD_COUNT_W].push(CTask(0, ikv.key_, ikv.value_)));
                ASSERT_TRUE(g_wque[ukv.key_ % THREAD_COUNT_W].push(CTask(1, ukv.key_, ukv.value_)));
                ASSERT_TRUE(g_wque[rkv.key_ % THREAD_COUNT_W].push(CTask(2, rkv.key_)));
                if(0 == (i & ((1 << 20) - 1)))
                    INFO("push cnt="<<i);
                if(0 == (i & ((1 << 23) - 1)))
                    INFO("ht="<<ht.toString());
            }
        }
    }
}
