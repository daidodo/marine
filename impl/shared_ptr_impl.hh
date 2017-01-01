#ifndef DOZERG_REFERENCE_COUNT_POINTER_IMPL_H_20080226
#define DOZERG_REFERENCE_COUNT_POINTER_IMPL_H_20080226

/*
    CSharedPtr���ڲ�ʵ��
        __RefImp
    History
        20080605    ��__RefImp::subRef���p = 0,�����ظ�ɾ��bug
        20080827    ͨ��Alloc�õ�__ElemAlloc
        20120118    ����release����
//*/

#include "../atomic_sync.hh"
#include "../tools/memory.hh"

NS_IMPL_BEGIN

template<class T, class Alloc>
struct __RefImp{
    typedef CAtomicSync<int>                __Count;
    typedef typename Alloc::
        template rebind<T>::other           __ElemAlloc;
    typedef typename Alloc::
        template rebind<__RefImp>::other    __RefAlloc;
    explicit __RefImp(T * pe):ptr_(pe), cnt_(1){}
    void addRef(__RefImp *& p) throw(){
        p = this;
        ++cnt_;
    }
    static void changeRef(__RefImp *& p, __RefImp * v) throw(){
        if(p != v){
            subRef(p);
            if(v)
                v->addRef(p);
        }
    }
    static void subRef(__RefImp *& p) throw(){
        if(p){
            if(p->cnt_-- <= 1){
                tools::Delete(p->ptr_, __ElemAlloc());
                tools::Delete(p, __RefAlloc());
            }
            p = NULL;
        }
    }
private:
    __RefImp(const __RefImp &);
    __RefImp & operator =(const __RefImp &);
public:
    T *  ptr_;
private:
    __Count cnt_;
};

NS_IMPL_END

#endif
