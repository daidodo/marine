#include <list>
#include <deque>
#include "byte_order.h"
#include "test.pb.h"

typedef __OutStream::buffer_type    __Buff;
typedef __OutStream::char_type __Char;


TEST(__OutStream, ctor)
{
    __Buff buf(10, 'a');
    {
        __OutStream out(buf);
        ASSERT_EQ(0, out.status());
        ASSERT_EQ(le(true), out.littleEndian());
    }{
        __OutStream out(buf, true);
        ASSERT_EQ(0, out.status());
        ASSERT_EQ(le(true), out.littleEndian());
    }{
        __OutStream out(buf, false);
        ASSERT_EQ(0, out.status());
        ASSERT_EQ(le(false), out.littleEndian());
    }
}

TEST(__OutStream, status)
{
    __Buff buf(10, 'a');
    __OutStream ds(buf);
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

TEST(__OutStream, byte_order)
{
    __Buff buf(10, 'a');
    __OutStream ds(buf);

    ds.netByteOrder(true);
    ASSERT_EQ(le(true), ds.littleEndian());

    ds.netByteOrder(false);
    ASSERT_EQ(le(false), ds.littleEndian());

    ds.littleEndian(true);
    ASSERT_TRUE(ds.littleEndian());

    ds.littleEndian(false);
    ASSERT_FALSE(ds.littleEndian());

    ASSERT_EQ(&ds, &(ds<<Manip::net_order));
    ASSERT_EQ(le(true), ds.littleEndian());

    ASSERT_EQ(&ds, &(ds<<Manip::host_order));
    ASSERT_EQ(le(false), ds.littleEndian());

    ASSERT_EQ(&ds, &(ds<<Manip::little_endian));
    ASSERT_TRUE(ds.littleEndian());

    ASSERT_EQ(&ds, &(ds<<Manip::big_endian));
    ASSERT_FALSE(ds.littleEndian());
}

TEST(__OutStream, seek)
{
    __Buff buf(10, 'a');
    {
        __OutStream ds(buf);
        size_t i = 0;
        for(;i <= 2000;++i){
            ASSERT_EQ(i, size_t(ds.seek(i)));
            ASSERT_EQ(0, ds.status());
            ASSERT_EQ(i, ds.cur());
            ASSERT_EQ(i, ds.size());
        }
    }{
        __OutStream ds(buf);
        size_t i = 0;
        for(;i <= 2000;++i){
            ASSERT_EQ(&ds, &(ds<<Manip::seek(i)));
            ASSERT_EQ(0, ds.status());
            ASSERT_EQ(i, ds.cur());
            ASSERT_EQ(i, ds.size());
        }
    }
}

TEST(__OutStream, skip)
{
    __Buff buf(10, 'a');
    {
        __OutStream ds(buf);
        size_t s = 0;
        ssize_t i = 0;
        for(;i <= 100;s += ++i){
            ASSERT_EQ(s, size_t(ds.skip(i)))<<"i="<<i;
            ASSERT_EQ(0, ds.status());
            ASSERT_EQ(s, ds.cur());
        }
        s = 5050;
        i = 0;
        for(;i >= -100;s += --i){
            ASSERT_EQ(s, size_t(ds.skip(i)))<<"i="<<i;
            ASSERT_EQ(0, ds.status());
            ASSERT_EQ(s, ds.cur());
        }
        ASSERT_EQ(0, ds.status());
        ASSERT_GT(0, ds.skip(-1));
        ASSERT_NE(0, ds.status())<<"s="<<s;
    }{
        __OutStream ds(buf);
        size_t s = 0;
        ssize_t i = 0;
        for(;i <= 100;s += ++i){
            ASSERT_EQ(&ds, &(ds<<Manip::skip(i)));
            ASSERT_EQ(0, ds.status());
            ASSERT_EQ(s, ds.cur());
        }
        s = 5050;
        i = 0;
        for(;i >= -100;s += --i){
            ASSERT_EQ(&ds, &(ds<<Manip::skip(i)));
            ASSERT_EQ(0, ds.status());
            ASSERT_EQ(s, ds.cur());
        }
        ASSERT_EQ(0, ds.status());
        ASSERT_EQ(&ds, &(ds<<Manip::skip(-1)));
        ASSERT_NE(0, ds.status());
    }{
        __OutStream ds(buf);
        size_t s = 0;
        ssize_t i = 0;
        for(;i <= 100;s += ++i){
            ASSERT_EQ(&ds, &(ds<<Manip::skip(&i)));
            ASSERT_EQ(0, ds.status());
            ASSERT_EQ(s, ds.cur());
        }
        s = 5050;
        i = 0;
        for(;i >= -100;s += --i){
            ASSERT_EQ(&ds, &(ds<<Manip::skip(&i)));
            ASSERT_EQ(0, ds.status());
            ASSERT_EQ(s, ds.cur());
        }
        ASSERT_EQ(0, ds.status());
        ASSERT_EQ(&ds, &(ds<<Manip::skip(-1)));
        ASSERT_NE(0, ds.status());
    }
}

template<typename T>
static void testV(T v)
{
    {
        __Buff buf;
        __OutStream ds(buf);
        ds.netByteOrder(true);
        ASSERT_EQ(0, ds.status());
        ASSERT_EQ(le(true), ds.littleEndian());

        ASSERT_EQ(&ds, &(ds<<v));
        ASSERT_EQ(0, ds.status());

        ASSERT_TRUE(ds.finish());
        T x = *reinterpret_cast<const T *>(&buf[0]);
        if(swapnb(true))
            x = tools::SwapByteOrder(x);
        ASSERT_EQ(v, x);
    }{
        __Buff buf;
        __OutStream ds(buf);
        ds.netByteOrder(false);
        ASSERT_EQ(0, ds.status());
        ASSERT_EQ(le(false), ds.littleEndian());

        ASSERT_EQ(&ds, &(ds<<v));
        ASSERT_EQ(0, ds.status());

        ASSERT_TRUE(ds.finish());
        T x = *reinterpret_cast<const T *>(&buf[0]);
        if(swapnb(false))
            x = tools::SwapByteOrder(x);
        ASSERT_EQ(v, x);
    }{
        __Buff buf;
        __OutStream ds(buf);
        ds.littleEndian(true);
        ASSERT_EQ(0, ds.status());
        ASSERT_TRUE(ds.littleEndian());

        ASSERT_EQ(&ds, &(ds<<v));
        ASSERT_EQ(0, ds.status());

        ASSERT_TRUE(ds.finish());
        T x = *reinterpret_cast<const T *>(&buf[0]);
        if(swaple(true))
            x = tools::SwapByteOrder(x);
        ASSERT_EQ(v, x);
    }{
        __Buff buf;
        __OutStream ds(buf);
        ds.littleEndian(false);
        ASSERT_EQ(0, ds.status());
        ASSERT_FALSE(ds.littleEndian());

        ASSERT_EQ(&ds, &(ds<<v));
        ASSERT_EQ(0, ds.status());

        ASSERT_TRUE(ds.finish());
        T x = *reinterpret_cast<const T *>(&buf[0]);
        if(swaple(false))
            x = tools::SwapByteOrder(x);
        ASSERT_EQ(v, x);
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

TEST(__OutStream, pod)
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

template<typename T>
static __Char * toBuf(T & v)
{
    return reinterpret_cast<__Char *>(&v);
}

template<class T, size_t N>
static void testRawCont(const int (&v)[N], bool seq)
{
    {
        T c;
        for(size_t i = 0;i < N;++i)
            c.insert(c.end(), v[i]);
        ASSERT_EQ(N, c.size());

        __Buff ss;
        __OutStream ds(ss, false);
        ASSERT_EQ(0, ds.status());

        ASSERT_EQ(&ds, &(ds<<Manip::raw(c)));
        ASSERT_EQ(0, ds.status());

        int x[N + 1] = {};
        __Char * buf = toBuf(x[0]);
        size_t sz = sizeof x;
        ASSERT_TRUE(ds.finish(buf, &sz));

        ASSERT_EQ(sizeof v, sz);
        for(size_t i = 0;i < N;++i){
            if(seq)
                ASSERT_EQ(v[i], x[i]);
            else
                ASSERT_NE(x + N, std::find(x, x + N, v[i]));
        }
    }{
        T c;
        for(size_t i = 0;i < N;++i)
            c.insert(c.end(), v[i]);
        ASSERT_EQ(N, c.size());

        __Buff ss;
        __OutStream ds(ss, false);
        ASSERT_EQ(0, ds.status());

        size_t sz = 0;
        ASSERT_EQ(&ds, &(ds<<Manip::raw(c, &sz)));
        ASSERT_EQ(0, ds.status());
        ASSERT_EQ(N, sz);

        int x[N + 1] = {};
        __Char * buf = toBuf(x[0]);
        sz = sizeof x;
        ASSERT_TRUE(ds.finish(buf, &sz));

        ASSERT_EQ(sizeof v, sz);
        for(size_t i = 0;i < N;++i){
            if(seq)
                ASSERT_EQ(v[i], x[i]);
            else
                ASSERT_NE(x + N, std::find(x, x + N, v[i]));
        }
    }{
        T c;
        for(size_t i = 0;i < N;++i)
            c.insert(c.end(), v[i]);
        ASSERT_EQ(N, c.size());

        __Buff ss;
        __OutStream ds(ss, false);
        ASSERT_EQ(0, ds.status());

        ASSERT_EQ(&ds, &(ds<<Manip::raw(c.begin(), c.end())));
        ASSERT_EQ(0, ds.status());

        int x[N + 1] = {};
        __Char * buf = toBuf(x[0]);
        size_t sz = sizeof x;
        ASSERT_TRUE(ds.finish(buf, &sz));

        ASSERT_EQ(sizeof v, sz);
        for(size_t i = 0;i < N;++i){
            if(seq)
                ASSERT_EQ(v[i], x[i]);
            else
                ASSERT_NE(x + N, std::find(x, x + N, v[i]));
        }
    }{
        T c;
        for(size_t i = 0;i < N;++i)
            c.insert(c.end(), v[i]);
        ASSERT_EQ(N, c.size());

        __Buff ss;
        __OutStream ds(ss, false);
        ASSERT_EQ(0, ds.status());

        size_t sz = 0;
        ASSERT_EQ(&ds, &(ds<<Manip::raw(c.begin(), c.end(), &sz)));
        ASSERT_EQ(0, ds.status());
        ASSERT_EQ(N, sz);

        int x[N + 1] = {};
        __Char * buf = toBuf(x[0]);
        sz = sizeof x;
        ASSERT_TRUE(ds.finish(buf, &sz));

        ASSERT_EQ(sizeof v, sz);
        for(size_t i = 0;i < N;++i){
            if(seq)
                ASSERT_EQ(v[i], x[i]);
            else
                ASSERT_NE(x + N, std::find(x, x + N, v[i]));
        }
    }
}

TEST(__OutStream, raw)
{
    const int v[] = {1, 2, 3, 4, 5, 6, 7, -1, -2, -3, -4, -5, -6, -7, 0, 1000};
    const size_t N = sizeof v / sizeof(int);
    {
        __Buff ss;
        __OutStream ds(ss, false);
        ASSERT_EQ(0, ds.status());

        ASSERT_EQ(&ds, &(ds<<v));
        ASSERT_EQ(0, ds.status());

        int x[N + 1] = {};
        __Char * buf = toBuf(x[0]);
        size_t sz = sizeof x;
        ASSERT_TRUE(ds.finish(buf, &sz));
        ASSERT_EQ(sizeof v, sz);
        for(size_t i = 0;i < N;++i)
            ASSERT_EQ(v[i], x[i]);
    }{
        __Buff ss;
        __OutStream ds(ss, false);
        ASSERT_EQ(0, ds.status());

        ASSERT_EQ(&ds, &(ds<<Manip::raw(v)));
        ASSERT_EQ(0, ds.status());

        int x[N + 1] = {};
        __Char * buf = toBuf(x[0]);
        size_t sz = sizeof x;
        ASSERT_TRUE(ds.finish(buf, &sz));
        ASSERT_EQ(sizeof v, sz);
        for(size_t i = 0;i < N;++i)
            ASSERT_EQ(v[i], x[i]);
    }{
        __Buff ss;
        __OutStream ds(ss, false);
        ASSERT_EQ(0, ds.status());

        ASSERT_EQ(&ds, &(ds<<Manip::raw(v, N)));
        ASSERT_EQ(0, ds.status());

        int x[N + 1] = {};
        __Char * buf = toBuf(x[0]);
        size_t sz = sizeof x;
        ASSERT_TRUE(ds.finish(buf, &sz));
        ASSERT_EQ(sizeof v, sz);
        for(size_t i = 0;i < N;++i)
            ASSERT_EQ(v[i], x[i]);
    }
    testRawCont<std::vector<int> >(v, true);
    testRawCont<std::list<int> >(v, true);
    testRawCont<std::deque<int> >(v, true);
    testRawCont<std::set<int> >(v, false);
    testRawCont<std::multiset<int> >(v, false);
}

template<typename SizeT>
static const int * fromArrayBuf(const char * buf, SizeT & sz)
{
    sz = *reinterpret_cast<const SizeT *>(buf);
    return reinterpret_cast<const int *>(buf + sizeof sz);
}

template<typename SizeT, class T, size_t N>
static void testArrayCont(const int (&v)[N], bool seq)
{
    {
        __Buff buf;
        __OutStream ds(buf, false);
        ASSERT_EQ(0, ds.status());

        T c(v, v + N);
        ASSERT_EQ(&ds, &(ds<<Manip::array<SizeT>(c)));
        ASSERT_EQ(0, ds.status());

        ASSERT_TRUE(ds.finish());
        ASSERT_EQ(sizeof(SizeT) + sizeof v, buf.size());

        SizeT sz;
        const int * x = fromArrayBuf(&buf[0], sz);
        ASSERT_EQ(N, size_t(sz));
        for(size_t i = 0;i < N;++i){
            if(seq)
                ASSERT_EQ(v[i], x[i]);
            else
                ASSERT_NE(x + N, std::find(x, x + N, v[i]));
        }
    }{
        __Buff buf;
        __OutStream ds(buf, false);
        ASSERT_EQ(0, ds.status());

        T c(v, v + N);
        ASSERT_EQ(&ds, &(ds<<Manip::array<SizeT>(c.begin(), c.end())));
        ASSERT_EQ(0, ds.status());

        ASSERT_TRUE(ds.finish());
        ASSERT_EQ(sizeof(SizeT) + sizeof v, buf.size());

        SizeT sz;
        const int * x = fromArrayBuf(&buf[0], sz);
        ASSERT_EQ(N, size_t(sz));
        for(size_t i = 0;i < N;++i){
            if(seq)
                ASSERT_EQ(v[i], x[i]);
            else
                ASSERT_NE(x + N, std::find(x, x + N, v[i]));
        }
    }{
        __Buff buf;
        __OutStream ds(buf, false);
        ASSERT_EQ(0, ds.status());

        T c(v, v + N);
        SizeT sz = 0;
        ASSERT_EQ(&ds, &(ds<<Manip::array(c.begin(), c.end(), &sz)));
        ASSERT_EQ(0, ds.status());

        ASSERT_TRUE(ds.finish());
        ASSERT_EQ(sizeof(SizeT) + sizeof v, buf.size());

        SizeT sz2;
        const int * x = fromArrayBuf(&buf[0], sz2);
        ASSERT_EQ(sz, sz2);
        ASSERT_EQ(N, size_t(sz));
        for(size_t i = 0;i < N;++i){
            if(seq)
                ASSERT_EQ(v[i], x[i]);
            else
                ASSERT_NE(x + N, std::find(x, x + N, v[i]));
        }
    }
}

template<class T, size_t N>
static void testArrayContU16(const int (&v)[N], bool seq)
{
    typedef uint16_t SizeT;
    testArrayCont<SizeT, T>(v, seq);
    {
        __Buff buf;
        __OutStream ds(buf, false);
        ASSERT_EQ(0, ds.status());

        T c(v, v + N);
        ASSERT_EQ(&ds, &(ds<<Manip::array(c)));
        ASSERT_EQ(0, ds.status());

        ASSERT_TRUE(ds.finish());
        ASSERT_EQ(sizeof(SizeT) + sizeof v, buf.size());

        SizeT sz;
        const int * x = fromArrayBuf(&buf[0], sz);
        ASSERT_EQ(N, size_t(sz));
        for(size_t i = 0;i < N;++i){
            if(seq)
                ASSERT_EQ(v[i], x[i]);
            else
                ASSERT_NE(x + N, std::find(x, x + N, v[i]));
        }
    }
}

template<typename SizeT, size_t N>
static void testArrayPtr(const int (&v)[N])
{
    {
        __Buff buf;
        __OutStream ds(buf, false);
        ASSERT_EQ(0, ds.status());

        ASSERT_EQ(&ds, &(ds<<Manip::array<SizeT>(v)));
        ASSERT_EQ(0, ds.status());

        ASSERT_TRUE(ds.finish());
        ASSERT_EQ(sizeof(SizeT) + sizeof v, buf.size());

        SizeT sz;
        const int * x = fromArrayBuf(&buf[0], sz);
        ASSERT_EQ(N, size_t(sz));
        for(size_t i = 0;i < N;++i)
            ASSERT_EQ(v[i], x[i]);
    }{
        __Buff buf;
        __OutStream ds(buf, false);
        ASSERT_EQ(0, ds.status());

        ASSERT_EQ(&ds, &(ds<<Manip::array<SizeT>(v, N)));
        ASSERT_EQ(0, ds.status());

        ASSERT_TRUE(ds.finish());
        ASSERT_EQ(sizeof(SizeT) + sizeof v, buf.size());

        SizeT sz;
        const int * x = fromArrayBuf(&buf[0], sz);
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

TEST(__OutStream, array)
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

TEST(__OutStream, byte_order_value)
{
    const int v = 1000;
    {
        int x = 0;
        __Buff buf;
        __OutStream ds(buf);
        ASSERT_EQ(0, ds.status());
        const bool old = ds.littleEndian();

        ASSERT_EQ(&ds, &(ds<<Manip::net_order_value(v)));
        ASSERT_EQ(0, ds.status());
        ASSERT_EQ(old, ds.littleEndian());

        ASSERT_TRUE(ds.finish());
        x = *reinterpret_cast<const int *>(&buf[0]);
        x = htonl(x);
        ASSERT_EQ(v, x);
    }{
        int x = 0;
        __Buff buf;
        __OutStream ds(buf);
        ASSERT_EQ(0, ds.status());
        const bool old = ds.littleEndian();

        ASSERT_EQ(&ds, &(ds<<Manip::net_order_value<uint32_t>(v)));
        ASSERT_EQ(0, ds.status());
        ASSERT_EQ(old, ds.littleEndian());

        ASSERT_TRUE(ds.finish());
        x = *reinterpret_cast<const int *>(&buf[0]);
        x = htonl(x);
        ASSERT_EQ(v, x);
    }{
        int x = 0;
        __Buff buf;
        __OutStream ds(buf);
        ASSERT_EQ(0, ds.status());
        const bool old = ds.littleEndian();

        ASSERT_EQ(&ds, &(ds<<Manip::host_order_value(v)));
        ASSERT_EQ(0, ds.status());
        ASSERT_EQ(old, ds.littleEndian());

        ASSERT_TRUE(ds.finish());
        x = *reinterpret_cast<const int *>(&buf[0]);
        ASSERT_EQ(v, x);
    }{
        int x = 0;
        __Buff buf;
        __OutStream ds(buf);
        ASSERT_EQ(0, ds.status());
        const bool old = ds.littleEndian();

        ASSERT_EQ(&ds, &(ds<<Manip::host_order_value<uint32_t>(v)));
        ASSERT_EQ(0, ds.status());
        ASSERT_EQ(old, ds.littleEndian());

        ASSERT_TRUE(ds.finish());
        x = *reinterpret_cast<const int *>(&buf[0]);
        ASSERT_EQ(v, x);
    }{
        int x = 0;
        __Buff buf;
        __OutStream ds(buf);
        ASSERT_EQ(0, ds.status());
        const bool old = ds.littleEndian();

        ASSERT_EQ(&ds, &(ds<<Manip::little_endian_value(v)));
        ASSERT_EQ(0, ds.status());
        ASSERT_EQ(old, ds.littleEndian());

        ASSERT_TRUE(ds.finish());
        x = *reinterpret_cast<const int *>(&buf[0]);
        if(swaple(true))
               x = tools::SwapByteOrder(x);
        ASSERT_EQ(v, x);
    }{
        int x = 0;
        __Buff buf;
        __OutStream ds(buf);
        ASSERT_EQ(0, ds.status());
        const bool old = ds.littleEndian();

        ASSERT_EQ(&ds, &(ds<<Manip::little_endian_value<uint32_t>(v)));
        ASSERT_EQ(0, ds.status());
        ASSERT_EQ(old, ds.littleEndian());

        ASSERT_TRUE(ds.finish());
        x = *reinterpret_cast<const int *>(&buf[0]);
        if(swaple(true))
               x = tools::SwapByteOrder(x);
        ASSERT_EQ(v, x);
    }{
        int x = 0;
        __Buff buf;
        __OutStream ds(buf);
        ASSERT_EQ(0, ds.status());
        const bool old = ds.littleEndian();

        ASSERT_EQ(&ds, &(ds<<Manip::big_endian_value(v)));
        ASSERT_EQ(0, ds.status());
        ASSERT_EQ(old, ds.littleEndian());

        ASSERT_TRUE(ds.finish());
        x = *reinterpret_cast<const int *>(&buf[0]);
        if(swaple(false))
               x = tools::SwapByteOrder(x);
        ASSERT_EQ(v, x);
    }{
        int x = 0;
        __Buff buf;
        __OutStream ds(buf);
        ASSERT_EQ(0, ds.status());
        const bool old = ds.littleEndian();

        ASSERT_EQ(&ds, &(ds<<Manip::big_endian_value<uint32_t>(v)));
        ASSERT_EQ(0, ds.status());
        ASSERT_EQ(old, ds.littleEndian());

        ASSERT_TRUE(ds.finish());
        x = *reinterpret_cast<const int *>(&buf[0]);
        if(swaple(false))
               x = tools::SwapByteOrder(x);
        ASSERT_EQ(v, x);
    }
}

TEST(__OutStream, offset_value)
{
    const int v = 0x12345678;
    __Buff buf;
    __OutStream ds(buf, false);
    ASSERT_EQ(0, ds.status());
    const size_t old = ds.cur();
    ASSERT_EQ(&ds, &(ds<<Manip::offset_value(3, v)));
    ASSERT_EQ(0, ds.status());
    ASSERT_EQ(old, ds.cur());

    ASSERT_TRUE(ds.finish());
    const int x = *reinterpret_cast<const int *>(&buf[0] + 3);
    ASSERT_EQ(v, x);
}

TEST(__OutStream, protobuf)
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
    std::string ss;
    ASSERT_TRUE(t.SerializeToString(&ss));
    {
        __Buff str;
        __OutStream ds(str);
        ASSERT_EQ(0, ds.status());

        ASSERT_EQ(&ds, &(ds<<Manip::protobuf(t)));
        ASSERT_EQ(0, ds.status());

        ASSERT_TRUE(ds.finish());

        ASSERT_EQ(ss.size(), str.size());
        ASSERT_EQ(0, memcmp(&ss[0], &str[0], ss.size()));
    }
}

TEST(__OutStream, stub)
{
    __Buff buf;
    __OutStream ds(buf);
    ASSERT_EQ(0, ds.status());

    ds<<Manip::stub(8)
        <<1<<2;
    ASSERT_EQ(0, ds.status());
    ds<<3;
    ASSERT_NE(0, ds.status());

    ds.status(0);
    ds<<Manip::stub(8)
        <<1<<2;
    ASSERT_EQ(0, ds.status());
    ds<<3;
    ASSERT_NE(0, ds.status());
}

TEST(__OutStream, stub_pop)
{
    __Buff buf(32, 0);
    {
        __OutStream ds(buf);
        ASSERT_EQ(0, ds.status());

        ASSERT_EQ(&ds, &(ds<<Manip::stub_pop()));
        ASSERT_NE(0, ds.status());
    }{
        __OutStream ds(buf);
        ds<<Manip::stub(1);
        ASSERT_EQ(0, ds.status());

        ASSERT_EQ(&ds, &(ds<<Manip::stub_pop()));
        ASSERT_EQ(0, ds.status());

        ASSERT_EQ(&ds, &(ds<<Manip::stub_pop()));
        ASSERT_NE(0, ds.status());
        ASSERT_EQ(&ds, &(ds<<Manip::stub_pop()));
    }{
        __OutStream ds(buf);
        ds<<Manip::stub(1)
            <<Manip::stub(2);
        ASSERT_EQ(0, ds.status());

        ASSERT_EQ(&ds, &(ds<<Manip::stub_pop()<<Manip::stub_pop()));
        ASSERT_EQ(0, ds.status());

        ASSERT_EQ(&ds, &(ds<<Manip::stub_pop()));
        ASSERT_NE(0, ds.status());
    }{
        __OutStream ds(buf);
        ds<<Manip::stub(1)
            <<Manip::stub(4);
        ASSERT_EQ(0, ds.status());

        ds<<1;
        ASSERT_EQ(0, ds.status());
        ASSERT_EQ(&ds, &(ds<<Manip::stub_pop()));
        ASSERT_NE(0, ds.status());
    }{
        const size_t off = 60;
        __OutStream ds(buf);
        ds<<Manip::stub(off);
        ASSERT_EQ(0, ds.status());

        ASSERT_EQ(&ds, &(ds<<Manip::stub_pop(true)));
        ASSERT_EQ(0, ds.status());
        ASSERT_EQ(off, ds.cur());
    }{
        __OutStream ds(buf);
        ds<<Manip::stub(1)
            <<Manip::stub(4);
        ASSERT_EQ(0, ds.status());

        ds<<1;
        ASSERT_EQ(0, ds.status());
        ASSERT_EQ(&ds, &(ds<<Manip::stub_pop(true)));
        ASSERT_NE(0, ds.status());
    }{
        __OutStream ds(buf);
        ds<<Manip::stub(buf.size());
        ASSERT_EQ(0, ds.status());

        ASSERT_EQ(&ds, &(ds<<Manip::stub_pop(true, true)));
        ASSERT_NE(0, ds.status());
    }{
        __OutStream ds(buf);
        ds<<Manip::stub(buf.size());
        ASSERT_EQ(0, ds.status());

        std::string x;
        ds<<Manip::raw(x, buf.size());
        ASSERT_EQ(&ds, &(ds<<Manip::stub_pop(true, true)));
        ASSERT_EQ(0, ds.status());
    }{
        __OutStream ds(buf);
        ds<<Manip::stub(1)
            <<Manip::stub(4);
        ASSERT_EQ(0, ds.status());

        ds<<1;
        ASSERT_EQ(0, ds.status());
        ASSERT_EQ(&ds, &(ds<<Manip::stub_pop(true, true)));
        ASSERT_NE(0, ds.status());
    }
}

TEST(__OutStream, finish)
{
    const char pp[] = "dn32";
    const char pre[] = "23ndf";
    const char v[] = ";n32kq-b q3 -9qef q;fdkf";
    {
        __Buff x(pre, pre + sizeof pre);
        __OutStream ds(x);
        ASSERT_EQ(0, ds.status());

        ASSERT_EQ(&ds, &(ds<<Manip::raw(v)));
        ASSERT_EQ(0, ds.status());
        ASSERT_NE(size_t(0), ds.cur());

        ASSERT_TRUE(ds.finish());
        ASSERT_EQ(size_t(0), ds.cur());
        ASSERT_EQ(sizeof pre + sizeof v, x.size());
        ASSERT_EQ(0, memcmp(pre, &x[0], sizeof pre));
        ASSERT_EQ(0, memcmp(v, &x[sizeof pre], sizeof v));
    }{
        __Buff x(pre, pre + sizeof pre);
        __OutStream ds(x);
        ASSERT_EQ(0, ds.status());

        ASSERT_EQ(&ds, &(ds<<Manip::raw(v)));
        ASSERT_EQ(0, ds.status());
        ASSERT_NE(size_t(0), ds.cur());

        size_t sz = 0;
        ASSERT_TRUE(ds.finish(&sz));
        ASSERT_EQ(size_t(0), ds.cur());
        ASSERT_EQ(sizeof pre + sizeof v, sz);
        ASSERT_EQ(sizeof pre + sizeof v, x.size());
        ASSERT_EQ(0, memcmp(pre, &x[0], sizeof pre));
        ASSERT_EQ(0, memcmp(v, &x[sizeof pre], sizeof v));
    }{
        __Buff x(pre, pre + sizeof pre);
        __OutStream ds(x);
        ASSERT_EQ(0, ds.status());

        ASSERT_EQ(&ds, &(ds<<Manip::raw(v)));
        ASSERT_EQ(0, ds.status());
        ASSERT_NE(size_t(0), ds.cur());

        __Buff y(pp, pp + sizeof pp);
        ASSERT_TRUE(ds.finish(y));
        ASSERT_EQ(size_t(0), ds.cur());

        ASSERT_EQ(sizeof pre + sizeof v, x.size());
        ASSERT_EQ(0, memcmp(pre, &x[0], sizeof pre));
        ASSERT_EQ(0, memcmp(v, &x[sizeof pre], sizeof v));
        ASSERT_EQ(sizeof pp + sizeof pre + sizeof v, y.size());
        ASSERT_EQ(0, memcmp(pp, &y[0], sizeof pp))<<"y="<<tools::Dump(y);
        ASSERT_EQ(0, memcmp(pre, &y[sizeof pp], sizeof pre));
        ASSERT_EQ(0, memcmp(v, &y[sizeof pp + sizeof pre], sizeof v));
    }{
        __Buff x(pre, pre + sizeof pre);
        __OutStream ds(x);
        ASSERT_EQ(0, ds.status());

        ASSERT_EQ(&ds, &(ds<<Manip::raw(v)));
        ASSERT_EQ(0, ds.status());
        ASSERT_NE(size_t(0), ds.cur());

        __Buff y(4096, 0);
        size_t sz = y.size();
        ASSERT_TRUE(ds.finish(&y[0], &sz));
        ASSERT_EQ(size_t(0), ds.cur());

        ASSERT_EQ(sizeof pre + sizeof v, x.size());
        ASSERT_EQ(0, memcmp(pre, &x[0], sizeof pre));
        ASSERT_EQ(0, memcmp(v, &x[sizeof pre], sizeof v));
        ASSERT_EQ(sizeof pre + sizeof v, sz);
        ASSERT_EQ(0, memcmp(pre, &y[0], sizeof pre));
        ASSERT_EQ(0, memcmp(v, &y[sizeof pre], sizeof v));
    }{
        __Buff x(pre, pre + sizeof pre);
        __OutStream ds(x);
        ASSERT_EQ(0, ds.status());

        ASSERT_EQ(&ds, &(ds<<Manip::raw(v)));
        ASSERT_EQ(0, ds.status());
        ASSERT_NE(size_t(0), ds.cur());

        ASSERT_EQ(&ds, &(ds<<Manip::end));
        ASSERT_EQ(size_t(0), ds.cur());
        ASSERT_EQ(sizeof pre + sizeof v, x.size());
        ASSERT_EQ(0, memcmp(pre, &x[0], sizeof pre));
        ASSERT_EQ(0, memcmp(v, &x[sizeof pre], sizeof v));
    }{
        __Buff x(pre, pre + sizeof pre);
        __OutStream ds(x);
        ASSERT_EQ(0, ds.status());

        ASSERT_EQ(&ds, &(ds<<Manip::raw(v)));
        ASSERT_EQ(0, ds.status());
        ASSERT_NE(size_t(0), ds.cur());

        ASSERT_EQ(&ds, &(ds<<Manip::end()));
        ASSERT_EQ(size_t(0), ds.cur());
        ASSERT_EQ(sizeof pre + sizeof v, x.size());
        ASSERT_EQ(0, memcmp(pre, &x[0], sizeof pre));
        ASSERT_EQ(0, memcmp(v, &x[sizeof pre], sizeof v));
    }{
        __Buff x(pre, pre + sizeof pre);
        __OutStream ds(x);
        ASSERT_EQ(0, ds.status());

        ASSERT_EQ(&ds, &(ds<<Manip::raw(v)));
        ASSERT_EQ(0, ds.status());
        ASSERT_NE(size_t(0), ds.cur());

        size_t sz = 0;
        ASSERT_EQ(&ds, &(ds<<Manip::end(&sz)));
        ASSERT_EQ(size_t(0), ds.cur());
        ASSERT_EQ(sizeof pre + sizeof v, sz);
        ASSERT_EQ(sizeof pre + sizeof v, x.size());
        ASSERT_EQ(0, memcmp(pre, &x[0], sizeof pre));
        ASSERT_EQ(0, memcmp(v, &x[sizeof pre], sizeof v));
    }{
        __Buff x(pre, pre + sizeof pre);
        __OutStream ds(x);
        ASSERT_EQ(0, ds.status());

        ASSERT_EQ(&ds, &(ds<<Manip::raw(v)));
        ASSERT_EQ(0, ds.status());
        ASSERT_NE(size_t(0), ds.cur());

        __Buff y(pp, pp + sizeof pp);
        ASSERT_EQ(&ds, &(ds<<Manip::end(y)));
        ASSERT_EQ(size_t(0), ds.cur());

        ASSERT_EQ(sizeof pre + sizeof v, x.size());
        ASSERT_EQ(0, memcmp(pre, &x[0], sizeof pre));
        ASSERT_EQ(0, memcmp(v, &x[sizeof pre], sizeof v));
        ASSERT_EQ(sizeof pp + sizeof pre + sizeof v, y.size());
        ASSERT_EQ(0, memcmp(pp, &y[0], sizeof pp))<<"y="<<tools::Dump(y);
        ASSERT_EQ(0, memcmp(pre, &y[sizeof pp], sizeof pre));
        ASSERT_EQ(0, memcmp(v, &y[sizeof pp + sizeof pre], sizeof v));
    }{
        __Buff x(pre, pre + sizeof pre);
        __OutStream ds(x);
        ASSERT_EQ(0, ds.status());

        ASSERT_EQ(&ds, &(ds<<Manip::raw(v)));
        ASSERT_EQ(0, ds.status());
        ASSERT_NE(size_t(0), ds.cur());

        __Buff y(4096, 0);
        size_t sz = y.size();
        ASSERT_EQ(&ds, &(ds<<Manip::end(&y[0], &sz)));
        ASSERT_EQ(size_t(0), ds.cur());

        ASSERT_EQ(sizeof pre + sizeof v, x.size());
        ASSERT_EQ(0, memcmp(pre, &x[0], sizeof pre));
        ASSERT_EQ(0, memcmp(v, &x[sizeof pre], sizeof v));
        ASSERT_EQ(sizeof pre + sizeof v, sz);
        ASSERT_EQ(0, memcmp(pre, &y[0], sizeof pre));
        ASSERT_EQ(0, memcmp(v, &y[sizeof pre], sizeof v));
    }
}

TEST(__OutStream, toString)
{
    const char p[] = "fgns;";
    const char buf[] = "2;nr-qn-n3n";
    {
        __Buff str(1, 0);
        str.clear();
        __OutStream ds(str);
        std::ostringstream oss;
        oss<<std::boolalpha
            <<"{CDataStreamBase={status_="<<ds.status()
            <<", hostByteOrder_="<<endianName(tools::HostByteOrder())
            <<", dataByteOrder_="<<endianName(ds.littleEndian())
            <<"}, data_={begin_=0, buf_=(0) | }}";
        ASSERT_EQ(oss.str(), ds.toString());
    }{
        __Buff str(p, p + sizeof p);
        __OutStream ds(str);
        std::ostringstream oss;
        oss<<std::boolalpha
            <<"{CDataStreamBase={status_="<<ds.status()
            <<", hostByteOrder_="<<endianName(tools::HostByteOrder())
            <<", dataByteOrder_="<<endianName(ds.littleEndian())
            <<"}, data_={begin_=6, buf_=(6)66 67 6E 73 3B 00 | }}";
        ASSERT_EQ(oss.str(), ds.toString());
    }{
        __Buff str;
        __OutStream ds(str);
        ds<<buf;
        std::ostringstream oss;
        oss<<std::boolalpha
            <<"{CDataStreamBase={status_="<<ds.status()
            <<", hostByteOrder_="<<endianName(tools::HostByteOrder())
            <<", dataByteOrder_="<<endianName(ds.littleEndian())
            <<"}, data_={begin_=0, buf_=(12) | 32 3B 6E 72 2D 71 6E 2D 6E 33 6E 00}}";
        ASSERT_EQ(oss.str(), ds.toString());
    }{
        __Buff str(p, p + sizeof p);
        __OutStream ds(str);
        ds<<buf;
        std::ostringstream oss;
        oss<<std::boolalpha
            <<"{CDataStreamBase={status_="<<ds.status()
            <<", hostByteOrder_="<<endianName(tools::HostByteOrder())
            <<", dataByteOrder_="<<endianName(ds.littleEndian())
            <<"}, data_={begin_=6, buf_=(18)66 67 6E 73 3B 00 | 32 3B 6E 72 2D 71 6E 2D 6E 33 6E 00}}";
        ASSERT_EQ(oss.str(), ds.toString());
    }
}
