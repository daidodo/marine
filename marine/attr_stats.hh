#ifndef DOZERG_ATTR_STATS_H_20130606
#define DOZERG_ATTR_STATS_H_20130606

/*
    对程序进行属性统计
        ATTR_INIT       初始化属性统计
        ATTR_ADD        增加属性统计值
        ATTR_ADD_SLOW   增加属性统计值
                        如果使用ATTR_ADD编译不过，请使用此函数
        ATTR_SET        修改属性统计值
        ATTR_SET_EX     修改属性统计值，并返回旧统计值
        ATTR_SET_SLOW   修改属性统计值，并返回旧统计值
                        如果使用ATTR_SET/ATTR_SET_EX编译不过，请使用此函数
        ATTR_ITERATE    遍历所有属性，进行操作，并清0统计值
//*/

#include "impl/attr_stats_impl.hh"

NS_SERVER_BEGIN

//初始化Attr属性统计
//capacity: 最多属性个数
inline bool ATTR_INIT(size_t capacity = 1000)
{
    return NS_IMPL::CAttrStats::Inst().init(capacity);
}

//增加指定属性的值
//int attr: 属性ID(>0)
//uint64_t val: 增加的值
#define ATTR_ADD(attr, val) \
    do{ \
        __UNUSED typedef NS_SERVER::NS_IMPL::__AttrDummy<attr> attr_must_be_constant;    \
        static uint64_t * __Attr = NULL;    \
        if(!__Attr){    \
            __Attr = NS_SERVER::NS_IMPL::CAttrStats::Inst().attrAdd(attr, val);    \
        }else   \
            NS_SERVER::NS_IMPL::CAttrStats::attrAdd(__Attr, val);   \
    }while(0)

inline bool ATTR_ADD_SLOW(int attr, uint64_t val)
{
    return (NULL != NS_IMPL::CAttrStats::Inst().attrAdd(attr, val));
}

//修改指定属性的值
//int attr: 属性ID(>0)
//uint64_t val: 修改后的属性值
#define ATTR_SET(attr, val) \
    do{ \
        __UNUSED typedef NS_SERVER::NS_IMPL::__AttrDummy<attr> attr_must_be_constant;    \
        static uint64_t * __Attr = NULL;    \
        if(!__Attr){    \
            __Attr = NS_SERVER::NS_IMPL::CAttrStats::Inst().attrSet(attr, val);    \
        }else   \
            NS_SERVER::NS_IMPL::CAttrStats::attrSet(__Attr, val);   \
    }while(0)

//修改指定属性的值，并返回之前的值
//int attr: 属性ID(>0)
//uint64_t val: 修改后的属性值
//uint64_t * old:
//  NULL    忽略
//  其他    返回修改前的属性值
#define ATTR_SET_EX(attr, val, old) \
    do{ \
        __UNUSED typedef NS_SERVER::NS_IMPL::__AttrDummy<attr> attr_must_be_constant;    \
        static uint64_t * __Attr = NULL;    \
        if(!__Attr){    \
            __Attr = NS_SERVER::NS_IMPL::CAttrStats::Inst().attrSet(attr, val, old);    \
        }else   \
            NS_SERVER::NS_IMPL::CAttrStats::attrSet(__Attr, val, old);   \
    }while(0)

inline bool ATTR_SET_SLOW(int attr, uint64_t val, uint64_t * old = NULL)
{
    return (NULL != NS_IMPL::CAttrStats::Inst().attrSet(attr, val, old));
}

//遍历所有attr，对每个[attr, value]执行op操作，并将value清0
//op: 要求实现
//      void operator ()(int attr, uint64_t value) const;
//      其中：
//          attr: 属性ID
//          value: 属性值
template<class Op>
inline void ATTR_ITERATE(Op op)
{
    return NS_IMPL::CAttrStats::Inst().iterate(op);
}

NS_SERVER_END

#endif

