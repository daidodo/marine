typedef CCharBuffer<Char> __Buffer;

Char str[100] = {'a','b', 'c', 'd', 'e', 'f', 'g'};
size_t LEN = 7;

TEST(name, ctor)
{
    __Buffer buf1;
    ASSERT_TRUE(buf1.empty());

    __Buffer buf2(str);
    ASSERT_EQ(LEN, buf2.size());

    const __Buffer buf3(str, sizeof str);
    ASSERT_TRUE(buf3.empty());

    const __Buffer buf4(str, sizeof str, LEN);
    ASSERT_EQ(LEN, buf4.size());

    __Buffer buf5 = str;
    ASSERT_EQ(LEN, buf5.size());
}

TEST(name, assign)
{
    __Buffer buf1, buf2;
    ASSERT_TRUE(buf1.empty());
    ASSERT_EQ(size_t(0), buf1.capacity());

    ASSERT_EQ(&buf2, &(buf2.assign(buf1)));
    ASSERT_TRUE(buf2.empty());
    ASSERT_EQ(size_t(0), buf2.capacity());

    ASSERT_EQ(&buf1, &(buf1.assign(str, sizeof str)));
    ASSERT_TRUE(buf1.empty());
    ASSERT_EQ(sizeof str, buf1.capacity());

    ASSERT_EQ(&buf2, &(buf2.assign(buf1)));
    ASSERT_TRUE(buf2.empty());
    ASSERT_EQ(sizeof str, buf2.capacity());

    ASSERT_EQ(&buf1, &(buf1.assign(str, sizeof str, LEN)));
    ASSERT_EQ(LEN, buf1.size());
    ASSERT_EQ(sizeof str, buf1.capacity());

    ASSERT_EQ(&buf2, &(buf2.assign(buf1)));
    ASSERT_EQ(LEN, buf2.size());
    ASSERT_EQ(sizeof str, buf2.capacity());
}

#define __CharP (Char *)

TEST(name, old)
{
    __Buffer buf1;
    __Buffer buf2(str);
    __Buffer buf3(str, sizeof str);
    __Buffer buf4(str, sizeof str, LEN);

    ASSERT_FALSE(buf1 == buf2);
    //ASSERT_FALSE(buf2 != buf3);
    //ASSERT_FALSE(buf3 != buf4);
    ASSERT_FALSE(buf2 != buf4);
    ASSERT_FALSE(!buf1.empty());
    ASSERT_FALSE(7 != buf2.size() || 7 != buf2.capacity());
    ASSERT_FALSE(sizeof str != buf3.capacity());
    //ASSERT_FALSE(7 != buf4.capacity());
    ASSERT_FALSE(7 != buf4.length());
    //assign
    buf1.assign(buf3);
    ASSERT_FALSE(buf1 != buf3);
    ASSERT_FALSE(sizeof str != buf1.capacity());
    buf2.assign(str, sizeof str, 7);
    //ASSERT_FALSE(buf2 != buf3);
    ASSERT_FALSE(sizeof str != buf2.capacity());
    //copy
    Char str2[8] = {};
    ASSERT_FALSE(7 != buf2.copy(str2, 7));
    ASSERT_EQ(0, memcmp(str, str2, sizeof str2));
    //begin, end
    __Buffer::value_type ch = 'A';
    typedef __Buffer::iterator __Iter;
    typedef __Buffer::const_iterator __CIter;
    for(__Iter i = buf1.begin();i != buf1.end();++i)
        *i = ch++;
    ch = 'A';
    for(__CIter i = buf3.begin();i != buf3.end();++i)
        ASSERT_FALSE(*i != ch++);
    //rbegin, rend
    typedef __Buffer::reverse_iterator __RIter;
    typedef __Buffer::const_reverse_iterator __CRIter;
    ch = 'A';
    for(__RIter i = buf1.rbegin();i != buf1.rend();++i)
        *i = ch++;
    ch = 'A';
    for(__CRIter i = buf3.rbegin();i != buf3.rend();++i)
        ASSERT_FALSE(*i != ch++);
    //operator []
    ch = 'A';
    for(size_t i = 0;i < buf1.size();++i)
        buf1[i] = ch++;
    ch = 'A';
    for(size_t i = 0;i < buf3.size();++i)
        ASSERT_FALSE(buf3[i] != ch++);
    //back
    buf1.back() = 'x';
    ASSERT_FALSE(buf3.back() != 'x');
    //at
    ch = 'A';
    for(size_t i = 0;i < buf1.size();++i)
        buf1.at(i) = ch++;
    ch = 'A';
    for(size_t i = 0;i < buf3.size();++i)
        ASSERT_FALSE(buf3.at(i) != ch++);
    //clear, empty
    ASSERT_FALSE(buf2.empty());
    buf2.clear();
    ASSERT_FALSE(!buf2.empty());
    //resize
    buf2.resize(7);
    ASSERT_FALSE(7 != buf2.size());
    //push_back
    buf2.clear();
    ASSERT_FALSE(buf2 == buf4);
    ch = 'a';
    for(int i = 0;i < 7;++i)
        buf2.push_back(ch + i);
    ASSERT_FALSE(buf2 != buf4);
    //swap
    buf4.assign(str2, sizeof str2, sizeof str2);
    buf2.swap(buf4);
    ASSERT_NE(str, &buf2[0]);
    ASSERT_FALSE(str != &buf4[0]);
    swap(buf4, buf2);
    ASSERT_FALSE(str == &buf4[0]);
    ASSERT_FALSE(str != &buf2[0]);
    //append
    buf1.clear();
    for(size_t i = 0;i < 10;++i)
        buf1.append(10, 'd');
    ASSERT_FALSE(100 != buf1.size());
    for(size_t i = 0;i < buf1.size();++i)
        ASSERT_FALSE(buf1[i] != 'd');
    buf1.clear();
    for(size_t i = 0;i < 20;++i)
        buf1.append(__CharP"ssdeb");
    ASSERT_FALSE(100 != buf1.size());
    for(size_t i = 0;i < buf1.size();i += 5)
        ASSERT_FALSE(0 != memcmp(&buf1[i], "ssdeb", 5));
    buf4.assign(__CharP"gadfdengb");
    buf1.clear();
    for(size_t i = 0;i < 20;++i)
        buf1.append(buf4, 4, 5);
    ASSERT_FALSE(100 != buf1.size());
    for(size_t i = 0;i < buf1.size();i += 5)
        ASSERT_FALSE(0 != memcmp(&buf1[i], "dengb", 5));
    buf4.resize(5);
    buf1.clear();
    for(size_t i = 0;i < 20;++i)
        buf1.append(buf4);
    ASSERT_FALSE(100 != buf1.size());
    for(size_t i = 0;i < buf1.size();i += 5)
        ASSERT_FALSE(0 != memcmp(&buf1[i], "gadfd", 5));
    //operator +=
    buf1.clear();
    for(size_t i = 0;i < 20;++i)
        buf1 += __CharP"sgdeb";
    ASSERT_FALSE(100 != buf1.size());
    for(size_t i = 0;i < buf1.size();i += 5)
        ASSERT_FALSE(0 != memcmp(&buf1[i], "sgdeb", 5));
    buf4.assign(__CharP"f90rg");
    buf1.clear();
    for(size_t i = 0;i < 20;++i)
        buf1 += buf4;
    ASSERT_FALSE(100 != buf1.size());
    for(size_t i = 0;i < buf1.size();i += 5)
        ASSERT_FALSE(0 != memcmp(&buf1[i], "f90rg", 5));
    buf1.clear();
    for(int i = 0;i < 100;++i)
        buf1 += 12 + i;
    ASSERT_FALSE(100 != buf1.size());
    for(size_t i = 0;i < buf1.size();++i)
        ASSERT_FALSE(buf1[i] != 12 + int(i))<<"buf1["<<i<<"]="<<int(buf1[i])<<" is not "<<(12 + int(i))<<endl;
    //insert
    buf4.assign(__CharP"3e4r5");
    buf1.clear();
    buf1 += buf4;
    ASSERT_FALSE(5 != buf1.size());
    for(int i = 0;i < 19;++i)
        buf1.insert(i * 5 + 3, 5, 'a');
    ASSERT_FALSE(100 != buf1.size())<<"1: buf1.size()="<<buf1.size()<<" is not 100\n";
    ASSERT_FALSE(0 != memcmp(&buf1[0], &buf4[0], 3))<<"1: buf1[0]="<<&buf1[0]<<", buf4[0]="<<&buf4[0]<<endl;
    for(int i = 3;i < 98;++i)
        ASSERT_FALSE('a' != buf1[i]);
    ASSERT_FALSE(0 != memcmp(&buf1[98], &buf4[3], 2));
    buf1.clear();
    buf1 += buf4;
    for(int i = 0;i < 19;++i)
        buf1.insert(i * 5 + 3, __CharP"3g89a", 5);
    ASSERT_FALSE(100 != buf1.size())<<"2: buf1.size()="<<buf1.size()<<" is not 100\n";
    ASSERT_FALSE(0 != memcmp(&buf1[0], &buf4[0], 3))<<"2: buf1[0]="<<&buf1[0]<<", buf4[0]="<<&buf4[0]<<endl;
    for(int i = 0;i < 19;++i)
        ASSERT_FALSE(0 != memcmp("3g89a", &buf1[i * 5 + 3], 5));
    ASSERT_FALSE(0 != memcmp(&buf1[98], &buf4[3], 2));
    buf1.clear();
    buf1 += buf4;
    for(int i = 0;i < 19;++i)
        buf1.insert(i * 5 + 3, __CharP"3g89a");
    ASSERT_FALSE(100 != buf1.size())<<"3: buf1.size()="<<buf1.size()<<" is not 100\n";
    ASSERT_FALSE(0 != memcmp(&buf1[0], &buf4[0], 3))<<"3: buf1[0]="<<&buf1[0]<<", buf4[0]="<<&buf4[0]<<endl;
    for(int i = 0;i < 19;++i)
        ASSERT_FALSE(0 != memcmp("3g89a", &buf1[i * 5 + 3], 5));
    ASSERT_FALSE(0 != memcmp(&buf1[98], &buf4[3], 2));
    buf4.assign(__CharP"23e32t2g22");
    buf1.clear();
    buf1.append(buf4, 0, 5);
    for(int i = 0;i < 19;++i)
        buf1.insert(i * 5 + 3, buf4, 4, 5);
    ASSERT_FALSE(100 != buf1.size())<<"4: buf1.size()="<<buf1.size()<<" is not 100\n";
    ASSERT_FALSE(0 != memcmp(&buf1[0], &buf4[0], 3))<<"4: buf1[0]="<<&buf1[0]<<", buf4[0]="<<&buf4[0]<<endl;
    for(int i = 0;i < 19;++i)
        ASSERT_FALSE(0 != memcmp(&buf4[4], &buf1[i * 5 + 3], 5));
    ASSERT_FALSE(0 != memcmp(&buf1[98], &buf4[3], 2));
    buf4.resize(5);
    buf1.clear();
    buf1.append(buf4);
    for(int i = 0;i < 19;++i)
        buf1.insert(i * 5 + 3, buf4);
    ASSERT_FALSE(100 != buf1.size())<<"5: buf1.size()="<<buf1.size()<<" is not 100\n";
    ASSERT_FALSE(0 != memcmp(&buf1[0], &buf4[0], 3))<<"5: buf1[0]="<<&buf1[0]<<", buf4[0]="<<&buf4[0]<<endl;
    for(int i = 0;i < 19;++i)
        ASSERT_FALSE(0 != memcmp(&buf4[0], &buf1[i * 5 + 3], 5));
    ASSERT_FALSE(0 != memcmp(&buf1[98], &buf4[3], 2));
    buf1.clear();
    buf1.append(buf4);
    for(int i = 0;i < 95;++i){
        __Iter p = buf1.insert(buf1.begin() + i + 3, 12 + i);
        ASSERT_FALSE(*p != 12 + i);
    }
    ASSERT_FALSE(100 != buf1.size())<<"6: buf1.size()="<<buf1.size()<<" is not 100\n";
    ASSERT_FALSE(0 != memcmp(&buf1[0], &buf4[0], 3))<<"6: buf1[0]="<<&buf1[0]<<", buf4[0]="<<&buf4[0]<<endl;
    for(int i = 0;i < 95;++i)
        ASSERT_FALSE(12 + i != buf1[i + 3]);
    ASSERT_FALSE(0 != memcmp(&buf1[98], &buf4[3], 2));
    //replace
    buf1.clear();
    buf1 += __CharP"0123456789";
    ASSERT_FALSE(10 != buf1.size());
    ASSERT_FALSE(0 != memcmp("0123456789", &buf1[0], buf1.size()));
    buf1.replace(4, 3, 3, '0');
    ASSERT_FALSE(10 != buf1.size());
    ASSERT_FALSE(0 != memcmp("0123000789", &buf1[0], buf1.size()));
    buf1.replace(4, 3, 5, '1');
    ASSERT_FALSE(12 != buf1.size());
    ASSERT_FALSE(0 != memcmp("012311111789", &buf1[0], buf1.size()));
    buf1.replace(4, 5, 3, 'a');
    ASSERT_FALSE(10 != buf1.size());
    ASSERT_FALSE(0 != memcmp("0123aaa789", &buf1[0], buf1.size()));
    buf1.clear();
    buf1 += __CharP"0123456789";
    ASSERT_FALSE(10 != buf1.size());
    ASSERT_FALSE(0 != memcmp("0123456789", &buf1[0], buf1.size()));
    buf1.replace(4, 3, __CharP"000", 3);
    ASSERT_FALSE(10 != buf1.size());
    ASSERT_FALSE(0 != memcmp("0123000789", &buf1[0], buf1.size()));
    buf1.replace(4, 3, __CharP"11111", 5);
    ASSERT_FALSE(12 != buf1.size());
    ASSERT_FALSE(0 != memcmp("012311111789", &buf1[0], buf1.size()));
    buf1.replace(4, 5, __CharP"aaa", 3);
    ASSERT_FALSE(10 != buf1.size());
    ASSERT_FALSE(0 != memcmp("0123aaa789", &buf1[0], buf1.size()));
    buf1.clear();
    buf1 += __CharP"0123456789";
    ASSERT_FALSE(10 != buf1.size());
    ASSERT_FALSE(0 != memcmp("0123456789", &buf1[0], buf1.size()));
    buf1.replace(4, 3, __CharP"000");
    ASSERT_FALSE(10 != buf1.size());
    ASSERT_FALSE(0 != memcmp("0123000789", &buf1[0], buf1.size()));
    buf1.replace(4, 3, __CharP"11111");
    ASSERT_FALSE(12 != buf1.size());
    ASSERT_FALSE(0 != memcmp("012311111789", &buf1[0], buf1.size()));
    buf1.replace(4, 5, __CharP"aaa");
    ASSERT_FALSE(10 != buf1.size());
    ASSERT_FALSE(0 != memcmp("0123aaa789", &buf1[0], buf1.size()));
    buf4.assign(__CharP"00011111aaa");
    buf1.clear();
    buf1 += __CharP"0123456789";
    ASSERT_FALSE(10 != buf1.size());
    ASSERT_FALSE(0 != memcmp("0123456789", &buf1[0], buf1.size()));
    buf1.replace(4, 3, buf4, 0, 3);
    ASSERT_FALSE(10 != buf1.size());
    ASSERT_FALSE(0 != memcmp("0123000789", &buf1[0], buf1.size()));
    buf1.replace(4, 3, buf4, 3, 5);
    ASSERT_FALSE(12 != buf1.size());
    ASSERT_FALSE(0 != memcmp("012311111789", &buf1[0], buf1.size()));
    buf1.replace(4, 5, buf4, 8, 3);
    ASSERT_FALSE(10 != buf1.size());
    ASSERT_FALSE(0 != memcmp("0123aaa789", &buf1[0], buf1.size()));
    buf1.clear();
    buf1 += __CharP"0123456789";
    ASSERT_FALSE(10 != buf1.size());
    ASSERT_FALSE(0 != memcmp("0123456789", &buf1[0], buf1.size()));
    buf4.assign(__CharP"000");
    buf1.replace(4, 3, buf4);
    ASSERT_FALSE(10 != buf1.size());
    ASSERT_FALSE(0 != memcmp("0123000789", &buf1[0], buf1.size()));
    buf4.assign(__CharP"11111");
    buf1.replace(4, 3, buf4);
    ASSERT_FALSE(12 != buf1.size());
    ASSERT_FALSE(0 != memcmp("012311111789", &buf1[0], buf1.size()));
    buf4.assign(__CharP"aaa");
    buf1.replace(4, 5, buf4);
    ASSERT_FALSE(10 != buf1.size());
    ASSERT_FALSE(0 != memcmp("0123aaa789", &buf1[0], buf1.size()));

    buf1.clear();
    buf1 += __CharP"0123456789";
    ASSERT_FALSE(10 != buf1.size());
    ASSERT_FALSE(0 != memcmp("0123456789", &buf1[0], buf1.size()));
    buf1.replace(buf1.begin() + 4, buf1.begin() + 7, 3, '0');
    ASSERT_FALSE(10 != buf1.size());
    ASSERT_FALSE(0 != memcmp("0123000789", &buf1[0], buf1.size()));
    buf1.replace(buf1.begin() + 4, buf1.begin() + 7, 5, '1');
    ASSERT_FALSE(12 != buf1.size());
    ASSERT_FALSE(0 != memcmp("012311111789", &buf1[0], buf1.size()));
    buf1.replace(buf1.begin() + 4, buf1.begin() + 9, 3, 'a');
    ASSERT_FALSE(10 != buf1.size());
    ASSERT_FALSE(0 != memcmp("0123aaa789", &buf1[0], buf1.size()));
    buf1.clear();
    buf1 += __CharP"0123456789";
    ASSERT_FALSE(10 != buf1.size());
    ASSERT_FALSE(0 != memcmp("0123456789", &buf1[0], buf1.size()));
    buf1.replace(buf1.begin() + 4, buf1.begin() + 7, __CharP"000", 3);
    ASSERT_FALSE(10 != buf1.size());
    ASSERT_FALSE(0 != memcmp("0123000789", &buf1[0], buf1.size()));
    buf1.replace(buf1.begin() + 4, buf1.begin() + 7, __CharP"11111", 5);
    ASSERT_FALSE(12 != buf1.size());
    ASSERT_FALSE(0 != memcmp("012311111789", &buf1[0], buf1.size()));
    buf1.replace(buf1.begin() + 4, buf1.begin() + 9, __CharP"aaa", 3);
    ASSERT_FALSE(10 != buf1.size());
    ASSERT_FALSE(0 != memcmp("0123aaa789", &buf1[0], buf1.size()));
    buf1.clear();
    buf1 += __CharP"0123456789";
    ASSERT_FALSE(10 != buf1.size());
    ASSERT_FALSE(0 != memcmp("0123456789", &buf1[0], buf1.size()));
    buf1.replace(buf1.begin() + 4, buf1.begin() + 7, __CharP"000");
    ASSERT_FALSE(10 != buf1.size());
    ASSERT_FALSE(0 != memcmp("0123000789", &buf1[0], buf1.size()));
    buf1.replace(buf1.begin() + 4, buf1.begin() + 7, __CharP"11111");
    ASSERT_FALSE(12 != buf1.size());
    ASSERT_FALSE(0 != memcmp("012311111789", &buf1[0], buf1.size()));
    buf1.replace(buf1.begin() + 4, buf1.begin() + 9, __CharP"aaa");
    ASSERT_FALSE(10 != buf1.size());
    ASSERT_FALSE(0 != memcmp("0123aaa789", &buf1[0], buf1.size()));
    buf4.assign(__CharP"00011111aaa");
    buf1.clear();
    buf1 += __CharP"0123456789";
    ASSERT_FALSE(10 != buf1.size());
    ASSERT_FALSE(0 != memcmp("0123456789", &buf1[0], buf1.size()));
    buf1.replace(buf1.begin() + 4, buf1.begin() + 7, buf4, 0, 3);
    ASSERT_FALSE(10 != buf1.size());
    ASSERT_FALSE(0 != memcmp("0123000789", &buf1[0], buf1.size()));
    buf1.replace(buf1.begin() + 4, buf1.begin() + 7, buf4, 3, 5);
    ASSERT_FALSE(12 != buf1.size());
    ASSERT_FALSE(0 != memcmp("012311111789", &buf1[0], buf1.size()));
    buf1.replace(buf1.begin() + 4, buf1.begin() + 9, buf4, 8, 3);
    ASSERT_FALSE(10 != buf1.size());
    ASSERT_FALSE(0 != memcmp("0123aaa789", &buf1[0], buf1.size()));
    buf1.clear();
    buf1 += __CharP"0123456789";
    ASSERT_FALSE(10 != buf1.size());
    ASSERT_FALSE(0 != memcmp("0123456789", &buf1[0], buf1.size()));
    buf4.assign(__CharP"000");
    buf1.replace(buf1.begin() + 4, buf1.begin() + 7, buf4);
    ASSERT_FALSE(10 != buf1.size());
    ASSERT_FALSE(0 != memcmp("0123000789", &buf1[0], buf1.size()));
    buf4.assign(__CharP"11111");
    buf1.replace(buf1.begin() + 4, buf1.begin() + 7, buf4);
    ASSERT_FALSE(12 != buf1.size());
    ASSERT_FALSE(0 != memcmp("012311111789", &buf1[0], buf1.size()));
    buf4.assign(__CharP"aaa");
    buf1.replace(buf1.begin() + 4, buf1.begin() + 9, buf4);
    ASSERT_FALSE(10 != buf1.size());
    ASSERT_FALSE(0 != memcmp("0123aaa789", &buf1[0], buf1.size()));
    //erase
    buf1.clear();
    buf1 += __CharP"0123456789";
    ASSERT_FALSE(10 != buf1.size());
    ASSERT_FALSE(0 != memcmp("0123456789", &buf1[0], buf1.size()));
    __Iter p = buf1.erase(buf1.begin() + 1, buf1.end() - 2);
    ASSERT_FALSE(*p != '8');
    ASSERT_FALSE(3 != buf1.size());
    ASSERT_FALSE(0 != memcmp("089", &buf1[0], buf1.size()));
    buf1.clear();
    buf1 += __CharP"0123456789";
    ASSERT_FALSE(10 != buf1.size());
    ASSERT_FALSE(0 != memcmp("0123456789", &buf1[0], buf1.size()));
    p = buf1.erase(buf1.begin() + 5);
    ASSERT_FALSE(*p != '6');
    ASSERT_FALSE(9 != buf1.size());
    ASSERT_FALSE(0 != memcmp("012346789", &buf1[0], buf1.size()));
    buf1.clear();
    buf1 += __CharP"0123456789";
    ASSERT_FALSE(10 != buf1.size());
    ASSERT_FALSE(0 != memcmp("0123456789", &buf1[0], buf1.size()));
    buf1.erase(3, 4);
    ASSERT_FALSE(6 != buf1.size());
    ASSERT_FALSE(0 != memcmp("012789", &buf1[0], buf1.size()));
    buf1.clear();
    buf1 += __CharP"0123456789";
    ASSERT_FALSE(10 != buf1.size());
    ASSERT_FALSE(0 != memcmp("0123456789", &buf1[0], buf1.size()));
    buf1.erase(6);
    ASSERT_FALSE(6 != buf1.size())<<"buf1.size()="<<buf1.size()<<" is not 6\n";
    ASSERT_FALSE(0 != memcmp("012345", &buf1[0], buf1.size()));
    //substr
    buf1.clear();
    buf1 += __CharP"0123456789";
    ASSERT_FALSE(10 != buf1.size());
    ASSERT_FALSE(0 != memcmp("0123456789", &buf1[0], buf1.size()));
    buf4 = buf1.substr(2, 7);
    ASSERT_FALSE(7 != buf4.size());
    ASSERT_FALSE(0 != memcmp("2345678", &buf4[0], buf4.size()));
    //compare
    buf1.clear();
    buf1 += __CharP"0123456789";
    ASSERT_FALSE(10 != buf1.size());
    ASSERT_TRUE(0 > buf1.compare(2, 5, __CharP"256444", 5));
    ASSERT_TRUE(0 < buf1.compare(2, 5, __CharP"226444", 5));
    ASSERT_TRUE(0 == buf1.compare(2, 5, __CharP"23456444", 5));
    ASSERT_TRUE(0 > buf1.compare(2, 5, __CharP"256444"));
    ASSERT_TRUE(0 < buf1.compare(2, 5, __CharP"226444"));
    ASSERT_TRUE(0 == buf1.compare(2, 5, __CharP"23456"));
    ASSERT_TRUE(0 < buf1.compare(__CharP"0123425"));
    ASSERT_TRUE(0 > buf1.compare(__CharP"0124425"));
    ASSERT_TRUE(0 == buf1.compare(__CharP"0123456789"));
    buf4.assign(__CharP"238623456444");
    ASSERT_TRUE(0 < buf1.compare(2, 6, buf4, 4, 8));
    ASSERT_TRUE(0 > buf1.compare(2, 5, buf4, 4, 8));
    ASSERT_TRUE(0 == buf1.compare(2, 5, buf4, 4, 5));
    buf4.assign(__CharP"23456");
    ASSERT_TRUE(0 > buf1.compare(2, 4, buf4));
    ASSERT_TRUE(0 < buf1.compare(2, 6, buf4));
    ASSERT_TRUE(0 == buf1.compare(2, 5, buf4));
    buf4.assign(__CharP"2623424");
    ASSERT_TRUE(0 > buf1.compare(buf4));
    ASSERT_FALSE(buf1 == buf4);
    buf4.assign(__CharP"01223434");
    ASSERT_TRUE(0 < buf1.compare(buf4));
    ASSERT_FALSE(buf1 == buf4);
    buf4.assign(__CharP"0123456789");
    ASSERT_TRUE(0 == buf1.compare(buf4));
    ASSERT_FALSE(buf1 != buf4);
    //operators ==, !=, <, <=, >, >=
    buf4.assign(__CharP"0123455789ag");
    ASSERT_FALSE((buf1 == buf4));
    ASSERT_FALSE(!(buf1 != buf4));
    ASSERT_FALSE((buf1 < buf4));
    ASSERT_FALSE((buf1 <= buf4));
    ASSERT_FALSE(!(buf1 > buf4));
    ASSERT_FALSE(!(buf1 >= buf4));
    buf4.assign(__CharP"012345678");
    ASSERT_FALSE((buf1 == buf4));
    ASSERT_FALSE(!(buf1 != buf4));
    ASSERT_FALSE((buf1 < buf4));
    ASSERT_FALSE((buf1 <= buf4));
    ASSERT_FALSE(!(buf1 > buf4));
    ASSERT_FALSE(!(buf1 >= buf4));
    buf4.assign(__CharP"0123456789");
    ASSERT_FALSE(!(buf1 == buf4));
    ASSERT_FALSE((buf1 != buf4));
    ASSERT_FALSE((buf1 < buf4));
    ASSERT_FALSE(!(buf1 <= buf4));
    ASSERT_FALSE((buf1 > buf4));
    ASSERT_FALSE(!(buf1 >= buf4));
    buf4.assign(__CharP"012345688");
    ASSERT_FALSE((buf1 == buf4));
    ASSERT_FALSE(!(buf1 != buf4));
    ASSERT_FALSE(!(buf1 < buf4));
    ASSERT_FALSE(!(buf1 <= buf4));
    ASSERT_FALSE((buf1 > buf4));
    ASSERT_FALSE((buf1 >= buf4));
    buf4.assign(__CharP"01234567890");
    ASSERT_FALSE((buf1 == buf4));
    ASSERT_FALSE(!(buf1 != buf4));
    ASSERT_FALSE(!(buf1 < buf4));
    ASSERT_FALSE(!(buf1 <= buf4));
    ASSERT_FALSE((buf1 > buf4));
    ASSERT_FALSE((buf1 >= buf4));

    const Char * cstr = __CharP"0123455789ag";
    ASSERT_FALSE((buf1 == cstr));
    ASSERT_FALSE(!(buf1 != cstr));
    ASSERT_FALSE((buf1 < cstr));
    ASSERT_FALSE((buf1 <= cstr));
    ASSERT_FALSE(!(buf1 > cstr));
    ASSERT_FALSE(!(buf1 >= cstr));
    cstr = __CharP"012345678";
    ASSERT_FALSE((buf1 == cstr));
    ASSERT_FALSE(!(buf1 != cstr));
    ASSERT_FALSE((buf1 < cstr));
    ASSERT_FALSE((buf1 <= cstr));
    ASSERT_FALSE(!(buf1 > cstr));
    ASSERT_FALSE(!(buf1 >= cstr));
    cstr = __CharP"0123456789";
    ASSERT_FALSE(!(buf1 == cstr));
    ASSERT_FALSE((buf1 != cstr));
    ASSERT_FALSE((buf1 < cstr));
    ASSERT_FALSE(!(buf1 <= cstr));
    ASSERT_FALSE((buf1 > cstr));
    ASSERT_FALSE(!(buf1 >= cstr));
    cstr = __CharP"012345688";
    ASSERT_FALSE((buf1 == cstr));
    ASSERT_FALSE(!(buf1 != cstr));
    ASSERT_FALSE(!(buf1 < cstr));
    ASSERT_FALSE(!(buf1 <= cstr));
    ASSERT_FALSE((buf1 > cstr));
    ASSERT_FALSE((buf1 >= cstr));
    cstr = __CharP"01234567890";
    ASSERT_FALSE((buf1 == cstr));
    ASSERT_FALSE(!(buf1 != cstr));
    ASSERT_FALSE(!(buf1 < cstr));
    ASSERT_FALSE(!(buf1 <= cstr));
    ASSERT_FALSE((buf1 > cstr));
    ASSERT_FALSE((buf1 >= cstr));

    cstr = __CharP"0123456789";
    buf4.assign(__CharP"0123455789ag");
    ASSERT_FALSE((cstr == buf4));
    ASSERT_FALSE(!(cstr != buf4));
    ASSERT_FALSE((cstr < buf4));
    ASSERT_FALSE((cstr <= buf4));
    ASSERT_FALSE(!(cstr > buf4));
    ASSERT_FALSE(!(cstr >= buf4));
    buf4.assign(__CharP"012345678");
    ASSERT_FALSE((cstr == buf4));
    ASSERT_FALSE(!(cstr != buf4));
    ASSERT_FALSE((cstr < buf4));
    ASSERT_FALSE((cstr <= buf4));
    ASSERT_FALSE(!(cstr > buf4));
    ASSERT_FALSE(!(cstr >= buf4));
    buf4.assign(__CharP"0123456789");
    ASSERT_FALSE(!(cstr == buf4));
    ASSERT_FALSE((cstr != buf4));
    ASSERT_FALSE((cstr < buf4));
    ASSERT_FALSE(!(cstr <= buf4));
    ASSERT_FALSE((cstr > buf4));
    ASSERT_FALSE(!(cstr >= buf4));
    buf4.assign(__CharP"012345688");
    ASSERT_FALSE((cstr == buf4));
    ASSERT_FALSE(!(cstr != buf4));
    ASSERT_FALSE(!(cstr < buf4));
    ASSERT_FALSE(!(cstr <= buf4));
    ASSERT_FALSE((cstr > buf4));
    ASSERT_FALSE((cstr >= buf4));
    buf4.assign(__CharP"01234567890");
    ASSERT_FALSE((cstr == buf4));
    ASSERT_FALSE(!(cstr != buf4));
    ASSERT_FALSE(!(cstr < buf4));
    ASSERT_FALSE(!(cstr <= buf4));
    ASSERT_FALSE((cstr > buf4));
    ASSERT_FALSE((cstr >= buf4));
}
