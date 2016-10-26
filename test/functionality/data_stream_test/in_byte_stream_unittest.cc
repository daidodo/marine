#include <marine/data_stream.hh>

#include <list>
#include <deque>
#include <set>
#include <map>
#include "../inc.h"
#include "byte_order.h"
#include "test.pb.h"

TEST(CInByteStream, ctor)
{
    std::string str("abcdef");
    {
        CInByteStream ds;
        ASSERT_NE(0, ds.status());
    }{
        const char * buf = str.c_str();
        {
            CInByteStream ds(buf, str.size());
            ASSERT_EQ(0, ds.status());
            ASSERT_EQ(str.c_str(), ds.data());
            ASSERT_TRUE(0 == ds.cur());
            ASSERT_EQ(str.size(), ds.left());
            ASSERT_EQ(le(true), ds.littleEndian());
        }{
            CInByteStream ds(buf, str.size(), true);
            ASSERT_EQ(0, ds.status());
            ASSERT_EQ(str.c_str(), ds.data());
            ASSERT_TRUE(0 == ds.cur());
            ASSERT_EQ(str.size(), ds.left());
            ASSERT_EQ(le(true), ds.littleEndian());
        }{
            CInByteStream ds(buf, str.size(), false);
            ASSERT_EQ(0, ds.status());
            ASSERT_EQ(str.c_str(), ds.data());
            ASSERT_TRUE(0 == ds.cur());
            ASSERT_EQ(str.size(), ds.left());
            ASSERT_EQ(le(false), ds.littleEndian());
        }
    }{
        const unsigned char * buf = reinterpret_cast<const unsigned char *>(str.c_str());
        {
            CInByteStream ds(buf, str.size());
            ASSERT_EQ(0, ds.status());
            ASSERT_EQ(str.c_str(), ds.data());
            ASSERT_TRUE(0 == ds.cur());
            ASSERT_EQ(str.size(), ds.left());
            ASSERT_EQ(le(true), ds.littleEndian());
        }{
            CInByteStream ds(buf, str.size(), true);
            ASSERT_EQ(0, ds.status());
            ASSERT_EQ(str.c_str(), ds.data());
            ASSERT_TRUE(0 == ds.cur());
            ASSERT_EQ(str.size(), ds.left());
            ASSERT_EQ(le(true), ds.littleEndian());
        }{
            CInByteStream ds(buf, str.size(), false);
            ASSERT_EQ(0, ds.status());
            ASSERT_EQ(str.c_str(), ds.data());
            ASSERT_TRUE(0 == ds.cur());
            ASSERT_EQ(str.size(), ds.left());
            ASSERT_EQ(le(false), ds.littleEndian());
        }
    }{
        const signed char * buf = reinterpret_cast<const signed char *>(str.c_str());
        {
            CInByteStream ds(buf, str.size());
            ASSERT_EQ(0, ds.status());
            ASSERT_EQ(str.c_str(), ds.data());
            ASSERT_TRUE(0 == ds.cur());
            ASSERT_EQ(str.size(), ds.left());
            ASSERT_EQ(le(true), ds.littleEndian());
        }{
            CInByteStream ds(buf, str.size(), true);
            ASSERT_EQ(0, ds.status());
            ASSERT_EQ(str.c_str(), ds.data());
            ASSERT_TRUE(0 == ds.cur());
            ASSERT_EQ(str.size(), ds.left());
            ASSERT_EQ(le(true), ds.littleEndian());
        }{
            CInByteStream ds(buf, str.size(), false);
            ASSERT_EQ(0, ds.status());
            ASSERT_EQ(str.c_str(), ds.data());
            ASSERT_TRUE(0 == ds.cur());
            ASSERT_EQ(str.size(), ds.left());
            ASSERT_EQ(le(false), ds.littleEndian());
        }
    }{
        const std::string & buf = str;
        {
            CInByteStream ds(buf);
            ASSERT_EQ(0, ds.status());
            ASSERT_EQ(str.c_str(), ds.data());
            ASSERT_TRUE(0 == ds.cur());
            ASSERT_EQ(str.size(), ds.left());
            ASSERT_EQ(le(true), ds.littleEndian());
        }{
            CInByteStream ds(buf, true);
            ASSERT_EQ(0, ds.status());
            ASSERT_EQ(str.c_str(), ds.data());
            ASSERT_TRUE(0 == ds.cur());
            ASSERT_EQ(str.size(), ds.left());
            ASSERT_EQ(le(true), ds.littleEndian());
        }{
            CInByteStream ds(buf, false);
            ASSERT_EQ(0, ds.status());
            ASSERT_EQ(str.c_str(), ds.data());
            ASSERT_TRUE(0 == ds.cur());
            ASSERT_EQ(str.size(), ds.left());
            ASSERT_EQ(le(false), ds.littleEndian());
        }
    }
}

TEST(CInByteStream, setSource)
{
    std::string str("abcdef");
    {
        const char * buf = str.c_str();
        {
            CInByteStream ds;
            ASSERT_NE(0, ds.status());

            ds.setSource(buf, str.size());
            ASSERT_EQ(0, ds.status());
            ASSERT_EQ(str.c_str(), ds.data());
            ASSERT_TRUE(0 == ds.cur());
            ASSERT_EQ(str.size(), ds.left());
            ASSERT_EQ(le(true), ds.littleEndian());
        }{
            CInByteStream ds;
            ASSERT_NE(0, ds.status());

            ds.setSource(buf, str.size(), true);
            ASSERT_EQ(0, ds.status());
            ASSERT_EQ(str.c_str(), ds.data());
            ASSERT_TRUE(0 == ds.cur());
            ASSERT_EQ(str.size(), ds.left());
            ASSERT_EQ(le(true), ds.littleEndian());
        }{
            CInByteStream ds;
            ASSERT_NE(0, ds.status());

            ds.setSource(buf, str.size(), false);
            ASSERT_EQ(0, ds.status());
            ASSERT_EQ(str.c_str(), ds.data());
            ASSERT_TRUE(0 == ds.cur());
            ASSERT_EQ(str.size(), ds.left());
            ASSERT_EQ(le(false), ds.littleEndian());
        }
    }{
        const unsigned char * buf = reinterpret_cast<const unsigned char *>(str.c_str());
        {
            CInByteStream ds;
            ASSERT_NE(0, ds.status());

            ds.setSource(buf, str.size());
            ASSERT_EQ(0, ds.status());
            ASSERT_EQ(str.c_str(), ds.data());
            ASSERT_TRUE(0 == ds.cur());
            ASSERT_EQ(str.size(), ds.left());
            ASSERT_EQ(le(true), ds.littleEndian());
        }{
            CInByteStream ds;
            ASSERT_NE(0, ds.status());

            ds.setSource(buf, str.size(), true);
            ASSERT_EQ(0, ds.status());
            ASSERT_EQ(str.c_str(), ds.data());
            ASSERT_TRUE(0 == ds.cur());
            ASSERT_EQ(str.size(), ds.left());
            ASSERT_EQ(le(true), ds.littleEndian());
        }{
            CInByteStream ds;
            ASSERT_NE(0, ds.status());

            ds.setSource(buf, str.size(), false);
            ASSERT_EQ(0, ds.status());
            ASSERT_EQ(str.c_str(), ds.data());
            ASSERT_TRUE(0 == ds.cur());
            ASSERT_EQ(str.size(), ds.left());
            ASSERT_EQ(le(false), ds.littleEndian());
        }
    }{
        const signed char * buf = reinterpret_cast<const signed char *>(str.c_str());
        {
            CInByteStream ds;
            ASSERT_NE(0, ds.status());

            ds.setSource(buf, str.size());
            ASSERT_EQ(0, ds.status());
            ASSERT_EQ(str.c_str(), ds.data());
            ASSERT_TRUE(0 == ds.cur());
            ASSERT_EQ(str.size(), ds.left());
            ASSERT_EQ(le(true), ds.littleEndian());
        }{
            CInByteStream ds;
            ASSERT_NE(0, ds.status());

            ds.setSource(buf, str.size(), true);
            ASSERT_EQ(0, ds.status());
            ASSERT_EQ(str.c_str(), ds.data());
            ASSERT_TRUE(0 == ds.cur());
            ASSERT_EQ(str.size(), ds.left());
            ASSERT_EQ(le(true), ds.littleEndian());
        }{
            CInByteStream ds;
            ASSERT_NE(0, ds.status());

            ds.setSource(buf, str.size(), false);
            ASSERT_EQ(0, ds.status());
            ASSERT_EQ(str.c_str(), ds.data());
            ASSERT_TRUE(0 == ds.cur());
            ASSERT_EQ(str.size(), ds.left());
            ASSERT_EQ(le(false), ds.littleEndian());
        }
    }{
        const std::string & buf = str;
        {
            CInByteStream ds;
            ASSERT_NE(0, ds.status());

            ds.setSource(buf);
            ASSERT_EQ(0, ds.status());
            ASSERT_EQ(str.c_str(), ds.data());
            ASSERT_TRUE(0 == ds.cur());
            ASSERT_EQ(str.size(), ds.left());
            ASSERT_EQ(le(true), ds.littleEndian());
        }{
            CInByteStream ds;
            ASSERT_NE(0, ds.status());

            ds.setSource(buf, true);
            ASSERT_EQ(0, ds.status());
            ASSERT_EQ(str.c_str(), ds.data());
            ASSERT_TRUE(0 == ds.cur());
            ASSERT_EQ(str.size(), ds.left());
            ASSERT_EQ(le(true), ds.littleEndian());
        }{
            CInByteStream ds;
            ASSERT_NE(0, ds.status());

            ds.setSource(buf, false);
            ASSERT_EQ(0, ds.status());
            ASSERT_EQ(str.c_str(), ds.data());
            ASSERT_TRUE(0 == ds.cur());
            ASSERT_EQ(str.size(), ds.left());
            ASSERT_EQ(le(false), ds.littleEndian());
        }
    }
}

TEST(CInByteStream, status)
{
    CInByteStream ds;
    for(int i = 0;i < 100;++i){
        ds.status(i);
        ASSERT_EQ(i, ds.status());
        ASSERT_EQ(0 != i, !ds);
        ASSERT_EQ(0 == i, bool(ds));
        ASSERT_EQ(0 == i, ds.good());
    }
    for(int i = 0;i < 100;++i){
        ASSERT_EQ(&ds, &ds.bad(i));
        ASSERT_EQ(i, ds.status());
        ASSERT_EQ(0 != i, !ds);
        ASSERT_EQ(0 == i, bool(ds));
        ASSERT_EQ(0 == i, ds.good());
    }
}

TEST(CInByteStream, byte_order)
{
    CInByteStream ds;

    ds.netByteOrder(true);
    ASSERT_EQ(le(true), ds.littleEndian());

    ds.netByteOrder(false);
    ASSERT_EQ(le(false), ds.littleEndian());

    ds.littleEndian(true);
    ASSERT_TRUE(ds.littleEndian());

    ds.littleEndian(false);
    ASSERT_FALSE(ds.littleEndian());

    ASSERT_EQ(&ds, &(ds>>Manip::net_order));
    ASSERT_EQ(le(true), ds.littleEndian());

    ASSERT_EQ(&ds, &(ds>>Manip::host_order));
    ASSERT_EQ(le(false), ds.littleEndian());

    ASSERT_EQ(&ds, &(ds>>Manip::little_endian));
    ASSERT_TRUE(ds.littleEndian());

    ASSERT_EQ(&ds, &(ds>>Manip::big_endian));
    ASSERT_FALSE(ds.littleEndian());
}

TEST(CInByteStream, seek)
{
    std::string str("abcdef");
    {
        CInByteStream ds(str);
        size_t i = 0;
        for(;i <= str.size();++i){
            ASSERT_EQ(i, size_t(ds.seek(i)));
            ASSERT_EQ(0, ds.status());
            ASSERT_EQ(i, ds.cur());
            ASSERT_EQ(str.c_str() + i, ds.data());
            ASSERT_EQ(str.size() - i, ds.left());
        }
        for(;i <= str.size() + 100;++i, ds.status(0)){
            ASSERT_EQ(0, ds.status());
            ASSERT_GT(0, ds.seek(i));
            ASSERT_NE(0, ds.status());
        }
    }{
        CInByteStream ds(str);
        size_t i = 0;
        for(;i <= str.size();++i){
            ASSERT_EQ(&ds, &(ds>>Manip::seek(i)));
            ASSERT_EQ(0, ds.status());
            ASSERT_EQ(i, ds.cur());
            ASSERT_EQ(str.c_str() + i, ds.data());
            ASSERT_EQ(str.size() - i, ds.left());
        }
        for(;i <= str.size() + 100;++i, ds.status(0)){
            ASSERT_EQ(0, ds.status());
            ASSERT_EQ(&ds, &(ds>>Manip::seek(i)));
            ASSERT_NE(0, ds.status());
        }
    }
}

TEST(CInByteStream, skip)
{
    std::string str;
    str.resize(55, 'a');
    {
        CInByteStream ds(str);
        size_t s = 0;
        ssize_t i = 0;
        for(;i <= 10;s += ++i){
            ASSERT_EQ(s, size_t(ds.skip(i)))<<"i="<<i;
            ASSERT_EQ(0, ds.status());
            ASSERT_EQ(s, ds.cur());
            ASSERT_EQ(str.c_str() + s, ds.data());
            ASSERT_EQ(str.size() - s, ds.left());
        }
        s = str.size();
        i = 0;
        for(;i >= -10;s += --i){
            ASSERT_EQ(s, size_t(ds.skip(i)))<<"i="<<i;
            ASSERT_EQ(0, ds.status());
            ASSERT_EQ(s, ds.cur());
            ASSERT_EQ(str.c_str() + s, ds.data());
            ASSERT_EQ(str.size() - s, ds.left());
        }
        ASSERT_EQ(0, ds.status());
        ASSERT_GT(0, ds.skip(-1));
        ASSERT_NE(0, ds.status())<<"s="<<s;

        ds.status(0);
        ASSERT_EQ(0, ds.status());
        ASSERT_GT(0, ds.skip(str.size() + 1));
        ASSERT_NE(0, ds.status());
    }{
        CInByteStream ds(str);
        size_t s = 0;
        ssize_t i = 0;
        for(;i <= 10;s += ++i){
            ASSERT_EQ(&ds, &(ds>>Manip::skip(i)));
            ASSERT_EQ(0, ds.status());
            ASSERT_EQ(s, ds.cur());
            ASSERT_EQ(str.c_str() + s, ds.data());
            ASSERT_EQ(str.size() - s, ds.left());
        }
        s = str.size();
        i = 0;
        for(;i >= -10;s += --i){
            ASSERT_EQ(&ds, &(ds>>Manip::skip(i)));
            ASSERT_EQ(0, ds.status());
            ASSERT_EQ(s, ds.cur());
            ASSERT_EQ(str.c_str() + s, ds.data());
            ASSERT_EQ(str.size() - s, ds.left());
        }
        ASSERT_EQ(0, ds.status());
        ASSERT_GT(0, ds.skip(-1));
        ASSERT_NE(0, ds.status())<<"s="<<s;

        ds.status(0);
        ASSERT_EQ(0, ds.status());
        ASSERT_EQ(&ds, &(ds>>Manip::skip(str.size() + 1)));
        ASSERT_NE(0, ds.status());
    }{
        uint16_t len = 10;
        memcpy(&str[0], &len, sizeof len);

        CInByteStream ds(str, false);
        ASSERT_EQ(0, ds.status());
        ASSERT_EQ(size_t(0), ds.cur());

        len = 0;
        ds>>len
            >>Manip::skip(&len)
            >>Manip::skip<uint32_t>(NULL);

        ASSERT_EQ(0, ds.status());
        ASSERT_EQ(sizeof len + len, ds.cur());
    }
}

template<typename T>
static void testV(T v)
{
    {
        CInByteStream ds(reinterpret_cast<const char *>(&v), sizeof v);
        ds.netByteOrder(true);
        ASSERT_EQ(0, ds.status());
        ASSERT_EQ(le(true), ds.littleEndian());

        T x = v + 1, y = v;
        ASSERT_NE(y, x);
        ASSERT_EQ(&ds, &(ds>>x));
        ASSERT_EQ(0, ds.status());
        if(swapnb(true))
            y = tools::SwapByteOrder(v);
        ASSERT_EQ(y, x);
    }{
        CInByteStream ds(reinterpret_cast<const char *>(&v), sizeof v);
        ds.netByteOrder(false);
        ASSERT_EQ(0, ds.status());
        ASSERT_EQ(le(false), ds.littleEndian());

        T x = v + 1, y = v;
        ASSERT_NE(y, x);
        ASSERT_EQ(&ds, &(ds>>x));
        ASSERT_EQ(0, ds.status());
        if(swapnb(false))
            y = tools::SwapByteOrder(v);
        ASSERT_EQ(y, x);
    }{
        CInByteStream ds(reinterpret_cast<const char *>(&v), sizeof v);
        ds.littleEndian(true);
        ASSERT_EQ(0, ds.status());
        ASSERT_TRUE(ds.littleEndian());

        T x = v + 1, y = v;
        ASSERT_NE(y, x);
        ASSERT_EQ(&ds, &(ds>>x));
        ASSERT_EQ(0, ds.status());
        if(swaple(true))
            y = tools::SwapByteOrder(v);
        ASSERT_EQ(y, x);
    }{
        CInByteStream ds(reinterpret_cast<const char *>(&v), sizeof v);
        ds.littleEndian(false);
        ASSERT_EQ(0, ds.status());
        ASSERT_FALSE(ds.littleEndian());

        T x = v + 1, y = v;
        ASSERT_NE(y, x);
        ASSERT_EQ(&ds, &(ds>>x));
        ASSERT_EQ(0, ds.status());
        if(swaple(false))
            y = tools::SwapByteOrder(v);
        ASSERT_EQ(y, x);
    }
}

template<typename T>
static void testPod()
{
    for(int i = 0;i < 1000;++i)
        testV<T>(i);
    for(int i = 0;i > -1000;--i)
        testV<T>(i);
}

TEST(CInByteStream, pod)
{
    testPod<char>();
    testPod<unsigned char>();
    testPod<signed char>();
    testPod<short>();
    testPod<unsigned short>();
    testPod<int>();
    testPod<unsigned int>();
    testPod<long>();
    testPod<unsigned long>();
    testPod<long long>();
    testPod<unsigned long long>();
    testPod<wchar_t>();
}

template<class T, size_t N>
static void testRawCont(const int (&v)[N], bool seq = false)
{
    const std::string buf(reinterpret_cast<const char *>(v), sizeof v);

    CInByteStream ds(buf, false);
    ASSERT_EQ(0, ds.status());

    T y;
    y.insert(y.end(), 100);
    y.insert(y.end(), 101);
    y.insert(y.end(), 102);
    T x(y);
    ASSERT_EQ(&ds, &(ds>>Manip::raw(x, N)));
    ASSERT_EQ(0, ds.status());
    ASSERT_EQ(N + y.size(), x.size());
    if(seq){
        typename T::const_iterator it = x.begin();
        for(typename T::const_iterator it2 = y.begin();it2 != y.end();++it, ++it2)
            ASSERT_EQ(*it2, *it);
        for(size_t i = 0;i < N;++i, ++it)
            ASSERT_EQ(v[i], *it);
    }else{
        for(typename T::const_iterator i = y.begin();i != y.end();++i)
            ASSERT_TRUE(x.end() != std::find(x.begin(), x.end(), *i));
        for(size_t i = 0;i < N;++i)
            ASSERT_TRUE(x.end() != std::find(x.begin(), x.end(), v[i]));
    }
}

template<class T, size_t N>
static void testRawSeqCont(const int (&v)[N])
{
    testRawCont<T>(v, true);
    const std::string buf(reinterpret_cast<const char *>(v), sizeof v);
    {
        CInByteStream ds(buf, false);
        ASSERT_EQ(0, ds.status());

        T x(N, 0);
        ASSERT_EQ(&ds, &(ds>>Manip::raw(x.begin(), x.end())));
        ASSERT_EQ(0, ds.status());
        ASSERT_EQ(N, x.size());
        size_t i = 0;
        for(typename T::const_iterator it = x.begin();it != x.end();++it, ++i)
            ASSERT_EQ(v[i], *it);
    }{
        CInByteStream ds(buf, false);
        ASSERT_EQ(0, ds.status());

        T x(N, 0);
        size_t sz = 0;
        ASSERT_EQ(&ds, &(ds>>Manip::raw(x.begin(), x.end(), &sz)));
        ASSERT_EQ(0, ds.status());
        ASSERT_EQ(N, sz);
        ASSERT_EQ(N, x.size());
        size_t i = 0;
        for(typename T::const_iterator it = x.begin();it != x.end();++it, ++i)
            ASSERT_EQ(v[i], *it);
    }
}

TEST(CInByteStream, raw)
{
    const int v[] = {1, 2, 3, 4, 5, 6, 7, -1, -2, -3, -4, -5, -6, -7, 0, 1000};
    const size_t N = sizeof v / sizeof(int);
    const std::string buf(reinterpret_cast<const char *>(v), sizeof v);
    {
        CInByteStream ds(buf, false);
        ASSERT_EQ(0, ds.status());

        int x[N] = {};
        ASSERT_EQ(&ds, &(ds>>x));
        ASSERT_EQ(0, ds.status());
        for(size_t i = 0;i < N;++i)
            ASSERT_EQ(v[i], x[i]);
    }{
        CInByteStream ds(buf, false);
        ASSERT_EQ(0, ds.status());

        int x[N] = {};
        ASSERT_EQ(&ds, &(ds>>Manip::raw(x)));
        ASSERT_EQ(0, ds.status());
        for(size_t i = 0;i < N;++i)
            ASSERT_EQ(v[i], x[i]);
    }{
        CInByteStream ds(buf, false);
        ASSERT_EQ(0, ds.status());

        int x[N] = {};
        ASSERT_EQ(&ds, &(ds>>Manip::raw(x, N)));
        ASSERT_EQ(0, ds.status());
        for(size_t i = 0;i < N;++i)
            ASSERT_EQ(v[i], x[i]);
    }
    testRawSeqCont<std::vector<int> >(v);
    testRawSeqCont<std::list<int> >(v);
    testRawSeqCont<std::deque<int> >(v);
    testRawCont<std::set<int> >(v);
    testRawCont<std::multiset<int> >(v);
}

template<typename SizeT, size_t N>
static std::string arrayBuf(const int (&v)[N])
{
    std::string buf;
    SizeT sz = N;
    buf.append(reinterpret_cast<const char *>(&sz), sizeof sz);
    buf.append(reinterpret_cast<const char *>(v), sizeof v);
    return buf;
}

template<typename SizeT, class T, size_t N>
static void testArrayCont(const int (&v)[N], bool seq)
{
    const std::string buf = arrayBuf<SizeT>(v);
    {
        CInByteStream ds(buf, false);
        ASSERT_EQ(0, ds.status());

        T y;
        y.insert(y.end(), 100);
        y.insert(y.end(), 101);
        y.insert(y.end(), 102);
        T x(y);
        ASSERT_EQ(&ds, &(ds>>Manip::array<SizeT>(x)));
        ASSERT_EQ(0, ds.status());
        ASSERT_EQ(N + y.size(), x.size());
        if(seq){
            typename T::const_iterator it = x.begin();
            for(typename T::const_iterator it2 = y.begin();it2 != y.end();++it, ++it2)
                ASSERT_EQ(*it2, *it);
            for(size_t i = 0;i < N;++i, ++it)
                ASSERT_EQ(v[i], *it);
        }else{
            for(typename T::const_iterator i = y.begin();i != y.end();++i)
                ASSERT_TRUE(x.end() != std::find(x.begin(), x.end(), *i));
            for(size_t i = 0;i < N;++i)
                ASSERT_TRUE(x.end() != std::find(x.begin(), x.end(), v[i]));
        }
    }{
        CInByteStream ds(buf, false);
        ASSERT_EQ(0, ds.status());

        T y;
        y.insert(y.end(), 100);
        y.insert(y.end(), 101);
        y.insert(y.end(), 102);
        T x(y);
        ASSERT_EQ(&ds, &(ds>>Manip::array(x, SizeT(N))));
        ASSERT_EQ(0, ds.status());
        ASSERT_EQ(N + y.size(), x.size());
        if(seq){
            typename T::const_iterator it = x.begin();
            for(typename T::const_iterator it2 = y.begin();it2 != y.end();++it, ++it2)
                ASSERT_EQ(*it2, *it);
            for(size_t i = 0;i < N;++i, ++it)
                ASSERT_EQ(v[i], *it);
        }else{
            for(typename T::const_iterator i = y.begin();i != y.end();++i)
                ASSERT_TRUE(x.end() != std::find(x.begin(), x.end(), *i));
            for(size_t i = 0;i < N;++i)
                ASSERT_TRUE(x.end() != std::find(x.begin(), x.end(), v[i]));
        }
    }
}

template<class T, size_t N>
static void testArrayContU16(const int (&v)[N], bool seq)
{
    typedef uint16_t SizeT;
    testArrayCont<SizeT, T>(v, seq);
    const std::string buf = arrayBuf<SizeT>(v);
    {
        CInByteStream ds(buf, false);
        ASSERT_EQ(0, ds.status());

        T y;
        y.insert(y.end(), 100);
        y.insert(y.end(), 101);
        y.insert(y.end(), 102);
        T x(y);
        ASSERT_EQ(&ds, &(ds>>Manip::array(x)));
        ASSERT_EQ(0, ds.status());
        ASSERT_EQ(N + y.size(), x.size());
        if(seq){
            typename T::const_iterator it = x.begin();
            for(typename T::const_iterator it2 = y.begin();it2 != y.end();++it, ++it2)
                ASSERT_EQ(*it2, *it);
            for(size_t i = 0;i < N;++i, ++it)
                ASSERT_EQ(v[i], *it);
        }else{
            for(typename T::const_iterator i = y.begin();i != y.end();++i)
                ASSERT_TRUE(x.end() != std::find(x.begin(), x.end(), *i));
            for(size_t i = 0;i < N;++i)
                ASSERT_TRUE(x.end() != std::find(x.begin(), x.end(), v[i]));
        }
    }
}

template<typename SizeT, size_t N>
static void testArrayPtr(const int (&v)[N])
{
    const std::string buf = arrayBuf<SizeT>(v);
    {
        CInByteStream ds(buf, false);
        ASSERT_EQ(0, ds.status());

        int x[N] = {};
        SizeT sz = 0;
        ASSERT_EQ(&ds, &(ds>>Manip::array(x, &sz)));
        ASSERT_EQ(0, ds.status());
        ASSERT_EQ(N, size_t(sz));
        for(size_t i = 0;i < N;++i)
            ASSERT_EQ(v[i], x[i]);
    }{
        CInByteStream ds(buf, false);
        ASSERT_EQ(0, ds.status());

        int x[N] = {};
        SizeT sz = 0;
        ASSERT_EQ(&ds, &(ds>>Manip::array(x, N, &sz)));
        ASSERT_EQ(0, ds.status());
        ASSERT_EQ(N, size_t(sz));
        for(size_t i = 0;i < N;++i)
            ASSERT_EQ(v[i], x[i]);
    }
}

template<typename SizeT>
static void testArray()
{
    const int v[] = {1, 2, 3, 4, 5, 6, 7, -1, -2, -3, -4, -5, -6, -7, 0, 1000};

    testArrayPtr<SizeT>(v);
    testArrayCont<SizeT, std::vector<int> >(v, true);
    testArrayCont<SizeT, std::list<int> >(v, true);
    testArrayCont<SizeT, std::deque<int> >(v, true);
    testArrayCont<SizeT, std::set<int> >(v, false);
    testArrayCont<SizeT, std::multiset<int> >(v, false);
}

static void testArray()
{
    typedef uint16_t SizeT;
    const int v[] = {1, 2, 3, 4, 5, 6, 7, -1, -2, -3, -4, -5, -6, -7, 0, 1000};

    testArrayPtr<SizeT>(v);
    testArrayContU16<std::vector<int> >(v, true);
    testArrayContU16<std::list<int> >(v, true);
    testArrayContU16<std::deque<int> >(v, true);
    testArrayContU16<std::set<int> >(v, false);
    testArrayContU16<std::multiset<int> >(v, false);
}

TEST(CInByteStream, array)
{
    testArray<char>();
    testArray<unsigned char>();
    testArray<signed char>();
    testArray<short>();
    testArray<unsigned short>();
    testArray<int>();
    testArray<unsigned int>();
    testArray<long>();
    testArray<unsigned long>();
    testArray<long long>();
    testArray<unsigned long long>();
    testArray<wchar_t>();
    testArray<wchar_t>();
    testArray();    //uint16_t
}

TEST(CInByteStream, byte_order_value)
{
    const int v = 1000;
    {
        const int vv = htonl(v);
        CInByteStream ds(reinterpret_cast<const char *>(&vv), sizeof vv);
        ASSERT_EQ(0, ds.status());
        const bool old = ds.littleEndian();

        int x = 0;
        ASSERT_EQ(&ds, &(ds>>Manip::net_order_value(x)));
        ASSERT_EQ(0, ds.status());
        ASSERT_EQ(old, ds.littleEndian());
        ASSERT_EQ(v, x);
    }{
        const int vv = v;
        CInByteStream ds(reinterpret_cast<const char *>(&vv), sizeof vv);
        ASSERT_EQ(0, ds.status());
        const bool old = ds.littleEndian();

        int x = 0;
        ASSERT_EQ(&ds, &(ds>>Manip::host_order_value(x)));
        ASSERT_EQ(0, ds.status());
        ASSERT_EQ(old, ds.littleEndian());
        ASSERT_EQ(v, x);
    }{
        int vv = v;
        if(swaple(true))
               vv = tools::SwapByteOrder(vv);
        CInByteStream ds(reinterpret_cast<const char *>(&vv), sizeof vv);
        ASSERT_EQ(0, ds.status());
        const bool old = ds.littleEndian();

        int x = 0;
        ASSERT_EQ(&ds, &(ds>>Manip::little_endian_value(x)));
        ASSERT_EQ(0, ds.status());
        ASSERT_EQ(old, ds.littleEndian());
        ASSERT_EQ(v, x);
    }{
        int vv = v;
        if(swaple(false))
               vv = tools::SwapByteOrder(vv);
        CInByteStream ds(reinterpret_cast<const char *>(&vv), sizeof vv);
        ASSERT_EQ(0, ds.status());
        const bool old = ds.littleEndian();

        int x = 0;
        ASSERT_EQ(&ds, &(ds>>Manip::big_endian_value(x)));
        ASSERT_EQ(0, ds.status());
        ASSERT_EQ(old, ds.littleEndian());
        ASSERT_EQ(v, x);
    }
}

TEST(CInByteStream, offset_value)
{
    const int v = 0x12345678;
    std::string buf = "abc";
    buf.append(reinterpret_cast<const char *>(&v), sizeof v);
    buf.append("efg");
    CInByteStream ds(buf, false);
    ASSERT_EQ(0, ds.status());
    const size_t old = ds.cur();
    int x = 0;
    ASSERT_EQ(&ds, &(ds>>Manip::offset_value(3, x)));
    ASSERT_EQ(0, ds.status());
    ASSERT_EQ(old, ds.cur());
    ASSERT_EQ(v, x);
}

TEST(CInByteStream, protobuf)
{
    PbTest t;
    t.set_a(-100);
    t.set_b(200);
    t.set_c(-300);
    t.add_d(-400);
    t.add_d(-402);
    t.add_d(-403);
    t.set_e(500);
    t.set_f(-600);
    t.set_g(true);
    t.set_h(kVal1);
    t.set_i(700);
    t.set_j(800);
    t.set_k(3.1415);
    t.set_l(900);
    t.set_m(1000);
    t.set_n(2.71828);
    t.set_o("0a3nad-0a");
    t.set_p("23n-0f-q");
    t.mutable_q()->set_a(1100);
    std::string str;
    ASSERT_TRUE(t.SerializeToString(&str));
    {
        const std::string & buf = str;
        CInByteStream ds(buf);
        ASSERT_EQ(0, ds.status());

        PbTest x;
        ASSERT_EQ(&ds, &(ds>>Manip::protobuf(x)));
        ASSERT_EQ(0, ds.status());

        std::string ss;
        ASSERT_TRUE(x.SerializeToString(&ss));
        ASSERT_EQ(str, ss);
    }{
        size_t sz = str.size();
        const std::string buf = str + "abcdef";
        CInByteStream ds(buf);
        ASSERT_EQ(0, ds.status());

        PbTest x;
        ASSERT_EQ(&ds, &(ds>>Manip::protobuf(x, sz)));
        ASSERT_EQ(0, ds.status());

        std::string ss;
        ASSERT_TRUE(x.SerializeToString(&ss));
        ASSERT_EQ(str, ss);
    }
}

template<typename T>
bool testVarintValue(T v)
{
    std::string buf;
    COutByteStream oss;
    oss<<Manip::varint(v)<<Manip::end(buf);
    if(0 != oss.status()){
        cerr<<"oss.status()="<<oss.status()<<" for v="<<v<<endl;
        return false;
    }
    CInByteStream ds(buf);
    T x = v - 1;
    if(&ds != &(ds>>Manip::varint(x))){
        cerr<<"mismatch &ds="<<&ds<<" for v="<<v<<endl;
        return false;
    }
    if(0 != ds.status()){
        cerr<<"ds.status()="<<ds.status()<<" for v="<<v<<endl;
        return false;
    }
    if(v != x){
        cerr<<"x="<<uint64_t(x)<<" for v="<<uint64_t(v)<<endl;
        return false;
    }
    return true;
}

template<typename T>
bool testVarint()
{
    T v = 0, vv = 1;
    vv <<= CTypeTraits<T>::kMaxBits - 1;
    for(int i = 0;i < 1000;++i){
        if(!testVarintValue<T>(v + i))
            return false;
        if(!testVarintValue<T>(v - i))
            return false;
        if(!testVarintValue<T>(vv + i))
            return false;
        if(!testVarintValue<T>(vv - i))
            return false;
    }
    return true;
}

TEST(CInByteStream, varint)
{
    {
        typedef uint8_t T;
        T v = 128;
        std::string buf;
        COutByteStream oss;
        oss<<Manip::varint(v)<<Manip::end(buf);
        ASSERT_EQ(0, oss.status());
        T x = v - 1;
        CInByteStream ds(buf);
        ASSERT_EQ(&ds, &(ds>>Manip::varint(x)));
        ASSERT_EQ(0, ds.status());
        ASSERT_EQ(v, x);
    }{
        char buf[1];
        COutByteStreamBuf oss(buf, sizeof buf);
        oss<<Manip::varint(12345);
        ASSERT_NE(0, oss.status());

        buf[0] = 0xFF;
        CInByteStream ds(buf, sizeof buf);
        int x;
        ASSERT_EQ(&ds, &(ds>>Manip::varint(x)));
        ASSERT_NE(0, ds.status());
    }
    ASSERT_TRUE(testVarint<char>());
    ASSERT_TRUE(testVarint<unsigned char>());
    ASSERT_TRUE(testVarint<signed char>());
    ASSERT_TRUE(testVarint<short>());
    ASSERT_TRUE(testVarint<unsigned short>());
    ASSERT_TRUE(testVarint<int>());
    ASSERT_TRUE(testVarint<unsigned int>());
    ASSERT_TRUE(testVarint<long>());
    ASSERT_TRUE(testVarint<unsigned long>());
    ASSERT_TRUE(testVarint<long long>());
    ASSERT_TRUE(testVarint<unsigned long long>());
}

TEST(CInByteStream, stub)
{
    const int v[] = {1, 2, 3};
    const char * const buf = reinterpret_cast<const char *>(v);
    const size_t sz = sizeof v;
    {
        CInByteStream ds(buf, sz);
        ASSERT_EQ(0, ds.status());
        ASSERT_EQ(sz, ds.left());

        ASSERT_EQ(&ds, &(ds>>Manip::stub(4)));
        ASSERT_EQ(0, ds.status());
        ASSERT_EQ(size_t(4), ds.left());

        uint16_t x;
        ds>>x;
        ASSERT_EQ(0, ds.status());
        ASSERT_EQ(size_t(2), ds.left());

        ds>>x;
        ASSERT_EQ(4, (int)ds.cur());
        ASSERT_EQ(0, ds.status());
        ASSERT_EQ(size_t(0), ds.left());

        ds>>x;
        ASSERT_EQ(4, (int)ds.cur());
        ASSERT_NE(0, ds.status());
        ASSERT_EQ(size_t(0), ds.left());

        ds.status(0);
        ASSERT_EQ(&ds, &(ds>>Manip::stub(4)));
        ASSERT_EQ(0, ds.status());
        ASSERT_EQ(size_t(4), ds.left());

        ds>>x;
        ASSERT_EQ(0, ds.status());
        ASSERT_EQ(size_t(2), ds.left());

        ds>>x;
        ASSERT_EQ(8, (int)ds.cur());
        ASSERT_EQ(0, ds.status());
        ASSERT_EQ(size_t(0), ds.left());

        ds>>x;
        ASSERT_EQ(8, (int)ds.cur());
        ASSERT_NE(0, ds.status());
        ASSERT_EQ(size_t(0), ds.left());
    }
}

TEST(CInByteStream, stub_pop)
{
    const std::string buf(10, 'a');
    {
        CInByteStream ds(buf);
        ASSERT_EQ(0, ds.status());

        ASSERT_EQ(&ds, &(ds>>Manip::stub_pop()));
        ASSERT_NE(0, ds.status());
    }{
        CInByteStream ds(buf);
        ds>>Manip::stub(1);
        ASSERT_EQ(0, ds.status());

        ASSERT_EQ(&ds, &(ds>>Manip::stub_pop()));
        ASSERT_EQ(0, ds.status());

        ASSERT_EQ(&ds, &(ds>>Manip::stub_pop()));
        ASSERT_NE(0, ds.status());
        ASSERT_EQ(&ds, &(ds>>Manip::stub_pop()));
    }{
        CInByteStream ds(buf);
        ds>>Manip::stub(1)
            >>Manip::stub(2);
        ASSERT_EQ(0, ds.status());

        ASSERT_EQ(&ds, &(ds>>Manip::stub_pop()>>Manip::stub_pop()));
        ASSERT_EQ(0, ds.status());

        ASSERT_EQ(&ds, &(ds>>Manip::stub_pop()));
        ASSERT_NE(0, ds.status());
    }{
        CInByteStream ds(buf);
        ds>>Manip::stub(1)
            >>Manip::stub(2);
        ASSERT_EQ(0, ds.status());

        uint16_t x;
        ds>>x;
        ASSERT_EQ(0, ds.status());
        ASSERT_EQ(&ds, &(ds>>Manip::stub_pop()));
        ASSERT_NE(0, ds.status());
    }{
        CInByteStream ds(buf);
        ds>>Manip::stub(buf.size());
        ASSERT_EQ(0, ds.status());

        ASSERT_EQ(&ds, &(ds>>Manip::stub_pop(true)));
        ASSERT_EQ(0, ds.status());
        ASSERT_EQ(buf.size(), ds.cur());
    }{
        CInByteStream ds(buf);
        ds>>Manip::stub(buf.size() + 1);
        ASSERT_EQ(0, ds.status());

        ASSERT_EQ(&ds, &(ds>>Manip::stub_pop(true)));
        ASSERT_NE(0, ds.status());
    }{
        CInByteStream ds(buf);
        ds>>Manip::stub(1)
            >>Manip::stub(2);
        ASSERT_EQ(0, ds.status());

        uint16_t x;
        ds>>x;
        ASSERT_EQ(0, ds.status());
        ASSERT_EQ(&ds, &(ds>>Manip::stub_pop(true)));
        ASSERT_NE(0, ds.status());
    }{
        CInByteStream ds(buf);
        ds>>Manip::stub(buf.size());
        ASSERT_EQ(0, ds.status());

        ASSERT_EQ(&ds, &(ds>>Manip::stub_pop(true, true)));
        ASSERT_NE(0, ds.status());
    }{
        CInByteStream ds(buf);
        ds>>Manip::stub(buf.size());
        ASSERT_EQ(0, ds.status());

        std::string x;
        ds>>Manip::raw(x, buf.size());
        ASSERT_EQ(&ds, &(ds>>Manip::stub_pop(true, true)));
        ASSERT_EQ(0, ds.status());
    }{
        CInByteStream ds(buf);
        ds>>Manip::stub(1)
            >>Manip::stub(2);
        ASSERT_EQ(0, ds.status());

        uint16_t x;
        ds>>x;
        ASSERT_EQ(0, ds.status());
        ASSERT_EQ(&ds, &(ds>>Manip::stub_pop(true, true)));
        ASSERT_NE(0, ds.status());
    }
}

TEST(CInByteStream, end)
{
    const int v[] = {1, 2, 3};
    const char * const buf = reinterpret_cast<const char *>(v);
    const size_t sz = sizeof v;
    {
        CInByteStream ds(buf, sz);
        ASSERT_EQ(0, ds.status());

        ASSERT_EQ(&ds, &(ds>>Manip::end()));
        ASSERT_NE(0, ds.status());

        ds.status(0);
        ASSERT_EQ(0, ds.status());

        ASSERT_EQ(&ds, &(ds>>Manip::end));
        ASSERT_NE(0, ds.status());
    }{
        CInByteStream ds(buf, sz);
        int x;
        ASSERT_EQ(&ds, &(ds>>x));
        ASSERT_EQ(0, ds.status());

        ASSERT_EQ(&ds, &(ds>>Manip::end()));
        ASSERT_NE(0, ds.status());

        ds.status(0);
        ASSERT_EQ(0, ds.status());

        ASSERT_EQ(&ds, &(ds>>Manip::end));
        ASSERT_NE(0, ds.status());
    }{
        CInByteStream ds(buf, sz);
        int x[sizeof v / sizeof(int)];
        ASSERT_EQ(&ds, &(ds>>x));
        ASSERT_EQ(0, ds.status());

        ASSERT_EQ(&ds, &(ds>>Manip::end()));
        ASSERT_EQ(0, ds.status());

        ds.status(0);
        ASSERT_EQ(0, ds.status());

        ASSERT_EQ(&ds, &(ds>>Manip::end));
        ASSERT_EQ(0, ds.status());
    }
}

TEST(CInByteStream, toString)
{
    const char buf[] = "2;nr-qn-n3n";
    {
        CInByteStream ds;
        std::ostringstream oss;
        oss<<"{CDataStreamBase={status_="<<ds.status()
            <<", hostByteOrder_="<<endianName(tools::HostByteOrder())
            <<", dataByteOrder_="<<endianName(ds.littleEndian())
            <<"}, cur_=0, data_=(NULL)}";
        ASSERT_EQ(oss.str(), ds.toString());
    }{
        CInByteStream ds(buf, sizeof buf);
        std::ostringstream oss;
        oss<<"{CDataStreamBase={status_="<<ds.status()
            <<", hostByteOrder_="<<endianName(tools::HostByteOrder())
            <<", dataByteOrder_="<<endianName(ds.littleEndian())
            <<"}, cur_=0, data_=(12) | 32 3B 6E 72 2D 71 6E 2D 6E 33 6E 00}";
        ASSERT_EQ(oss.str(), ds.toString());
    }{
        CInByteStream ds(buf, sizeof buf);
        int x;
        ds>>x;
        std::ostringstream oss;
        oss<<"{CDataStreamBase={status_="<<ds.status()
            <<", hostByteOrder_="<<endianName(tools::HostByteOrder())
            <<", dataByteOrder_="<<endianName(ds.littleEndian())
            <<"}, cur_="<<ds.cur()
            <<", data_=(12)32 3B 6E 72 | 2D 71 6E 2D 6E 33 6E 00}";
        ASSERT_EQ(oss.str(), ds.toString());
    }{
        CInByteStream ds(buf, sizeof buf);
        int x;
        ds>>x
            >>Manip::stub(1)
            >>Manip::stub(2)
            >>Manip::stub(3);
        std::ostringstream oss;
        oss<<"{CDataStreamBase={status_="<<ds.status()
            <<", hostByteOrder_="<<endianName(tools::HostByteOrder())
            <<", dataByteOrder_="<<endianName(ds.littleEndian())
            <<", stubs_={5, 6, 7}"
            <<"}, cur_="<<ds.cur()
            <<", data_=(12)32 3B 6E 72 | 2D 71 6E 2D 6E 33 6E 00}";
        ASSERT_EQ(oss.str(), ds.toString());
    }{
        CInByteStream ds(buf, sizeof buf);
        int x;
        ds>>x
            >>Manip::stub(1)
            >>Manip::stub(2)
            >>Manip::stub(3)
            >>Manip::stub_pop()
            >>Manip::stub_pop()
            >>Manip::stub_pop()
            ;
        std::ostringstream oss;
        oss<<"{CDataStreamBase={status_="<<ds.status()
            <<", hostByteOrder_="<<endianName(tools::HostByteOrder())
            <<", dataByteOrder_="<<endianName(ds.littleEndian())
            <<"}, cur_="<<ds.cur()
            <<", data_=(12)32 3B 6E 72 | 2D 71 6E 2D 6E 33 6E 00}";
        ASSERT_EQ(oss.str(), ds.toString());
    }
}
