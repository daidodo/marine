#include <marine/single_list.hh>

#include "../inc.h"

typedef CSingleList<int>    __List;

static bool compareList(const __List & list1, const __List & list2)
{
    __List::const_iterator cit = list2.begin();
    if(*cit != 1){
        cerr<<"list2.begin()="<<*cit<<" is not 1\n";
        return false;
    }
    ++cit;
    for(__List::const_iterator i = list1.begin();i != list1.end();++i, ++cit)
        if(*i != *cit){
            cerr<<"list1 item="<<*i<<" is not equal list2 item="<<*cit<<endl;
            return false;
        }
    for(int i = 0;i < 10;++i, ++cit)
        if(*cit != 3){
            cerr<<"2: list2 item="<<*cit<<" is not 3\n";
            return false;
        }
    if(*cit != 2){
        cerr<<"++list2.begin()="<<*cit<<" is not 2\n";
        return false;
    }
    ++cit;
    if(cit != list2.end()){
        cerr<<"cit != list2.end()\n";
        return false;
    }
    return true;
}

static bool testSingleList()
{
    __List list1, list2(100, 1);
    __List list3(list2), list4(list3.begin(), list3.end());
    if(!list1.empty()){
        cerr<<"list1.empty() is false\n";
        return false;
    }
    if(list2.empty()){
        cerr<<"list2.empty() is true\n";
        return false;
    }
    if(list3.size() != 100){
        cerr<<"list3.size()="<<list3.size()<<" is not 100\n";
        return false;
    }
    int sum = 0;
    for(__List::iterator i = list4.begin();i != list4.end();++i){
        sum += *i;
        *i = sum;
    }
    sum = 1;
    for(__List::const_iterator i = list4.begin();i != list4.end();++i, ++sum)
        if(*i != sum){
            cerr<<"list4 item="<<*i<<" is not "<<sum<<endl;
            return false;
        }
    list2.front() = 100;
    list2.back() = 200;
    const __List & list_ref = list2;
    if(100 != list_ref.front() ||
            200 != list_ref.back()){
        cerr<<"list_ref.front()="<<list_ref.front()<<", list_ref.back()="<<list_ref.back()<<endl;
        return false;
    }
    list2.clear();
    if(!list2.empty()){
        cerr<<"list2.empty() is false after clear()\n";
        return false;
    }
    for(int i = 0;i < 100;++i)
        list2.push_front(i);
    sum = 99;
    for(__List::const_iterator i = list2.begin();i != list2.end();++i, --sum)
        if(*i != sum){
            cerr<<"1: list2 item="<<*i<<" is not sum="<<sum<<endl;
            return false;
        }
    list3.clear();
    if(!list3.empty()){
        cerr<<"list3.empty() is false after clear()\n";
        return false;
    }
    for(int i = 0;i < 100;++i)
        list3.push_back(i);
    sum = 0;
    for(__List::const_iterator i = list3.begin();i != list3.end();++i, ++sum)
        if(*i != sum){
            cerr<<"1: list3 item="<<*i<<" is not sum="<<sum<<endl;
            return false;
        }
    for(int i = 0;i < 30;++i){
        if(i != list3.front()){
            cerr<<"1: list3.front()="<<list3.front()<<" is not "<<i<<endl;
            return false;
        }
        list3.pop_front();
    }
    if(30 != list3.front()){
        cerr<<"2: list3.front()="<<list3.front()<<" is not 30\n";
        return false;
    }
    for(int i = 0;i < 10;++i){
        __List::iterator it = list3.begin();
        if(30 != *it){
            cerr<<"1: list3.begin()="<<*it<<" is not 30\n";
            return false;
        }
        ++it;
        if(31 + i != *it){
            cerr<<"list3 item="<<*it<<" is not (31 + i)="<<(31 + i)<<endl;
            return false;
        }
        list3.erase_after(list3.begin());
    }
    __List::iterator it = list3.begin();
    if(30 != *it){
        cerr<<"2: list3.begin()="<<*it<<" is not 30\n";
        return false;
    }
    for(int i = 0;i < 10;++i)
        ++it;
    list3.erase_after(list3.begin(), it);
    it = list3.begin();
    ++it;
    for(int i = 50;i < 60;++i, ++it)
        if(i != *it){
            cerr<<"2: list3 item="<<*it<<" is not "<<i<<endl;
            return false;
        }
    list2.clear();
    list2.push_front(1);
    list2.insert_after(list2.begin(), 2);
    list2.insert_after(list2.begin(), 10, 3);
    list2.insert_after(list2.begin(), list3.begin(), list3.end());
    if(!compareList(list3, list2)){
        cerr<<"compareList(list3, list2) failed\n";
        return false;
    }
    list1.clear();
    list1.assign(10, 1);
    sum = 0;
    for(__List::const_iterator i = list1.begin();i != list1.end();++i, ++sum)
        if(1 != *i){
            cerr<<"1: list1 item="<<*i<<" is not 1\n";
            return false;
        }
    if(10 != sum){
        cerr<<"sum="<<sum<<" is not 10\n";
        return false;
    }
    list1.resize(20, 2);
    sum = 0;
    for(__List::const_iterator i = list1.begin();i != list1.end();++i, ++sum){
        if(sum < 10){
            if(1 != *i){
                cerr<<"2: list1 item["<<sum<<"]="<<*i<<" is not 1\n";
                return false;
            }
        }else{
            if(2 != *i){
                cerr<<"3: list1 item["<<sum<<"]="<<*i<<" is not 2\n";
                return false;
            }
        }
    }
    list1.assign(list2.begin(), list2.end());
    if(!compareList(list3, list1)){
        cerr<<"compareList(list3, list1) failed\n";
        return false;
    }
    if(list1 != list2){
        cerr<<"list1 != list2\n";
        return false;
    }
    list1.resize(list1.size() - 1);
    if(!(list1 < list2)){
        cerr<<"list1 < list2 is false\n";
        return false;
    }
    swap(list1, list2);
    if(!(list1 > list2)){
        cerr<<"list1 > list2 is false\n";
        return false;
    }
    return true;
}

TEST(CSingleList, all)
{
    EXPECT_TRUE(testSingleList());
}
