#ifndef DOZERG_ITERATOR_ADAPTER_H_20081013
#define DOZERG_ITERATOR_ADAPTER_H_20081013

/*
    �ṩ��������������
    ����ͨ���Զ���ĺ������º������ı�Ե������ķ�����Ϊ
        const_iter_adapt        ͨ���º����Զ������const����������Ϊ��ֻ����
        iter_adapt              ͨ���º����Զ�����ʷ�const����������Ϊ���ɶ�д��
        const_iter_adapt_fun    ͨ�������Զ������const����������Ϊ��ֻ����
        iter_adapt_fun          ͨ�������Զ�����ʷ�const����������Ϊ���ɶ�д��

    ʹ��ʾ��:
        template<class Iter>
        void checkInt(Iter first, Iter last){    //�Ը��������н��в���
            for(;first != last;++first)
                cout<<*first<<endl;
        }
        struct A{
            int v1;
            string v2;
        };
        const int & ExtV1(const A & a){
            return a.v1;
        }
        int main(){
            vector<A> vecA;
            //...   //��vecA�м�����һЩԪ��
            //�������Ҫ����checkInt()����vecA��ÿ��Ԫ�ص�v1��Ա����ô�죿
            //ֻҪ�����˺���ExtV1()�����ǾͿ���ʵ�����£�
            checkInt(const_iter_adapt_fun<int>(vecA.begin(), ExtV1),
                     const_iter_adapt_fun<int>(vecA.end(), ExtV1));
        }
        //����v1�ں���checkInt()��û�иı�, ����ʹ��const_iter_adapt_fun
        //����Ӧ��ʹ��iter_adapt_fun
        //����int�����ṩ��const_iter_adapt_fun��iter_adapt_fun����Ϊ��������Ҫ֪������ʲô���͸�checkInt()����
        //ExtV1()�ķ���ֵ���������ã�const���const���У�����ʵ����Ҫ����

        //�������һ���º�������ɺ�ExtV1һ���Ĺ���
        //��ô�Ϳ���ʹ��const_iter_adapt��iter_adapt
        //���磺
        struct ExtV1Func{
            typedef A argument_type;
            typedef int result_type;
            const result_type & operator ()(const argument_type & a) const{
                return a.v1;
            }
        };
        int main(){
            vector<A> vecA;
            //...
            checkInt(const_iter_adapt(vecA.begin(), ExtV1Func()),
                     const_iter_adapt(vecA.end(), ExtV1Func()));
        }
        //result_type�Ƿº��������ṩ��һ�����Ͷ��壬��ʾ���ظ�checkInt()����������
        //ExtV1Func::operator ()�ķ���ֵ���������ã�const���const���У�����ʵ����Ҫ����
//*/

#include "impl/iter_adapter_impl.hh"

NS_SERVER_BEGIN

//const_iter_adapt
template<class Ext, class ConstIt>
inline NS_IMPL::CConstIterAdapter<ConstIt, Ext> const_iter_adapt(ConstIt it, Ext ext)
{
    return NS_IMPL::CConstIterAdapter<ConstIt, Ext>(it, ext);
}

template<class Ext, class ConstIt>
inline NS_IMPL::CConstIterAdapter<ConstIt, Ext> const_iter_adapt(ConstIt it)
{
    return NS_IMPL::CConstIterAdapter<ConstIt, Ext>(it, Ext());
}

//iter_adapt
template<class Ext, class It>
inline NS_IMPL::CIterAdapter<It, Ext> iter_adapt(It it, Ext ext)
{
    return NS_IMPL::CIterAdapter<It, Ext>(it, ext);
}

template<class Ext, class It>
inline NS_IMPL::CIterAdapter<It, Ext> iter_adapt(It it)
{
    return NS_IMPL::CIterAdapter<It, Ext>(it, Ext());
}

//const_iter_adapt_fun
template<class T, class ConstIt, class Func>
inline NS_IMPL::CConstIterAdapter<ConstIt, NS_IMPL::CIterAdapterFunctor<ConstIt, T, Func> > const_iter_adapt_fun(ConstIt it, Func func)
{
    typedef NS_IMPL::CIterAdapterFunctor<ConstIt, T, Func> __Ext;
    return NS_IMPL::CConstIterAdapter<ConstIt, __Ext>(it, __Ext(func));
}

//iter_adapt_fun
template<class T, class It, class Func>
inline NS_IMPL::CIterAdapter<It, NS_IMPL::CIterAdapterFunctor<It, T, Func> > iter_adapt_fun(It it, Func func)
{
    typedef NS_IMPL::CIterAdapterFunctor<It, T, Func> __Ext;
    return NS_IMPL::CIterAdapter<It, __Ext>(it, __Ext(func));
}

NS_SERVER_END

#endif
