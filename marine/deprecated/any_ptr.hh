#ifndef DOZERG_ANY_PTR_H_20130301
#define DOZERG_ANY_PTR_H_20130301

/*
        CAnyPtr     可以存放任何指针的类型，并保证指针转换的正确性
//*/

#include <typeinfo>      //std::type_info
#include "to_string.hh"
#include "tools/other.hh"    //CxxDemangle

NS_SERVER_BEGIN

//TODO: unit test
class CAnyPtr
{
    typedef void (CAnyPtr::*safe_bool_type)() const;
public:
    CAnyPtr():p_(0),t_(0){}
    template<class T>
    explicit CAnyPtr(T * p)
        : p_(p)
        , t_(&typeid(T))
    {}
    template<class T>
    CAnyPtr & operator =(T * p){
        p_ = p;
        t_ = &typeid(T);
        return *this;
    }
    template<class T>
    T * CastTo() const{
        if(!t_ || *t_ != typeid(T))
            return 0;
        return reinterpret_cast<T *>(p_);
    }
    void Reset(){
        p_ = 0;
        t_ = 0;
    }
    operator safe_bool_type() const{return (p_ ? &CAnyPtr::dummyFun : 0);}
    std::string ToString() const{
        CToString oss;
        oss<<"{p_=@"<<p_
            <<", t_=@"<<t_;
        if(t_)
            oss<<"("<<tools::CxxDemangle(t_->name())<<")";
        oss<<"}";
        return oss.str();
    }
private:
    void dummyFun() const{}
    void * p_;
    const std::type_info * t_;
};

template<class T>
inline T * PtrCast(const CAnyPtr & p)
{
    return p.CastTo<T>();
}

NS_SERVER_END

#endif
