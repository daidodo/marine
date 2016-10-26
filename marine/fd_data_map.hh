#ifndef DOZERG_FD_DATA_MAP_H_20130228
#define DOZERG_FD_DATA_MAP_H_20130228

/*
    �ṩfd�������ӳ��
    ��fd��Ϊ��������Ҫ���б߽�����Զ�����
        CFdDataMap  ��fdΪ�������������̰߳�ȫ
                    �ڲ��洢CSharedPtr<T>������������ָ��:w
    History
        20080604    CFdSockMap���Ӷ�CSharedPtr��֧�֣���Ҫ��putSock�Ĵ���ͬ
        20080910    CFdSockMap����sz_�ֶΣ�Size()�����������Ϊ����fd�ĸ���
                    GetSock()�����ڵ�null_ptr�������ͼ���const�޶�
        20080911    CFdSockMap����CloseSock()����������رմ�������ʱƵ������
                    CFdSockMap����SetSock()���������أ�ͨ��һ�β����ȸ������Ӷ����ֵõ��ɵ����Ӷ���
                    CFdSockMap�޸�ԭSetSock()����������del���������롰�ر����ӡ��롰�ͷŶ��󡱲���
        20080917    ��CFdMap::operator [] const����DEFAULT����
                    CFdMap��CFdSockMap����ģ�����Container�������ڲ���������
                    CFdMap����reserve()����
        20080920    CFdSockMapʹ��ģ���������������
        20081010    CFdSockMap::GetSock()��������ֵ��Ϊ__SockPtr������Ϊ����const���û������������map_[fd]
                    ����CFdSockMap::GetSock(int, __SockPtr &) const���أ�����һ����ʱ����
        20081013    ����CFdSockMap::GetSock(ForwardIter, ForwardIter, OutputIter) const���أ�������ȡ���Ӷ���
        20130228    �ع�CFdSockMap�������ڴ洢Socket���󣬲�����ΪCFdDataMap
//*/

#include <vector>
#include <cassert>
#include "mutex.hh"
#include "shared_ptr.hh"
#include "fd_map.hh"

NS_SERVER_BEGIN

template<class T, class LockT = CSpinLock>
class CFdDataMap
{
public:
    typedef T                       value_type;
    typedef CSharedPtr<value_type>  pointer;
private:
    typedef CFdMap<pointer>     __Map;
    typedef LockT               lock_type;
    typedef CGuard<lock_type>   guard_type;
public:
    //ctor
    //capacity: Ԥ��(fd -> data)�������ɼ����ڴ�������
    explicit CFdDataMap(size_t capacity = 100)
        : map_(capacity)
        , sz_(0)
    {}
    //��ȡ��ǰ��Ч��(fd -> data)����
    size_t size() const{return sz_;}
    //�޸�/��ȡ��ǰ�����ɵ�(fd -> data)����
    size_t capacity() const{
        guard_type g(lock_);
        return map_.capacity();
    }
    void capacity(size_t c){
        guard_type g(lock_);
        map_.capacity(c);
    }
    //����fd��Ӧ������
    //old: ����fd֮ǰ��Ӧ������
    void setData(int fd, const pointer & data, pointer * old = NULL){
        if(fd < 0)
            return;
        guard_type g(lock_);
        setAux(fd, data, old);
    }
    //��ȡfd��Ӧ������
    pointer getData(int fd) const{
        if(fd < 0)
            return pointer();
        guard_type g(lock_);
        return map_[fd];
    }
    //��ȡfd��Ӧ�����ݣ��Ż���
    void getData(int fd, pointer * data) const{
        if(NULL == data || fd < 0)
            return;
        guard_type g(lock_);
        *data = map_[fd];
    }
    //������ȡ����
    //first: fd���е���ʼ������
    //last: fd���еĽ���������
    //dstFirst: ����������е���ʼ������
    //ע��: �����߸���֤Ŀ�ĵ�dstFirst����Ч�Ժͷ�Χ
    template<class ForwardIter, class OutputIter>
    void getData(ForwardIter first, ForwardIter last, OutputIter dstFirst) const{
        guard_type g(lock_);
        for(int fd = -1;first != last;++first, ++dstFirst){
            fd = *first;
            if(fd >= 0)
                *dstFirst = map_[fd];
        }
    }
    //�Ƴ�fd��Ӧ������
    //old: ����fd֮ǰ��Ӧ������
    void clearData(int fd, pointer * old = NULL){
        if(fd < 0)
            return;
        guard_type g(lock_);
        setAux(fd, pointer(), old);
    }
    //�����Ƴ�����
    //first: fd���е���ʼ������
    //last: fd���еĽ���������
    //oldFirst: �������е���ʼ������������֮ǰ������
    //ע��: �����߸���֤Ŀ�ĵ�dstFirst����Ч�Ժͷ�Χ
    template<class ForwardIter>
    void clearData(ForwardIter first, ForwardIter last){
        guard_type g(lock_);
        for(int fd = -1;first != last;++first){
            fd = *first;
            if(fd >= 0)
                setAux(fd, pointer(), NULL);
        }
    }
    template<class ForwardIter, class OutputIter>
    void clearData(ForwardIter first, ForwardIter last, OutputIter dstFirst){
        guard_type g(lock_);
        for(int fd = -1;first != last;++first, ++dstFirst){
            fd = *first;
            if(fd >= 0)
                setAux(fd, pointer(), &*dstFirst);
        }
    }
    //�Ƴ���������
    void clear(){
        guard_type g(lock_);
        map_.clear();
        sz_ = 0;
    }
private:
    //set *old = map_[fd], then set map_[fd] = data
    //Note: old and &data may be equal
    void setAux(int fd, const pointer & data, pointer * old){
        assert(fd >= 0);
        if(fd < int(map_.capacity())){
            pointer & cur = map_[fd];
            sz_ += (data ? 1 : 0) - (cur ? 1 : 0);
            if(NULL != old)
                old->swap(cur);
            if(&data != old)
                cur = data;
        }else{
            if(data){
                map_[fd] = data;
                ++sz_;
            }
            if(NULL != old)
                old->reset();
        }
    }
    //fields:
    __Map map_;
    lock_type lock_;
    volatile size_t sz_;
};

NS_SERVER_END

#endif
