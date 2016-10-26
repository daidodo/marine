#ifndef DOZERG_LOCKED_OBJECT_H_20131016
#define DOZERG_LOCKED_OBJECT_H_20131016

#include "mutex.hh"

//TODO: unit test

NS_SERVER_BEGIN

CLockObj<A> a;
a.obj().fun();

template<class T, class LockT = CMutex>
class CLockObj
{
    //typedefs
    typedef CLockObj<T, LockT>  __Myt;
public:
    typedef LockT               lock_type;
    typedef CGuard<lock_type>   guard_type;
    typedef T                   value_type;
    typedef value_type &        reference;
    typedef const value_type &  const_reference;
    typedef value_type *        pointer;
    typedef const value_type *  const_pointer;
public:
    //functions
    CLockObj(){}
    reference obj(){return lockObj(obj_, lock_);}
    const_reference cobj() const{lockObj(obj_, lock_);}
    lock_type & locker(){return lock_;}
    //unsafe
    reference unsafe_obj(){return obj_;}
    const_reference unsafe_cobj() const{return obj_;}
private:
    CLockObj(const __Myt &);    //disable copy and assignment
    __Myt & operator =(const __Myt &);
    //fields
    lock_type lock_;
    value_type obj_;
};

/*
template<class T, class LockT = CMutex>
class CLockList
{
    //typedefs
    typedef CLockList<T, LockT> __Myt;
public:
    typedef LockT               lock_type;
    typedef CGuard<lock_type>   guard_type;
    typedef std::list<T>        container_type;
    typedef typename container_type::value_type       value_type;
    typedef typename container_type::size_type        size_type;
    typedef typename container_type::reference        reference;
    typedef typename container_type::iterator         iterator;
    typedef typename container_type::const_reference  const_reference;
    typedef typename container_type::const_iterator   const_iterator;
    //functions
    CLockList();
    bool empty() const volatile{
        guard_type g(lock_);
        return c().empty();
    }
    size_type size() const volatile{
        guard_type g(lock_);
        return c().size();
    }
    size_type maxSize() const{return c().max_size();}
    iterator pushFront(const_reference v) volatile{
        guard_type g(lock_);
        return c().insert(c().begin(), v);
    }
    void popFront(reference v) volatile{
        guard_type g(lock_);
        v = c().front();
        c().pop_front();
    }
    iterator pushBack(const_reference v) volatile{
        guard_type g(lock_);
        return c().insert(c().end(), v);
    }
    void popBack(reference v) volatile{
        guard_type g(lock_);
        v = c().back();
        c().pop_back();
    }
    iterator insert(iterator pos, const_reference v) volatile{
        guard_type g(lock_);
        return c().insert(pos, v);
    }
    iterator insert(iterator pos, size_type n, const_reference v) volatile{
        guard_type g(lock_);
        return c().insert(pos, n, v);
    }
    void erase(iterator it) volatile{
        guard_type g(lock_);
        c().erase(it);
    }
    void erase(iterator first, iterator last) volatile{
        guard_type g(lock_);
        c().erase(first, last);
    }
    void swap(container_type & c) volatile{
        guard_type g(lock_);
        c().swap(c);
    }
    void clear() volatile{
        guard_type g(lock_);
        c().clear();
    }
private:
    CLockList(const __Myt & a); //disable copy and assignment
    __Myt & operator =(const __Myt & a);
    container_type & c() volatile{return const_cast<container_type &>(con_);}
    const container_type & c() const volatile{return const_cast<const container_type &>(con_);}
    //fields
    container_type con_;
    lock_type lock_;
};*/

NS_SERVER_END

#endif

