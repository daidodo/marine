#ifndef DOZERG_ITERATOR_ADAPTER_H_20081013
#define DOZERG_ITERATOR_ADAPTER_H_20081013

/*
    提供迭代器的适配器
    可以通过自定义的函数（仿函数）改变对迭代器的访问行为
        const_iter_adapt        通过仿函数自定义访问const迭代器的行为（只读）
        iter_adapt              通过仿函数自定义访问非const迭代器的行为（可读写）
        const_iter_adapt_fun    通过函数自定义访问const迭代器的行为（只读）
        iter_adapt_fun          通过函数自定义访问非const迭代器的行为（可读写）

    使用示例:
        template<class Iter>
        void checkInt(Iter first, Iter last){    //对给定的序列进行操作
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
            //...   //向vecA中加入了一些元素
            //如果我们要调用checkInt()处理vecA中每个元素的v1成员，怎么办？
            //只要定义了函数ExtV1()，我们就可以实现如下：
            checkInt(const_iter_adapt_fun<int>(vecA.begin(), ExtV1),
                     const_iter_adapt_fun<int>(vecA.end(), ExtV1));
        }
        //由于v1在函数checkInt()里没有改变, 所以使用const_iter_adapt_fun
        //否则应该使用iter_adapt_fun
        //类型int必须提供给const_iter_adapt_fun和iter_adapt_fun，因为适配器需要知道返回什么类型给checkInt()函数
        //ExtV1()的返回值必须是引用，const或非const都行，根据实际需要定义

        //如果定义一个仿函数，完成和ExtV1一样的功能
        //那么就可以使用const_iter_adapt和iter_adapt
        //例如：
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
        //result_type是仿函数必须提供的一个类型定义，表示返回给checkInt()函数的类型
        //ExtV1Func::operator ()的返回值必须是引用，const或非const都行，根据实际需要定义
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
