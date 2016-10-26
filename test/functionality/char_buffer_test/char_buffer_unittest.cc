#include <marine/char_buffer.hh>

#include "../inc.h"


typedef char    Char;
#define __CharP  (Char *)

TEST(CharBuffer, all)
{
    typedef CCharBuffer<Char> __Buffer;
    Char str[100] = {'a','b', 'c', 'd', 'e', 'f', 'g'};
    //ctor
    __Buffer buf1;
    __Buffer buf2(__CharP"abcdefg");
    const __Buffer buf3(str, sizeof str, 7);
    __Buffer buf4 = __CharP"abcdefg";
    EXPECT_FALSE(buf1 == buf2);
    EXPECT_FALSE(buf2 != buf3);
    EXPECT_FALSE(buf3 != buf4);
    EXPECT_FALSE(buf2 != buf4);
    EXPECT_FALSE(!buf1.empty());
    EXPECT_FALSE(7 != buf2.size() || 7 != buf2.capacity());
    EXPECT_FALSE(sizeof str != buf3.capacity());
    EXPECT_FALSE(7 != buf4.capacity());
    EXPECT_FALSE(7 != buf4.length());
    //assign
    buf1.assign(buf3);
    EXPECT_FALSE(buf1 != buf3);
    EXPECT_FALSE(sizeof str != buf1.capacity());
    buf2.assign(str, sizeof str, 7);
    EXPECT_FALSE(buf2 != buf3);
    EXPECT_FALSE(sizeof str != buf2.capacity());
    //copy
    Char str2[8] = {};
    EXPECT_FALSE(7 != buf2.copy(str2, 7));
    EXPECT_FALSE(0 != strcmp((const char *)str, (const char *)str2));
    //begin, end
    __Buffer::value_type ch = 'A';
    typedef __Buffer::iterator __Iter;
    typedef __Buffer::const_iterator __CIter;
    for(__Iter i = buf1.begin();i != buf1.end();++i)
        *i = ch++;
    ch = 'A';
    for(__CIter i = buf3.begin();i != buf3.end();++i)
        EXPECT_FALSE(*i != ch++);
    //rbegin, rend
    typedef __Buffer::reverse_iterator __RIter;
    typedef __Buffer::const_reverse_iterator __CRIter;
    ch = 'A';
    for(__RIter i = buf1.rbegin();i != buf1.rend();++i)
        *i = ch++;
    ch = 'A';
    for(__CRIter i = buf3.rbegin();i != buf3.rend();++i)
        EXPECT_FALSE(*i != ch++);
    //operator []
    ch = 'A';
    for(size_t i = 0;i < buf1.size();++i)
        buf1[i] = ch++;
    ch = 'A';
    for(size_t i = 0;i < buf3.size();++i)
        EXPECT_FALSE(buf3[i] != ch++);
    //back
    buf1.back() = 'x';
    EXPECT_FALSE(buf3.back() != 'x');
    //at
    ch = 'A';
    for(size_t i = 0;i < buf1.size();++i)
        buf1.at(i) = ch++;
    ch = 'A';
    for(size_t i = 0;i < buf3.size();++i)
        EXPECT_FALSE(buf3.at(i) != ch++);
    //clear, empty
    EXPECT_FALSE(buf2.empty());
    buf2.clear();
    EXPECT_FALSE(!buf2.empty());
    //resize
    buf2.resize(7);
    EXPECT_FALSE(7 != buf2.size());
    //push_back
    buf2.clear();
    EXPECT_FALSE(buf2 == buf4);
    ch = 'a';
    for(int i = 0;i < 7;++i)
        buf2.push_back(ch + i);
    EXPECT_FALSE(buf2 != buf4);
    //swap
    buf2.swap(buf4);
    EXPECT_FALSE(str == &buf2[0]);
    EXPECT_FALSE(str != &buf4[0]);
    swap(buf4, buf2);
    EXPECT_FALSE(str == &buf4[0]);
    EXPECT_FALSE(str != &buf2[0]);
    //append
    buf1.clear();
    for(size_t i = 0;i < 10;++i)
        buf1.append(10, 'd');
    EXPECT_FALSE(100 != buf1.size());
    for(size_t i = 0;i < buf1.size();++i)
        EXPECT_FALSE(buf1[i] != 'd');
    buf1.clear();
    for(size_t i = 0;i < 20;++i)
        buf1.append(__CharP"ssdeb");
    EXPECT_FALSE(100 != buf1.size());
    for(size_t i = 0;i < buf1.size();i += 5)
        EXPECT_FALSE(0 != memcmp(&buf1[i], "ssdeb", 5));
    buf4.assign(__CharP"gadfdengb");
    buf1.clear();
    for(size_t i = 0;i < 20;++i)
        buf1.append(buf4, 4, 5);
    EXPECT_FALSE(100 != buf1.size());
    for(size_t i = 0;i < buf1.size();i += 5)
        EXPECT_FALSE(0 != memcmp(&buf1[i], "dengb", 5));
    buf4.resize(5);
    buf1.clear();
    for(size_t i = 0;i < 20;++i)
        buf1.append(buf4);
    EXPECT_FALSE(100 != buf1.size());
    for(size_t i = 0;i < buf1.size();i += 5)
        EXPECT_FALSE(0 != memcmp(&buf1[i], "gadfd", 5));
    //operator +=
    buf1.clear();
    for(size_t i = 0;i < 20;++i)
        buf1 += __CharP"sgdeb";
    EXPECT_FALSE(100 != buf1.size());
    for(size_t i = 0;i < buf1.size();i += 5)
        EXPECT_FALSE(0 != memcmp(&buf1[i], "sgdeb", 5));
    buf4.assign(__CharP"f90rg");
    buf1.clear();
    for(size_t i = 0;i < 20;++i)
        buf1 += buf4;
    EXPECT_FALSE(100 != buf1.size());
    for(size_t i = 0;i < buf1.size();i += 5)
        EXPECT_FALSE(0 != memcmp(&buf1[i], "f90rg", 5));
    buf1.clear();
    for(int i = 0;i < 100;++i)
        buf1 += 12 + i;
    EXPECT_FALSE(100 != buf1.size());
    for(size_t i = 0;i < buf1.size();++i)
        EXPECT_FALSE(buf1[i] != 12 + int(i))<<"buf1["<<i<<"]="<<int(buf1[i])<<" is not "<<(12 + int(i))<<endl;
    //insert
    buf4.assign(__CharP"3e4r5");
    buf1.clear();
    buf1 += buf4;
    EXPECT_FALSE(5 != buf1.size());
    for(int i = 0;i < 19;++i)
        buf1.insert(i * 5 + 3, 5, 'a');
    EXPECT_FALSE(100 != buf1.size())<<"1: buf1.size()="<<buf1.size()<<" is not 100\n";
    EXPECT_FALSE(0 != memcmp(&buf1[0], &buf4[0], 3))<<"1: buf1[0]="<<&buf1[0]<<", buf4[0]="<<&buf4[0]<<endl;
    for(int i = 3;i < 98;++i)
        EXPECT_FALSE('a' != buf1[i]);
    EXPECT_FALSE(0 != memcmp(&buf1[98], &buf4[3], 2));
    buf1.clear();
    buf1 += buf4;
    for(int i = 0;i < 19;++i)
        buf1.insert(i * 5 + 3, __CharP"3g89a", 5);
    EXPECT_FALSE(100 != buf1.size())<<"2: buf1.size()="<<buf1.size()<<" is not 100\n";
    EXPECT_FALSE(0 != memcmp(&buf1[0], &buf4[0], 3))<<"2: buf1[0]="<<&buf1[0]<<", buf4[0]="<<&buf4[0]<<endl;
    for(int i = 0;i < 19;++i)
        EXPECT_FALSE(0 != memcmp("3g89a", &buf1[i * 5 + 3], 5));
    EXPECT_FALSE(0 != memcmp(&buf1[98], &buf4[3], 2));
    buf1.clear();
    buf1 += buf4;
    for(int i = 0;i < 19;++i)
        buf1.insert(i * 5 + 3, __CharP"3g89a");
    EXPECT_FALSE(100 != buf1.size())<<"3: buf1.size()="<<buf1.size()<<" is not 100\n";
    EXPECT_FALSE(0 != memcmp(&buf1[0], &buf4[0], 3))<<"3: buf1[0]="<<&buf1[0]<<", buf4[0]="<<&buf4[0]<<endl;
    for(int i = 0;i < 19;++i)
        EXPECT_FALSE(0 != memcmp("3g89a", &buf1[i * 5 + 3], 5));
    EXPECT_FALSE(0 != memcmp(&buf1[98], &buf4[3], 2));
    buf4.assign(__CharP"23e32t2g22");
    buf1.clear();
    buf1.append(buf4, 0, 5);
    for(int i = 0;i < 19;++i)
        buf1.insert(i * 5 + 3, buf4, 4, 5);
    EXPECT_FALSE(100 != buf1.size())<<"4: buf1.size()="<<buf1.size()<<" is not 100\n";
    EXPECT_FALSE(0 != memcmp(&buf1[0], &buf4[0], 3))<<"4: buf1[0]="<<&buf1[0]<<", buf4[0]="<<&buf4[0]<<endl;
    for(int i = 0;i < 19;++i)
        EXPECT_FALSE(0 != memcmp(&buf4[4], &buf1[i * 5 + 3], 5));
    EXPECT_FALSE(0 != memcmp(&buf1[98], &buf4[3], 2));
    buf4.resize(5);
    buf1.clear();
    buf1.append(buf4);
    for(int i = 0;i < 19;++i)
        buf1.insert(i * 5 + 3, buf4);
    EXPECT_FALSE(100 != buf1.size())<<"5: buf1.size()="<<buf1.size()<<" is not 100\n";
    EXPECT_FALSE(0 != memcmp(&buf1[0], &buf4[0], 3))<<"5: buf1[0]="<<&buf1[0]<<", buf4[0]="<<&buf4[0]<<endl;
    for(int i = 0;i < 19;++i)
        EXPECT_FALSE(0 != memcmp(&buf4[0], &buf1[i * 5 + 3], 5));
    EXPECT_FALSE(0 != memcmp(&buf1[98], &buf4[3], 2));
    buf1.clear();
    buf1.append(buf4);
    for(int i = 0;i < 95;++i){
        __Iter p = buf1.insert(buf1.begin() + i + 3, 12 + i);
        EXPECT_FALSE(*p != 12 + i);
    }
    EXPECT_FALSE(100 != buf1.size())<<"6: buf1.size()="<<buf1.size()<<" is not 100\n";
    EXPECT_FALSE(0 != memcmp(&buf1[0], &buf4[0], 3))<<"6: buf1[0]="<<&buf1[0]<<", buf4[0]="<<&buf4[0]<<endl;
    for(int i = 0;i < 95;++i)
        EXPECT_FALSE(12 + i != buf1[i + 3]);
    EXPECT_FALSE(0 != memcmp(&buf1[98], &buf4[3], 2));
    //replace
    buf1.clear();
    buf1 += __CharP"0123456789";
    EXPECT_FALSE(10 != buf1.size());
    EXPECT_FALSE(0 != memcmp("0123456789", &buf1[0], buf1.size()));
    buf1.replace(4, 3, 3, '0');
    EXPECT_FALSE(10 != buf1.size());
    EXPECT_FALSE(0 != memcmp("0123000789", &buf1[0], buf1.size()));
    buf1.replace(4, 3, 5, '1');
    EXPECT_FALSE(12 != buf1.size());
    EXPECT_FALSE(0 != memcmp("012311111789", &buf1[0], buf1.size()));
    buf1.replace(4, 5, 3, 'a');
    EXPECT_FALSE(10 != buf1.size());
    EXPECT_FALSE(0 != memcmp("0123aaa789", &buf1[0], buf1.size()));
    buf1.clear();
    buf1 += __CharP"0123456789";
    EXPECT_FALSE(10 != buf1.size());
    EXPECT_FALSE(0 != memcmp("0123456789", &buf1[0], buf1.size()));
    buf1.replace(4, 3, __CharP"000", 3);
    EXPECT_FALSE(10 != buf1.size());
    EXPECT_FALSE(0 != memcmp("0123000789", &buf1[0], buf1.size()));
    buf1.replace(4, 3, __CharP"11111", 5);
    EXPECT_FALSE(12 != buf1.size());
    EXPECT_FALSE(0 != memcmp("012311111789", &buf1[0], buf1.size()));
    buf1.replace(4, 5, __CharP"aaa", 3);
    EXPECT_FALSE(10 != buf1.size());
    EXPECT_FALSE(0 != memcmp("0123aaa789", &buf1[0], buf1.size()));
    buf1.clear();
    buf1 += __CharP"0123456789";
    EXPECT_FALSE(10 != buf1.size());
    EXPECT_FALSE(0 != memcmp("0123456789", &buf1[0], buf1.size()));
    buf1.replace(4, 3, __CharP"000");
    EXPECT_FALSE(10 != buf1.size());
    EXPECT_FALSE(0 != memcmp("0123000789", &buf1[0], buf1.size()));
    buf1.replace(4, 3, __CharP"11111");
    EXPECT_FALSE(12 != buf1.size());
    EXPECT_FALSE(0 != memcmp("012311111789", &buf1[0], buf1.size()));
    buf1.replace(4, 5, __CharP"aaa");
    EXPECT_FALSE(10 != buf1.size());
    EXPECT_FALSE(0 != memcmp("0123aaa789", &buf1[0], buf1.size()));
    buf4.assign(__CharP"00011111aaa");
    buf1.clear();
    buf1 += __CharP"0123456789";
    EXPECT_FALSE(10 != buf1.size());
    EXPECT_FALSE(0 != memcmp("0123456789", &buf1[0], buf1.size()));
    buf1.replace(4, 3, buf4, 0, 3);
    EXPECT_FALSE(10 != buf1.size());
    EXPECT_FALSE(0 != memcmp("0123000789", &buf1[0], buf1.size()));
    buf1.replace(4, 3, buf4, 3, 5);
    EXPECT_FALSE(12 != buf1.size());
    EXPECT_FALSE(0 != memcmp("012311111789", &buf1[0], buf1.size()));
    buf1.replace(4, 5, buf4, 8, 3);
    EXPECT_FALSE(10 != buf1.size());
    EXPECT_FALSE(0 != memcmp("0123aaa789", &buf1[0], buf1.size()));
    buf1.clear();
    buf1 += __CharP"0123456789";
    EXPECT_FALSE(10 != buf1.size());
    EXPECT_FALSE(0 != memcmp("0123456789", &buf1[0], buf1.size()));
    buf4.assign(__CharP"000");
    buf1.replace(4, 3, buf4);
    EXPECT_FALSE(10 != buf1.size());
    EXPECT_FALSE(0 != memcmp("0123000789", &buf1[0], buf1.size()));
    buf4.assign(__CharP"11111");
    buf1.replace(4, 3, buf4);
    EXPECT_FALSE(12 != buf1.size());
    EXPECT_FALSE(0 != memcmp("012311111789", &buf1[0], buf1.size()));
    buf4.assign(__CharP"aaa");
    buf1.replace(4, 5, buf4);
    EXPECT_FALSE(10 != buf1.size());
    EXPECT_FALSE(0 != memcmp("0123aaa789", &buf1[0], buf1.size()));

    buf1.clear();
    buf1 += __CharP"0123456789";
    EXPECT_FALSE(10 != buf1.size());
    EXPECT_FALSE(0 != memcmp("0123456789", &buf1[0], buf1.size()));
    buf1.replace(buf1.begin() + 4, buf1.begin() + 7, 3, '0');
    EXPECT_FALSE(10 != buf1.size());
    EXPECT_FALSE(0 != memcmp("0123000789", &buf1[0], buf1.size()));
    buf1.replace(buf1.begin() + 4, buf1.begin() + 7, 5, '1');
    EXPECT_FALSE(12 != buf1.size());
    EXPECT_FALSE(0 != memcmp("012311111789", &buf1[0], buf1.size()));
    buf1.replace(buf1.begin() + 4, buf1.begin() + 9, 3, 'a');
    EXPECT_FALSE(10 != buf1.size());
    EXPECT_FALSE(0 != memcmp("0123aaa789", &buf1[0], buf1.size()));
    buf1.clear();
    buf1 += __CharP"0123456789";
    EXPECT_FALSE(10 != buf1.size());
    EXPECT_FALSE(0 != memcmp("0123456789", &buf1[0], buf1.size()));
    buf1.replace(buf1.begin() + 4, buf1.begin() + 7, __CharP"000", 3);
    EXPECT_FALSE(10 != buf1.size());
    EXPECT_FALSE(0 != memcmp("0123000789", &buf1[0], buf1.size()));
    buf1.replace(buf1.begin() + 4, buf1.begin() + 7, __CharP"11111", 5);
    EXPECT_FALSE(12 != buf1.size());
    EXPECT_FALSE(0 != memcmp("012311111789", &buf1[0], buf1.size()));
    buf1.replace(buf1.begin() + 4, buf1.begin() + 9, __CharP"aaa", 3);
    EXPECT_FALSE(10 != buf1.size());
    EXPECT_FALSE(0 != memcmp("0123aaa789", &buf1[0], buf1.size()));
    buf1.clear();
    buf1 += __CharP"0123456789";
    EXPECT_FALSE(10 != buf1.size());
    EXPECT_FALSE(0 != memcmp("0123456789", &buf1[0], buf1.size()));
    buf1.replace(buf1.begin() + 4, buf1.begin() + 7, __CharP"000");
    EXPECT_FALSE(10 != buf1.size());
    EXPECT_FALSE(0 != memcmp("0123000789", &buf1[0], buf1.size()));
    buf1.replace(buf1.begin() + 4, buf1.begin() + 7, __CharP"11111");
    EXPECT_FALSE(12 != buf1.size());
    EXPECT_FALSE(0 != memcmp("012311111789", &buf1[0], buf1.size()));
    buf1.replace(buf1.begin() + 4, buf1.begin() + 9, __CharP"aaa");
    EXPECT_FALSE(10 != buf1.size());
    EXPECT_FALSE(0 != memcmp("0123aaa789", &buf1[0], buf1.size()));
    buf4.assign(__CharP"00011111aaa");
    buf1.clear();
    buf1 += __CharP"0123456789";
    EXPECT_FALSE(10 != buf1.size());
    EXPECT_FALSE(0 != memcmp("0123456789", &buf1[0], buf1.size()));
    buf1.replace(buf1.begin() + 4, buf1.begin() + 7, buf4, 0, 3);
    EXPECT_FALSE(10 != buf1.size());
    EXPECT_FALSE(0 != memcmp("0123000789", &buf1[0], buf1.size()));
    buf1.replace(buf1.begin() + 4, buf1.begin() + 7, buf4, 3, 5);
    EXPECT_FALSE(12 != buf1.size());
    EXPECT_FALSE(0 != memcmp("012311111789", &buf1[0], buf1.size()));
    buf1.replace(buf1.begin() + 4, buf1.begin() + 9, buf4, 8, 3);
    EXPECT_FALSE(10 != buf1.size());
    EXPECT_FALSE(0 != memcmp("0123aaa789", &buf1[0], buf1.size()));
    buf1.clear();
    buf1 += __CharP"0123456789";
    EXPECT_FALSE(10 != buf1.size());
    EXPECT_FALSE(0 != memcmp("0123456789", &buf1[0], buf1.size()));
    buf4.assign(__CharP"000");
    buf1.replace(buf1.begin() + 4, buf1.begin() + 7, buf4);
    EXPECT_FALSE(10 != buf1.size());
    EXPECT_FALSE(0 != memcmp("0123000789", &buf1[0], buf1.size()));
    buf4.assign(__CharP"11111");
    buf1.replace(buf1.begin() + 4, buf1.begin() + 7, buf4);
    EXPECT_FALSE(12 != buf1.size());
    EXPECT_FALSE(0 != memcmp("012311111789", &buf1[0], buf1.size()));
    buf4.assign(__CharP"aaa");
    buf1.replace(buf1.begin() + 4, buf1.begin() + 9, buf4);
    EXPECT_FALSE(10 != buf1.size());
    EXPECT_FALSE(0 != memcmp("0123aaa789", &buf1[0], buf1.size()));
    //erase
    buf1.clear();
    buf1 += __CharP"0123456789";
    EXPECT_FALSE(10 != buf1.size());
    EXPECT_FALSE(0 != memcmp("0123456789", &buf1[0], buf1.size()));
    __Iter p = buf1.erase(buf1.begin() + 1, buf1.end() - 2);
    EXPECT_FALSE(*p != '8');
    EXPECT_FALSE(3 != buf1.size());
    EXPECT_FALSE(0 != memcmp("089", &buf1[0], buf1.size()));
    buf1.clear();
    buf1 += __CharP"0123456789";
    EXPECT_FALSE(10 != buf1.size());
    EXPECT_FALSE(0 != memcmp("0123456789", &buf1[0], buf1.size()));
    p = buf1.erase(buf1.begin() + 5);
    EXPECT_FALSE(*p != '6');
    EXPECT_FALSE(9 != buf1.size());
    EXPECT_FALSE(0 != memcmp("012346789", &buf1[0], buf1.size()));
    buf1.clear();
    buf1 += __CharP"0123456789";
    EXPECT_FALSE(10 != buf1.size());
    EXPECT_FALSE(0 != memcmp("0123456789", &buf1[0], buf1.size()));
    buf1.erase(3, 4);
    EXPECT_FALSE(6 != buf1.size());
    EXPECT_FALSE(0 != memcmp("012789", &buf1[0], buf1.size()));
    buf1.clear();
    buf1 += __CharP"0123456789";
    EXPECT_FALSE(10 != buf1.size());
    EXPECT_FALSE(0 != memcmp("0123456789", &buf1[0], buf1.size()));
    buf1.erase(6);
    EXPECT_FALSE(6 != buf1.size())<<"buf1.size()="<<buf1.size()<<" is not 6\n";
    EXPECT_FALSE(0 != memcmp("012345", &buf1[0], buf1.size()));
    //substr
    buf1.clear();
    buf1 += __CharP"0123456789";
    EXPECT_FALSE(10 != buf1.size());
    EXPECT_FALSE(0 != memcmp("0123456789", &buf1[0], buf1.size()));
    buf4 = buf1.substr(2, 7);
    EXPECT_FALSE(7 != buf4.size());
    EXPECT_FALSE(0 != memcmp("2345678", &buf4[0], buf4.size()));
    //compare
    buf1.clear();
    buf1 += __CharP"0123456789";
    EXPECT_FALSE(10 != buf1.size());
    EXPECT_TRUE(0 > buf1.compare(2, 5, __CharP"256444", 5));
    EXPECT_TRUE(0 < buf1.compare(2, 5, __CharP"226444", 5));
    EXPECT_TRUE(0 == buf1.compare(2, 5, __CharP"23456444", 5));
    EXPECT_TRUE(0 > buf1.compare(2, 5, __CharP"256444"));
    EXPECT_TRUE(0 < buf1.compare(2, 5, __CharP"226444"));
    EXPECT_TRUE(0 == buf1.compare(2, 5, __CharP"23456"));
    EXPECT_TRUE(0 < buf1.compare(__CharP"0123425"));
    EXPECT_TRUE(0 > buf1.compare(__CharP"0124425"));
    EXPECT_TRUE(0 == buf1.compare(__CharP"0123456789"));
    buf4.assign(__CharP"238623456444");
    EXPECT_TRUE(0 < buf1.compare(2, 6, buf4, 4, 8));
    EXPECT_TRUE(0 > buf1.compare(2, 5, buf4, 4, 8));
    EXPECT_TRUE(0 == buf1.compare(2, 5, buf4, 4, 5));
    buf4.assign(__CharP"23456");
    EXPECT_TRUE(0 > buf1.compare(2, 4, buf4));
    EXPECT_TRUE(0 < buf1.compare(2, 6, buf4));
    EXPECT_TRUE(0 == buf1.compare(2, 5, buf4));
    buf4.assign(__CharP"2623424");
    EXPECT_TRUE(0 > buf1.compare(buf4));
    EXPECT_FALSE(buf1 == buf4);
    buf4.assign(__CharP"01223434");
    EXPECT_TRUE(0 < buf1.compare(buf4));
    EXPECT_FALSE(buf1 == buf4);
    buf4.assign(__CharP"0123456789");
    EXPECT_TRUE(0 == buf1.compare(buf4));
    EXPECT_FALSE(buf1 != buf4);
    //operators ==, !=, <, <=, >, >=
    buf4.assign(__CharP"0123455789ag");
    EXPECT_FALSE((buf1 == buf4));
    EXPECT_FALSE(!(buf1 != buf4));
    EXPECT_FALSE((buf1 < buf4));
    EXPECT_FALSE((buf1 <= buf4));
    EXPECT_FALSE(!(buf1 > buf4));
    EXPECT_FALSE(!(buf1 >= buf4));
    buf4.assign(__CharP"012345678");
    EXPECT_FALSE((buf1 == buf4));
    EXPECT_FALSE(!(buf1 != buf4));
    EXPECT_FALSE((buf1 < buf4));
    EXPECT_FALSE((buf1 <= buf4));
    EXPECT_FALSE(!(buf1 > buf4));
    EXPECT_FALSE(!(buf1 >= buf4));
    buf4.assign(__CharP"0123456789");
    EXPECT_FALSE(!(buf1 == buf4));
    EXPECT_FALSE((buf1 != buf4));
    EXPECT_FALSE((buf1 < buf4));
    EXPECT_FALSE(!(buf1 <= buf4));
    EXPECT_FALSE((buf1 > buf4));
    EXPECT_FALSE(!(buf1 >= buf4));
    buf4.assign(__CharP"012345688");
    EXPECT_FALSE((buf1 == buf4));
    EXPECT_FALSE(!(buf1 != buf4));
    EXPECT_FALSE(!(buf1 < buf4));
    EXPECT_FALSE(!(buf1 <= buf4));
    EXPECT_FALSE((buf1 > buf4));
    EXPECT_FALSE((buf1 >= buf4));
    buf4.assign(__CharP"01234567890");
    EXPECT_FALSE((buf1 == buf4));
    EXPECT_FALSE(!(buf1 != buf4));
    EXPECT_FALSE(!(buf1 < buf4));
    EXPECT_FALSE(!(buf1 <= buf4));
    EXPECT_FALSE((buf1 > buf4));
    EXPECT_FALSE((buf1 >= buf4));

    const Char * cstr = __CharP"0123455789ag";
    EXPECT_FALSE((buf1 == cstr));
    EXPECT_FALSE(!(buf1 != cstr));
    EXPECT_FALSE((buf1 < cstr));
    EXPECT_FALSE((buf1 <= cstr));
    EXPECT_FALSE(!(buf1 > cstr));
    EXPECT_FALSE(!(buf1 >= cstr));
    cstr = __CharP"012345678";
    EXPECT_FALSE((buf1 == cstr));
    EXPECT_FALSE(!(buf1 != cstr));
    EXPECT_FALSE((buf1 < cstr));
    EXPECT_FALSE((buf1 <= cstr));
    EXPECT_FALSE(!(buf1 > cstr));
    EXPECT_FALSE(!(buf1 >= cstr));
    cstr = __CharP"0123456789";
    EXPECT_FALSE(!(buf1 == cstr));
    EXPECT_FALSE((buf1 != cstr));
    EXPECT_FALSE((buf1 < cstr));
    EXPECT_FALSE(!(buf1 <= cstr));
    EXPECT_FALSE((buf1 > cstr));
    EXPECT_FALSE(!(buf1 >= cstr));
    cstr = __CharP"012345688";
    EXPECT_FALSE((buf1 == cstr));
    EXPECT_FALSE(!(buf1 != cstr));
    EXPECT_FALSE(!(buf1 < cstr));
    EXPECT_FALSE(!(buf1 <= cstr));
    EXPECT_FALSE((buf1 > cstr));
    EXPECT_FALSE((buf1 >= cstr));
    cstr = __CharP"01234567890";
    EXPECT_FALSE((buf1 == cstr));
    EXPECT_FALSE(!(buf1 != cstr));
    EXPECT_FALSE(!(buf1 < cstr));
    EXPECT_FALSE(!(buf1 <= cstr));
    EXPECT_FALSE((buf1 > cstr));
    EXPECT_FALSE((buf1 >= cstr));

    cstr = __CharP"0123456789";
    buf4.assign(__CharP"0123455789ag");
    EXPECT_FALSE((cstr == buf4));
    EXPECT_FALSE(!(cstr != buf4));
    EXPECT_FALSE((cstr < buf4));
    EXPECT_FALSE((cstr <= buf4));
    EXPECT_FALSE(!(cstr > buf4));
    EXPECT_FALSE(!(cstr >= buf4));
    buf4.assign(__CharP"012345678");
    EXPECT_FALSE((cstr == buf4));
    EXPECT_FALSE(!(cstr != buf4));
    EXPECT_FALSE((cstr < buf4));
    EXPECT_FALSE((cstr <= buf4));
    EXPECT_FALSE(!(cstr > buf4));
    EXPECT_FALSE(!(cstr >= buf4));
    buf4.assign(__CharP"0123456789");
    EXPECT_FALSE(!(cstr == buf4));
    EXPECT_FALSE((cstr != buf4));
    EXPECT_FALSE((cstr < buf4));
    EXPECT_FALSE(!(cstr <= buf4));
    EXPECT_FALSE((cstr > buf4));
    EXPECT_FALSE(!(cstr >= buf4));
    buf4.assign(__CharP"012345688");
    EXPECT_FALSE((cstr == buf4));
    EXPECT_FALSE(!(cstr != buf4));
    EXPECT_FALSE(!(cstr < buf4));
    EXPECT_FALSE(!(cstr <= buf4));
    EXPECT_FALSE((cstr > buf4));
    EXPECT_FALSE((cstr >= buf4));
    buf4.assign(__CharP"01234567890");
    EXPECT_FALSE((cstr == buf4));
    EXPECT_FALSE(!(cstr != buf4));
    EXPECT_FALSE(!(cstr < buf4));
    EXPECT_FALSE(!(cstr <= buf4));
    EXPECT_FALSE((cstr > buf4));
    EXPECT_FALSE((cstr >= buf4));
}

/*
int main()
{
    EXPECT_FALSE(!test<char>());
        return 1;
    EXPECT_FALSE(!test<signed char>())
        return 1;
    EXPECT_FALSE(!test<unsigned char>())
        return 1;
    cout<<"CharBuff test succ\n";
    return 0;
}
*/
