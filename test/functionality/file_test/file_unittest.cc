#include <marine/file.hh>

#include "../inc.h"
#include <marine/tools/debug.hh>

TEST(CFile, all)
{
    const char * const FNAME = "ftest.txt";

    //ctor
    CFile f1;
    ASSERT_FALSE(f1.valid());

    CFile f2("not_exist_file");
    ASSERT_FALSE(f2.valid());

    CFile f3(FNAME, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

    CFile f5("del_test", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

    //valid
    ASSERT_TRUE(f3.valid());

    //fd
    ASSERT_TRUE(CFile::kInvalidFd == f1.fd());
    ASSERT_TRUE(CFile::kInvalidFd == f2.fd());
    ASSERT_TRUE(CFile::kInvalidFd != f3.fd());

    //fdType
    ASSERT_TRUE(CFile::kFdType == f1.fdType());
    ASSERT_TRUE(CFile::kFdType == f2.fdType());
    ASSERT_TRUE(CFile::kFdType == f3.fdType());

    //fdTypeName
    ASSERT_STREQ("CFile", f1.fdTypeName());
    ASSERT_STREQ("CFile", f2.fdTypeName());
    ASSERT_STREQ("CFile", f3.fdTypeName());

    //pathname
    ASSERT_NE(std::string(), f3.filename());

    //write, length
    std::string str("abddegl;nda;dnag");
    std::vector<char> buf(str.begin(), str.end());
    ASSERT_EQ(0, f3.length());
    ASSERT_EQ(str.length(), size_t(f3.write(str.c_str(), str.length())));
    ASSERT_EQ(str.length(), (size_t)f3.length());
    ASSERT_EQ(str.length(), size_t(f3.write(str)));
    ASSERT_EQ(2 * str.length(), (size_t)f3.length());
    ASSERT_EQ(buf.size(), size_t(f3.write(buf)));
    ASSERT_EQ(3 * str.length(), (size_t)f3.length());

    //deleted
    ASSERT_TRUE(f5.valid());
    ASSERT_FALSE(f5.deleted());
    system("rm -f del_test");
    ASSERT_TRUE(f5.deleted());

    //read
    ASSERT_TRUE(f2.open(FNAME));
    ASSERT_NE(std::string(), f2.filename());

    std::vector<char> buf2(buf.size());
    ASSERT_EQ(buf2.size(), size_t(f2.read(&buf2[0], buf2.size())));
    ASSERT_EQ(buf, buf2);

    //tell
    off_t off = f2.tell();
    ASSERT_NE(-1, off)<<"off="<<off<<endl;

    std::vector<char> buf3;
    ASSERT_TRUE(f2.read(buf3, buf.size(), true));
    ASSERT_EQ(buf, buf3);

    //seek
    ASSERT_TRUE(f2.seek(off, SEEK_SET));

    //copy
    CFile f4(f2);
    ASSERT_TRUE(f4.valid());

    //assignment
    ASSERT_FALSE(f1.valid());
    f1 = f2;
    ASSERT_TRUE(f1.valid());
    ASSERT_EQ(off, f1.tell());  //offset也会复制
    ASSERT_EQ(off, f4.tell());  //offset也会复制

    std::vector<char> buf4(3, 'a');
    ASSERT_TRUE(f1.read(buf4, buf.size(), true));
    ASSERT_NE(buf, buf4)<<"buf4="<<tools::Dump(buf4)<<endl;

    ASSERT_TRUE(f1.tell() == f2.tell());
    ASSERT_TRUE(f4.tell() == f2.tell());

    f1.close();
    ASSERT_FALSE(f1.valid());

    std::string str2("abcd");
    ASSERT_TRUE(f4.read(str2, str.length(), false));
    ASSERT_EQ(str, str2);

    f4.close();
    ASSERT_FALSE(f4.valid());

#ifdef __HAS_LSEEK64
    ASSERT_TRUE(f2.seek(off, SEEK_SET));
    //tell64
    off64_t off64 = f2.tell64();
    ASSERT_EQ(off, off64);

    //seek64
    ASSERT_TRUE(f2.seek64(off64, SEEK_SET));

    std::vector<char> buf5(3, 'a');
    ASSERT_TRUE(f2.read(buf5, buf.size(), true));
    ASSERT_NE(buf, buf5)<<"buf5="<<tools::Dump(buf5)<<endl;

    std::string str3("abcd");
    ASSERT_TRUE(f2.read(str3, str.length(), false));
    ASSERT_EQ(str, str3);
#endif

#ifdef __HAS_FTRUNCATE
    //truncate
    ASSERT_TRUE(f3.valid());
    ASSERT_TRUE(f3.truncate(1000))<<f2.toString()<<endl<<CFile::ErrMsg();
#endif

    //setBlock
    ASSERT_TRUE(f2.block(true));
    ASSERT_TRUE(f2.block(false));

    //close
    ASSERT_TRUE(f2.valid());
    f2.close();
    ASSERT_FALSE(f2.valid());

    //toString
    std::ostringstream oss;
    oss<<"{IFileDesc={fd_="<<f3.fd()<<"(CFile)}, filename="<<f3.filename()<<"}";
    ASSERT_EQ(oss.str(), f3.toString())<<"f3="<<f3.toString()<<endl;

    //rename
    ASSERT_TRUE(f3.rename("rn.txt"));
    ASSERT_TRUE(CFile::Rename("rn.txt", FNAME));

    //unlink
    ASSERT_TRUE(CFile::Unlink(FNAME));
}
