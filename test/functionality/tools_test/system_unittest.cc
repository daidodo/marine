#include <marine/tools/system.hh>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "../inc.h"

TEST(SetMaxFileDescriptor, GetMaxFileDescriptor)
{
    uint32_t n = tools::GetMaxFileDescriptor();
    EXPECT_TRUE(n > 0);

    n = 100;
    EXPECT_EQ(true, tools::SetMaxFileDescriptor(n))
        <<"errno "<<errno<<": "<<strerror(errno)<<endl;
    EXPECT_EQ(n, tools::GetMaxFileDescriptor());
}

TEST(GetProcessorCount, all)
{
    int n = tools::GetProcessorCount();
    EXPECT_TRUE(n > 0);
    cout<<"Processor count: "<<n<<endl;
}

TEST(GetPhysicalMemorySize, all)
{
    uint64_t n = tools::GetPhysicalMemorySize();
    EXPECT_TRUE(n > 0);
    cout<<"Physical Memory: "<<n<<" bytes\n";
}

TEST(GetPageSize, all)
{
    size_t n = tools::GetPageSize();
    EXPECT_TRUE(n > 0);
    cout<<"Page Size: "<<n<<" bytes\n";
}

TEST(ErrorMsg, all)
{
    EXPECT_EQ(" errno 1 - Operation not permitted", tools::ErrorMsg(1));
}

TEST(Daemon, all)
{
    if(fork() == 0){
        tools::Daemon();
        sleep(1);
        exit(0);
    }
}

TEST(GetFilenameByFd, all)
{
    int fd = ::open("/tmp/marine_test.txt", O_RDWR | O_CREAT, 0666);
    ASSERT_TRUE(fd >= 0)<<"fd="<<fd<<tools::ErrorMsg(errno);
    ASSERT_EQ("/tmp/marine_test.txt", tools::GetFilenameByFd(fd));
}

TEST(GetProcessMemoryUsage, all)
{
    size_t vm, rs;

    ASSERT_TRUE(tools::GetProcessMemoryUsage(NULL, NULL));

    vm = 0;
    ASSERT_TRUE(tools::GetProcessMemoryUsage(&vm, NULL));
    ASSERT_NE(size_t(0), vm);

    rs = 0;
    ASSERT_TRUE(tools::GetProcessMemoryUsage(NULL, &rs));
    ASSERT_NE(size_t(0), rs);

    vm = rs = 0;
    ASSERT_TRUE(tools::GetProcessMemoryUsage(&vm, &rs));
    ASSERT_NE(size_t(0), vm);
    ASSERT_NE(size_t(0), rs);
}

const char * fname = "system_unittest.cc";
const key_t key = ::ftok(fname, 1);

struct CEnv : public ::testing::Environment
{
    void SetUp(){
        CXsiSemaphoreSet sem(key);
        sem.destroy();
    }
};

__ADD_GTEST_ENV(CEnv, CXsiSemaphoreSet);

TEST(ProcessExclusion, all)
{
    ASSERT_GT(key, -1);
    ASSERT_TRUE(tools::ProcessExclusion(key, 1000));
    {
        pid_t pid = ::fork();
        ASSERT_GT(pid, -1);
        if(0 != pid){
            time_t now = time(NULL);
            ASSERT_FALSE(tools::ProcessExclusion(key, 1000));
            ASSERT_EQ(time(NULL), 1 + now);
            ::exit(0);
        }else{
            int ret = 0;
            ::wait(&ret);
            ASSERT_EQ(0, ret);
        }
    }{
        pid_t pid = ::fork();
        ASSERT_GT(pid, -1);
        if(0 != pid){
            time_t now = time(NULL);
            ASSERT_FALSE(tools::ProcessExclusion(fname, 1000));
            ASSERT_EQ(time(NULL), 1 + now);
            ::exit(0);
        }else{
            int ret = 0;
            ::wait(&ret);
            ASSERT_EQ(0, ret);
        }
    }{
        pid_t pid = ::fork();
        ASSERT_GT(pid, -1);
        if(0 != pid){
            time_t now = time(NULL);
            ASSERT_FALSE(tools::ProcessExclusion(fname, 1, 0));
            ASSERT_EQ(time(NULL), now);
            ::exit(0);
        }else{
            int ret = 0;
            ::wait(&ret);
            ASSERT_EQ(0, ret);
        }
    }
}
