#ifndef DOZERG_TOOLS_MEMORY_H_20130121
#define DOZERG_TOOLS_MEMORY_H_20130121

#include "../impl/tools_memory_impl.hh"

NS_SERVER_BEGIN

namespace tools
{
    //��װ�ڴ�����
    template<class T, class A>
    T * Allocate(A a, size_t sz)
    {
        return NS_IMPL::__Allocate<T>(a, sz);
    }

    template<class T, class A>
    T * Allocate(A a)
    {
        typedef typename CTypeTraits<A>::__IsInteger __Tag;
        return NS_IMPL::__Allocate<T>(a, __Tag());
    }

    template<class T>
    T * Allocate(size_t sz)
    {
        return Allocate<T>(std::allocator<T>(), sz);
    }

    template<class T>
    T * Allocate()
    {
        return Allocate<T>(std::allocator<T>(), 1);
    }

    //��װ�ڴ����
    //��Ҫ�����ǰ�p����Ϊ0
    template<class T, class A>
    void Deallocate(T *& p, size_t sz, A a) throw()
    {
        NS_IMPL::__Deallocate(p, sz, a);
    }

    template<class T, class A>
    void Deallocate(T *& p, A a) throw()
    {
        typedef typename CTypeTraits<A>::__IsInteger __Tag;
        NS_IMPL::__Deallocate(p, a, __Tag());
    }

    template<class T>
    void Deallocate(T *& p, size_t sz) throw()
    {
        Deallocate(p, sz, std::allocator<T>());
    }

    template<class T>
    void Deallocate(T *& p) throw()
    {
        Deallocate(p, 1, std::allocator<T>());
    }

    //��װ���캯��
    template<class T>
    T * Construct(T * p)
    {
        return NS_IMPL::__Construct(p);
    }

    template<class T, class V>
    T * Construct(T * p, const V & v)
    {
        return NS_IMPL::__Construct(p, v);
    }

    template<class T, class V1, class V2>
    T * Construct(T * p, const V1 & v1, const V2 & v2)
    {
        return NS_IMPL::__Construct(p, v1, v2);
    }

    //��װ��������
    template<class T>
    void Destroy(T * p) throw()
    {
        NS_IMPL::__Destroy(p);
    }

    //��װnew, �޲���
    template<class T, class A>
    T * New(A a, size_t sz)
    {
        return NS_IMPL::__New<T>(a, sz);
    }

    template<class T, class A>
    T * New(A a)
    {
        typedef typename CTypeTraits<A>::__IsInteger __Tag;
        return NS_IMPL::__New<T>(a, __Tag());
    }

    template<class T>
    T * New(size_t sz)
    {
        return New<T>(std::allocator<T>(), sz);
    }

    template<class T>
    T * New()
    {
        return New<T>(std::allocator<T>(), 1);
    }

    //��װnew, 1������
    template<class T, class V, class A>
    T * New1(const V & v, A a, size_t sz)
    {
        return NS_IMPL::__New1<T>(v, a, sz);
    }

    template<class T, class V, class A>
    T * New1(const V & v, A a)
    {
        typedef typename CTypeTraits<A>::__IsInteger __Tag;
        return NS_IMPL::__New1<T>(v, a, __Tag());
    }

    template<class T, class V>
    T * New1(const V & v, size_t sz)
    {
        return New1<T>(v, std::allocator<T>(), sz);
    }

    template<class T, class V>
    T * New1(const V & v)
    {
        return New1<T>(v, std::allocator<T>(), 1);
    }

    //��װnew, 2������
    template<class T, class V1, class V2, class A>
    T * New2(const V1 & v1, const V2 & v2, A a, size_t sz)
    {
        return NS_IMPL::__New2<T>(v1, v2, a, sz);
    }

    template<class T, class V1, class V2, class A>
    T * New2(const V1 & v1, const V2 & v2, A a)
    {
        typedef typename CTypeTraits<A>::__IsInteger __Tag;
        return NS_IMPL::__New2<T>(v1, v2, a, __Tag());
    }

    template<class T, class V1, class V2>
    T * New2(const V1 & v1, const V2 & v2, size_t sz)
    {
        return New2<T>(v1, v2, std::allocator<T>(), sz);
    }

    template<class T, class V1, class V2>
    T * New2(const V1 & v1, const V2 & v2)
    {
        return New2<T>(v1, v2, std::allocator<T>(), 1);
    }

    //��װdelete������ʹ��allocator
    template<class T, class A>
    void Delete(T *& p, size_t sz, A a) throw()
    {
        NS_IMPL::__Delete(p, sz, a);
    }

    template<class T, class A>
    void Delete(T *& p, A a) throw()
    {
        typedef typename CTypeTraits<A>::__IsInteger __Tag;
        NS_IMPL::__Delete(p, a, __Tag());
    }

    template<class T>
    void Delete(T *& p, size_t sz) throw()
    {
        Delete(p, sz, std::allocator<T>());
    }

    template<class T>
    void Delete(T *& p) throw()
    {
        Delete(p, 1, std::allocator<T>());
    }

}   //namespace tools

NS_SERVER_END

#endif

