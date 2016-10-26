#include <marine/tools/other.hh>

#include "../inc.h"

TEST(Crc, buf)
{
    const char c[] = "-0bha3 s9-ban3sdfaup93rjqdJN:LKDJF_H#EN#DHBNdfa90hefn a30n";
    ASSERT_EQ(4039688691u, tools::Crc(0u, c, sizeof c));

    std::string s(c, sizeof c);
    ASSERT_EQ(4039688691u, tools::Crc(0u, s));

    std::vector<char> v(s.begin(), s.end());
    ASSERT_EQ(4039688691u, tools::Crc(0u, v));
}

TEST(Crc, emtpy)
{
    const char c[0] = {};
    ASSERT_EQ(0u, tools::Crc(0u, c, sizeof c));

    std::string s;
    ASSERT_EQ(0u, tools::Crc(0u, s));

    std::vector<char> v;
    ASSERT_EQ(0u, tools::Crc(0u, v));
}

static bool slowIsPrime(int v)
{
    if(v < 2)
        return false;
    for(int i = 2;i < v;++i)
        if(0 == v % i)
            return false;
    return true;
}

static int slowPrimeLess(int v)
{
    for(;v >= 2;--v)
        if(slowIsPrime(v))
            return v;
    return 0;
}

static int slowPrimeGreater(int v)
{
    for(;;++v)
        if(slowIsPrime(v))
            return v;
    return 0;
}

TEST(IsPrime, all)
{
    for(int i = -100;i < 10000;++i)
        ASSERT_TRUE(slowIsPrime(i) == tools::IsPrime(i))<<"i = "<<i<<endl;
}

TEST(PrimeLess, all)
{
    for(int i = -100;i < 10000;++i){
        int r = tools::PrimeLess(i);
        ASSERT_EQ(slowPrimeLess(i), r)<<"i = "<<i<<", r = "<<r<<endl;
        if(i < 2){
            ASSERT_EQ(0, r);
        }else{
            ASSERT_GE(i, r)<<"i = "<<i<<", r = "<<r<<endl;
        }
    }
}

TEST(PrimeGreater, all)
{
    for(int i = -100;i < 10000;++i){
        int r = tools::PrimeGreater(i);
        ASSERT_EQ(slowPrimeGreater(i), r)<<"i = "<<i<<", r = "<<r<<endl;
        ASSERT_LE(i, r)<<"i = "<<i<<", r = "<<r<<endl;
    }
}

void printPrimes(uint64_t total, int count, uint64_t ret, const std::vector<int> & arr)
{
    cout<<"total="<<total<<", count="<<count<<":\n"
        <<"ret="<<ret<<"\n{";
    for(size_t i = 0;i < arr.size();++i){
        if(i)
            cout<<", ";
        cout<<arr[i];
    }
    cout<<"}\n";
}

TEST(PrimesGenerator, all)
{
    {
        const int count = 20;
        std::vector<int> arr;
        for(uint64_t total = 0;total <= 20000;++total){
            arr.clear();
            uint64_t ret = tools::PrimesGenerator(total, count, arr);
            ASSERT_LE(total, ret);
            ASSERT_EQ(size_t(count), arr.size());
            uint64_t sum = 0;
            for(int i = 0;i < count;++i){
                ASSERT_TRUE(slowIsPrime(arr[i]));
                sum += arr[i];
                for(int j = 0;j < count;++j){
                    if(i == j)
                        continue;
                    ASSERT_NE(arr[i], arr[j]);
                }
            }
            ASSERT_EQ(ret, sum);
            //if(total > 19990)
            //    printPrimes(total, count, ret, arr);
        }
    }{
        const int count = 1;
        std::vector<int> arr;
        for(uint64_t total = 0;total <= 20000;++total){
            arr.clear();
            uint64_t ret = tools::PrimesGenerator(total, count, arr);
            ASSERT_LE(total, ret);
            ASSERT_EQ(size_t(count), arr.size());
            uint64_t sum = 0;
            for(int i = 0;i < count;++i){
                ASSERT_TRUE(slowIsPrime(arr[i]));
                sum += arr[i];
                for(int j = 0;j < count;++j){
                    if(i == j)
                        continue;
                    ASSERT_NE(arr[i], arr[j]);
                }
            }
            ASSERT_EQ(ret, sum);
            //if(total > 19990)
            //    printPrimes(total, count, ret, arr);
        }
    }
}

namespace n1{
    namespace n2{
        struct A{
            struct B;

            int g(B *);
            typedef int (A::*_g)(B *);
            int k(B &) const;
            typedef int (A::*_k)(B &) const;

            struct B{
                static int f(A::_g);
                int g(const A *);
                typedef int (B::*_g)(A *);
                int k(const A &) const;
                typedef int (B::*_k)(const A &) const;
            };
            static int f(B::_k);
        };

        template<class T, int N>
        struct C{
            static int f(A::B::_k);
            int g(A *);
            typedef int (C::*_g)(A *);

            template<class S, int M>
            struct D{
            };
        };
    }
}

TEST(CxxDemangle, all)
{
    ASSERT_EQ("int", tools::CxxDemangle(typeid(int).name()));
    ASSERT_EQ("int*", tools::CxxDemangle(typeid(int *).name()));
    ASSERT_EQ("int**", tools::CxxDemangle(typeid(int **).name()));
    ASSERT_EQ("int", tools::CxxDemangle(typeid(int &).name()));

    ASSERT_EQ("int", tools::CxxDemangle(typeid(const int).name()));
    ASSERT_EQ("int const*", tools::CxxDemangle(typeid(const int *).name()));
    ASSERT_EQ("int const*", tools::CxxDemangle(typeid(const int * const).name()));

    using namespace n1::n2;

    ASSERT_EQ("n1::n2::A", tools::CxxDemangle(typeid(A).name()));
    ASSERT_EQ("n1::n2::A::B", tools::CxxDemangle(typeid(A::B).name()));

    //ASSERT_EQ("int ()(int (n1::n2::A::B::*)(n1::n2::A const&) const)", tools::CxxDemangle(typeid(A::f).name()));
    ASSERT_EQ("int (n1::n2::A::*)(n1::n2::A::B*)", tools::CxxDemangle(typeid(A::_g).name()));
    ASSERT_EQ("int (n1::n2::A::*)(n1::n2::A::B&) const", tools::CxxDemangle(typeid(A::_k).name()));

    //ASSERT_EQ("int ()(int (n1::n2::A::*)(n1::n2::A::B*))", tools::CxxDemangle(typeid(A::B::f).name()));
    ASSERT_EQ("int (n1::n2::A::B::*)(n1::n2::A*)", tools::CxxDemangle(typeid(A::B::_g).name()));
    ASSERT_EQ("int (n1::n2::A::B::*)(n1::n2::A const&) const", tools::CxxDemangle(typeid(A::B::_k).name()));

    ASSERT_EQ("n1::n2::C<int, 1>", tools::CxxDemangle(typeid(C<int, 1>).name()));
    ASSERT_EQ("n1::n2::C<n1::n2::A, -2>", tools::CxxDemangle(typeid(C<A, -2>).name()));
    ASSERT_EQ("n1::n2::C<long, 3>::D<double, -4>", tools::CxxDemangle(typeid(C<long, 3>::D<double, -4>).name()));
    ASSERT_EQ("n1::n2::C<n1::n2::A::B, 5>::D<int (n1::n2::A::*)(n1::n2::A::B*), -6>", tools::CxxDemangle(typeid(C<A::B, 5>::D<A::_g, -6>).name()));

    //ASSERT_EQ("int ()(int (n1::n2::A::B::*)(n1::n2::A const&) const)", tools::CxxDemangle(typeid(C<A::_k, 7>::f).name()));
    ASSERT_EQ("int (n1::n2::C<int (n1::n2::A::B::*)(n1::n2::A*), -8>::*)(n1::n2::A*)", tools::CxxDemangle(typeid(C<A::B::_g, -8>::_g).name()));

}
