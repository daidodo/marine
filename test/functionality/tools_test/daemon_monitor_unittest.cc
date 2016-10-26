#include <marine/tools/system.hh>

#include <marine/file.hh>
#include "../inc.h"

TEST(DaemonMonitor, all)
{
    std::string path = tools::AbsFilename("dm.tmp");

    tools::DaemonMonitor();

    ::sleep(1);
    CFile f(path, O_RDWR | O_CREAT, 0666);
    ASSERT_TRUE(f.valid())<<tools::ErrorMsg(errno);
    int x = 0;
    char * p = reinterpret_cast<char *>(&x);
    ASSERT_TRUE(f.seek(0, SEEK_SET));
    if(ssize_t(sizeof x) != f.read(p, sizeof x)){
        //first child
        if(fork() == 0){
            //check process
            cout<<"check process start\n";
            sleep(15);
            ASSERT_TRUE(f.seek(0, SEEK_SET));
            ASSERT_EQ(ssize_t(sizeof x), f.read(p, sizeof x))<<tools::ErrorMsg(errno);
            ASSERT_EQ(5, x);
            f.unlink();
            cout<<"check process exit\n";
            ::exit(0);
        }
    }
    if(++x <= 5){
        ASSERT_TRUE(f.seek(0, SEEK_SET));
        ASSERT_EQ(ssize_t(sizeof x), f.write(p, sizeof x))<<tools::ErrorMsg(errno);
        f.close();
        cout<<"child process "<<getpid()<<" abort, x="<<x<<"\n";
        ::abort();
    }
    FAIL()<<"freq limit is incorrect or dm.tmp is not removed";
    cout<<"child process "<<getpid()<<" exit\n";
    ::exit(1);
}

