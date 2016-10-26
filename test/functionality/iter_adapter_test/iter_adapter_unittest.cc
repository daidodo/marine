#include <marine/iter_adapter.hh>

#include "../inc.h"

template<class Iter>
static int sum(Iter first, Iter last)
{
    int ret = 0;
    for(;first != last;++first)
        ret += *first;
    return ret;
}

template<class Iter>
static int acc(Iter first, Iter last)
{
    int cur = 0;
    for(;first != last;++first){
        cur += *first;
        *first = cur;
    }
    return cur;
}

struct A
{
    int v1;
    string v2;
    A(){}
    explicit A(int x):v1(x){}
};

struct CV1 : public std::unary_function<A, int>
{
    const result_type & operator ()(const argument_type & a) const{
        return a.v1;
    }
};

TEST(const_iter_adapt, all)
{
    vector<A> v;
    for(int i = 1;i <= 100;++i)
        v.push_back(A(i));

    ASSERT_EQ(5050, sum(const_iter_adapt(v.begin(), CV1())
                , const_iter_adapt(v.end(), CV1())));

    ASSERT_EQ(5050, sum(const_iter_adapt<CV1>(v.begin())
                , const_iter_adapt<CV1>(v.end())));
}

struct V1 : public std::unary_function<A, int>
{
    result_type & operator ()(argument_type & a) const{
        return a.v1;
    }
};

TEST(iter_adapt, all)
{
    vector<A> s;
    for(int i = 1;i <= 100;++i)
        s.push_back(A(i));
    {
        vector<A> v(s);
        ASSERT_EQ(5050, acc(iter_adapt(v.begin(), V1())
                    , iter_adapt(v.end(), V1())));
        int sum = 0;
        for(size_t i = 0;i < v.size();++i){
            sum += i + 1;
            ASSERT_EQ(sum, v[i].v1)<<"i="<<i;
        }
    }{
        vector<A> v(s);
        ASSERT_EQ(5050, acc(iter_adapt<V1>(v.begin())
                    , iter_adapt<V1>(v.end())));
        int sum = 0;
        for(size_t i = 0;i < v.size();++i){
            sum += i + 1;
            ASSERT_EQ(sum, v[i].v1)<<"i="<<i;
        }
    }
}

static const int & CF1(const A & a)
{
    return a.v1;
}

TEST(const_iter_adapt_fun, all)
{
    vector<A> v;
    for(int i = 1;i <= 100;++i)
        v.push_back(A(i));

    ASSERT_EQ(5050, sum(const_iter_adapt_fun<int>(v.begin(), CF1)
                , const_iter_adapt_fun<int>(v.end(), CF1)));
}

static int & F1(A & a)
{
    return a.v1;
}

TEST(iter_adapt_fun, all)
{
    vector<A> v;
    for(int i = 1;i <= 100;++i)
        v.push_back(A(i));

    ASSERT_EQ(5050, acc(iter_adapt_fun<int>(v.begin(), F1)
                , iter_adapt_fun<int>(v.end(), F1)));
    int sum = 0;
    for(size_t i = 0;i < v.size();++i){
        sum += i + 1;
        ASSERT_EQ(sum, v[i].v1)<<"i="<<i;
    }
}
