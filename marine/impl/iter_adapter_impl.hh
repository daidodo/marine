#ifndef DOZERG_ITERATOR_ADAPTER_IMPL_H_20081013
#define DOZERG_ITERATOR_ADAPTER_IMPL_H_20081013

#include <iterator>     //std::iterator_traits
#include <functional>   //std::unary_function

#include "environment.hh"

NS_IMPL_BEGIN

template<class ConstIter, class Extractor>
class CConstIterAdapter
{
    typedef CConstIterAdapter<ConstIter, Extractor> __Myt;
    typedef std::iterator_traits<ConstIter>         __Traits;
public:
    typedef ConstIter                               target_type;
    typedef Extractor                               extract_type;
    typedef typename __Traits::iterator_category    iterator_category;
    typedef typename extract_type::result_type      value_type;
    typedef const value_type *                      pointer;
    typedef const value_type &                      reference;
    typedef typename __Traits::difference_type      difference_type;
    CConstIterAdapter(target_type it, const extract_type & ext)
        : iter_(it)
        , ext_(ext)
    {}
    __Myt & operator =(target_type it){
        iter_ = it;
        return *this;
    }
    const target_type & target() const{return iter_;}
    reference operator *() const{return ext_(*iter_);}
    pointer operator ->() const{return &(operator *());}
    __Myt & operator +=(difference_type off){
        iter_ += off;
        return *this;
    }
    __Myt operator +(difference_type off) const{return __Myt(*this).operator +=(off);}
    __Myt & operator -=(difference_type off){return operator +=(-off);}
    __Myt operator -(difference_type off) const{return __Myt(*this).operator -=(off);}
    difference_type operator -(const __Myt & other) const{return iter_ - other.iter_;}
    reference operator [](difference_type off) const{return *operator +(off);}
    __Myt & operator ++(){
        ++iter_;
        return *this;
    }
    __Myt operator ++(int){
        __Myt tmp(*this);
        ++*this;
        return tmp;
    }
    __Myt & operator --(){
        --iter_;
        return *this;
    }
    __Myt operator --(int){
        __Myt tmp(*this);
        --*this;
        return tmp;
    }
    bool operator ==(const __Myt & other) const{return iter_ == other.iter_;}
    bool operator !=(const __Myt & other) const{return !operator ==(other);}
    bool operator <(const __Myt & other) const{return iter_  < other.iter_;}
    bool operator <=(const __Myt & other) const{return !operator >(other);}
    bool operator >(const __Myt & other) const{return other.operator <(*this);}
    bool operator >=(const __Myt & other) const{return !operator <(other);}
protected:
    target_type     iter_;
    extract_type    ext_;
};

template<class Iter, class Extractor>
class CIterAdapter : public CConstIterAdapter<Iter, Extractor>
{
    typedef CIterAdapter<Iter, Extractor>        __Myt;
    typedef CConstIterAdapter<Iter, Extractor>   __MyBase;
public:
    typedef typename __MyBase::target_type          target_type;
    typedef typename __MyBase::extract_type         extract_type;
    typedef typename __MyBase::iterator_category    iterator_category;
    typedef typename __MyBase::value_type           value_type;
    typedef value_type *                            pointer;
    typedef value_type &                            reference;
    typedef typename __MyBase::difference_type      difference_type;
    CIterAdapter(target_type it, const extract_type & ext)
        : __MyBase(it, ext)
    {}
    reference operator *(){return __MyBase::ext_(*__MyBase::iter_);}
    pointer operator ->(){return &(operator *());}
};

template<class Iter, class T, class Func>
class CIterAdapterFunctor
    : public std::unary_function<typename std::iterator_traits<Iter>::value_type, T>
{
    typedef typename std::iterator_traits<Iter>::value_type __Arg;
    typedef std::unary_function<__Arg, T>                   __MyBase;
public:
    typedef typename __MyBase::argument_type    argument_type;
    typedef typename __MyBase::result_type      result_type;
    explicit CIterAdapterFunctor(Func f):fun_(f){}
    const result_type & operator ()(const argument_type & t) const{return fun_(t);}
    result_type & operator ()(argument_type & t){return fun_(t);}
private:
    Func fun_;
};

NS_IMPL_END

#endif
