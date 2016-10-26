#ifndef DOZERG_ATTR_STATS_H_20130606
#define DOZERG_ATTR_STATS_H_20130606

/*
    �Գ����������ͳ��
        ATTR_INIT       ��ʼ������ͳ��
        ATTR_ADD        ��������ͳ��ֵ
        ATTR_ADD_SLOW   ��������ͳ��ֵ
                        ���ʹ��ATTR_ADD���벻������ʹ�ô˺���
        ATTR_SET        �޸�����ͳ��ֵ
        ATTR_SET_EX     �޸�����ͳ��ֵ�������ؾ�ͳ��ֵ
        ATTR_SET_SLOW   �޸�����ͳ��ֵ�������ؾ�ͳ��ֵ
                        ���ʹ��ATTR_SET/ATTR_SET_EX���벻������ʹ�ô˺���
        ATTR_ITERATE    �����������ԣ����в���������0ͳ��ֵ
//*/

#include "impl/attr_stats_impl.hh"

NS_SERVER_BEGIN

//��ʼ��Attr����ͳ��
//capacity: ������Ը���
inline bool ATTR_INIT(size_t capacity = 1000)
{
    return NS_IMPL::CAttrStats::Inst().init(capacity);
}

//����ָ�����Ե�ֵ
//int attr: ����ID(>0)
//uint64_t val: ���ӵ�ֵ
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

//�޸�ָ�����Ե�ֵ
//int attr: ����ID(>0)
//uint64_t val: �޸ĺ������ֵ
#define ATTR_SET(attr, val) \
    do{ \
        __UNUSED typedef NS_SERVER::NS_IMPL::__AttrDummy<attr> attr_must_be_constant;    \
        static uint64_t * __Attr = NULL;    \
        if(!__Attr){    \
            __Attr = NS_SERVER::NS_IMPL::CAttrStats::Inst().attrSet(attr, val);    \
        }else   \
            NS_SERVER::NS_IMPL::CAttrStats::attrSet(__Attr, val);   \
    }while(0)

//�޸�ָ�����Ե�ֵ��������֮ǰ��ֵ
//int attr: ����ID(>0)
//uint64_t val: �޸ĺ������ֵ
//uint64_t * old:
//  NULL    ����
//  ����    �����޸�ǰ������ֵ
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

//��������attr����ÿ��[attr, value]ִ��op����������value��0
//op: Ҫ��ʵ��
//      void operator ()(int attr, uint64_t value) const;
//      ���У�
//          attr: ����ID
//          value: ����ֵ
template<class Op>
inline void ATTR_ITERATE(Op op)
{
    return NS_IMPL::CAttrStats::Inst().iterate(op);
}

NS_SERVER_END

#endif

