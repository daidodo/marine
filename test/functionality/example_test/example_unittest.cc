#include <marine/atomic_sync.hh>

#include <list>
#include "../inc.h"

double throw_test(int a, int b)
{
    if(!b)
        throw int(1);
    return double(a) / b;
}

TEST(a, 1)
{
    ASSERT_EQ(0, throw_test(0, 1));
    EXPECT_EQ(0, throw_test(0, 1));

    //ADD_FAILURE();
    //FAIL()<<"this is msg";
    SUCCEED();

    ASSERT_THROW(throw_test(1, 0), int);
    ASSERT_ANY_THROW(throw_test(1, 0));
    ASSERT_NO_THROW(throw_test(2, 1));
}

TEST(a, 2)
{
    ASSERT_PRED2(throw_test, 3, 3);
    //ASSERT_PRED2(throw_test, 0, 3);
}

TEST(a, 3)
{
    ASSERT_FLOAT_EQ(1. / 3, throw_test(1, 3));
    ASSERT_DOUBLE_EQ(1. / 3, throw_test(1, 3));

    ASSERT_NEAR(2.8 / 9, throw_test(1, 3), 0.1);
}

//environment
class CEnv : public ::testing::Environment
{
public:
    void SetUp(){
        cout<<"CEnv::SetUp()\n";
    }
    void TearDown(){
        cout<<"CEnv::TearDown()\n";
    }
};

struct CInit
{
    CInit(){
        ::testing::AddGlobalTestEnvironment(new CEnv);
    }
}g_init;

//fixture
struct CMyTest : public ::testing::Test
{
    static void SetUpTestCase(){
        cout<<"CMyTest::SetUpTestCase()\n";
    }
    static void TearDownTestCase(){
        cout<<"CMyTest::TearDownTestCase()\n";
    }
    void SetUp(){
        cout<<"CMyTest::SetUp()\n";
    }
    void TearDown(){
        cout<<"CMyTest::TearDown()\n";
    }
};

TEST_F(CMyTest, 1)
{
    //ADD_FAILURE();
    //FAIL();
    SUCCEED();

    ASSERT_THROW(throw_test(1, 0), int);
    ASSERT_ANY_THROW(throw_test(1, 0));
    ASSERT_NO_THROW(throw_test(2, 1));
}

TEST_F(CMyTest, 2)
{
    ASSERT_PRED2(throw_test, 3, 3);
    //ASSERT_PRED2(throw_test, 0, 3);
}

TEST_F(CMyTest, 3)
{
    ASSERT_FLOAT_EQ(1. / 3, throw_test(1, 3));
    ASSERT_DOUBLE_EQ(1. / 3, throw_test(1, 3));

    ASSERT_NEAR(2.8 / 9, throw_test(1, 3), 0.1);
}

//parameters
struct MyParamTest : public ::testing::TestWithParam<int>
{
    static void SetUpTestCase(){
        cout<<"MyParamTest::SetUpTestCase()\n";
    }
    static void TearDownTestCase(){
        cout<<"MyParamTest::TearDownTestCase()\n";
    }
    void SetUp(){
        cout<<"MyParamTest::SetUp()\n";
    }
    void TearDown(){
        cout<<"MyParamTest::TearDown()\n";
    }
};

TEST_P(MyParamTest, a)
{
    int b = GetParam();
    ASSERT_DOUBLE_EQ(10. / b, throw_test(10, b));
}

TEST_P(MyParamTest, b)
{
    int b = GetParam();
    ASSERT_DOUBLE_EQ(100. / b, throw_test(100, b));
}

INSTANTIATE_TEST_CASE_P(throw_test1, MyParamTest, ::testing::Range(1, 3));

INSTANTIATE_TEST_CASE_P(throw_test2, MyParamTest, ::testing::Values(11, 12, 13));

int VALS[] = {14, 15, 16};

INSTANTIATE_TEST_CASE_P(throw_test3, MyParamTest, ::testing::ValuesIn(VALS, VALS + 3));

//typed-test
template<class T>
struct CListTest : public ::testing::Test
{
    typedef std::list<T> __List;
    static void SetUpTestCase(){
        cout<<"CListTest<T>::SetUpTestCase()\n";
    }
    static void TearDownTestCase(){
        cout<<"CListTest<T>::TearDownTestCase()\n";
    }
    void SetUp(){
        cout<<"CListTest<T>::SetUp()\n";
    }
    void TearDown(){
        cout<<"CListTest<T>::TearDown()\n";
    }
};

typedef ::testing::Types<int, char, std::string> MyTypes;
TYPED_TEST_CASE(CListTest, MyTypes);

TYPED_TEST(CListTest, a)
{
    typename TestFixture::__List list;
    ASSERT_TRUE(list.empty());
    list.push_back(TypeParam());
    ASSERT_FALSE(list.empty());
}

TYPED_TEST(CListTest, b)
{
    typename TestFixture::__List list;
    ASSERT_EQ(0u, list.size());
    list.push_back(TypeParam());
    ASSERT_EQ(1u, list.size());
}

//type-parameterized
TYPED_TEST_CASE_P(CListTest);

TYPED_TEST_P(CListTest, a)
{
    typename TestFixture::__List list;
    ASSERT_TRUE(list.empty());
    list.push_back(TypeParam());
    ASSERT_FALSE(list.empty());
}

TYPED_TEST_P(CListTest, b)
{
    typename TestFixture::__List list;
    ASSERT_EQ(0u, list.size());
    list.push_back(TypeParam());
    ASSERT_EQ(1u, list.size());
}

REGISTER_TYPED_TEST_CASE_P(CListTest, a, b);

INSTANTIATE_TYPED_TEST_CASE_P(vector, CListTest, vector<long>);

INSTANTIATE_TYPED_TEST_CASE_P(types, CListTest, MyTypes);

