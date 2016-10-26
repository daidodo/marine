#include <marine/atomic_hash_table.hh>

#include <vector>
#include <marine/tools/system.hh>
#include "../pf_inc.h"

#if 0
#   define CAPACITY    (30UL << 20)
#else
#   define CAPACITY    (30UL << 10)
#endif
#define ROW         30
#define VALUE_LEN   128

typedef CAtomicHashTable<uint64_t>  __HT;

static vector<char> g_buf;
static uint64_t key;
static string value_1;
static string v;
__HT g_ht;

PFINIT()
{
    size_t sz = __HT::CalcBufSize(CAPACITY, ROW, VALUE_LEN);
    cout<<"memory size: "<<sz<<endl;
    g_buf.resize(sz);
}

//create
PFTEST(CAtomicHashTable, create)
{
    __HT ht(&g_buf[0], g_buf.size(), CAPACITY, ROW, VALUE_LEN, true);
}

//no_create
PFINIT_T(CAtomicHashTable, no_create)
{
    __HT ht(&g_buf[0], g_buf.size(), CAPACITY, ROW, VALUE_LEN, true);
}

PFTEST(CAtomicHashTable, no_create)
{
    __HT ht(&g_buf[0], g_buf.size(), CAPACITY, ROW, VALUE_LEN, false);
}

//insert_1
PFINIT_T(CAtomicHashTable, insert_1)
{
    g_ht.init(&g_buf[0], g_buf.size(), CAPACITY, ROW, VALUE_LEN, true);
    value_1.resize(VALUE_LEN, 'a');
}

PFTEST(CAtomicHashTable, insert_1)
{
    if(!g_ht.insert(key++ * pf_thread_count + pf_thread_index, value_1))
        pf_thread_finish = true;    //thread finish
}

PFUNINIT_T(CAtomicHashTable, insert_1)
{
    cout<<"ht="<<g_ht.toString()<<endl;
}

//get_1
PFINIT_T(CAtomicHashTable, get_1)
{
    g_ht.init(&g_buf[0], g_buf.size(), CAPACITY / 100, ROW, VALUE_LEN, true);
    value_1.resize(VALUE_LEN, 'a');
    for(;;)
        if(!g_ht.insert(key++, value_1)){
            key = 0;
            break;
        }
}

PFTEST(CAtomicHashTable, get_1)
{
    if(!g_ht.get(key++, value_1))
        key = 0;
}
