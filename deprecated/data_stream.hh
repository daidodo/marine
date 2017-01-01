#ifndef DOZERG_DATA_STREAM_H_20070905
#define DOZERG_DATA_STREAM_H_20070905

/*
    ��װ�������Ķ�ȡ��д��
    ע�����operator !(), �ڴ���״̬��, ���ж�д���ݲ���������Ч
    ����:
        CInByteStream           ���ֽ�Ϊ��λ��������
        COutByteStream          ��std::stringΪ�ײ�buf���ֽ������
        COutByteStreamStr       ͬCOutByteStream
        COutByteStreamStrRef    ���ⲿstd::string����Ϊ�ײ�buf���ֽ������
        COutByteStreamVec       ��std::vector<char>Ϊ�ײ�buf���ֽ������
        COutByteStreamVecRef    ���ⲿstd::vector<char>����Ϊ�ײ�buf���ֽ������
        COutByteStreamBuf       ��(char *, size_t)Ϊ�ײ�buf���ֽ������
    ������:
        raw                     ����/����������ݣ��޳����ֶ�
        array                   ����/����������ݣ��г����ֶ�
        net_order               ��������/�����Ϊ�����ֽ���
        host_order              ��������/�����Ϊ�����ֽ���
        little_endian           ��������/�����ΪС���ֽ���(little endian)
        big_endian              ��������/�����Ϊ����ֽ���(big endian��������)
        net_order_value         �������ֽ����д�ֶΣ����ı�����/��������ֽ���
        host_order_value        �Ա����ֽ����д�ֶΣ����ı�����/��������ֽ���
        little_endian_value     ��С���ֽ���(little endian)��д�ֶΣ����ı�����/��������ֽ���
        big_endian_value        �Դ���ֽ���(big endian��������)��д�ֶΣ����ı�����/��������ֽ���
        seek                    ��������/�������curָ��
        skip                    �޸�����/�������curָ��
        offset_value            ��ָ��λ������/������ݣ����ı�curָ��
        insert                  ��ָ��λ�ò�������
        protobuf                ��װprotobuf����������
        varint                  ʹ��Base 128 Varints���������
        end                     ����������: �����ʣ�����ݣ������ô���״̬���������
                                ���������: ���ö�Ӧ��finish()
    Manual:
        ��ο�"document/data_stream-manual.txt"

//*/

#include <cassert>
#include <cstring>  //memcpy
#include <vector>
#include "impl/data_stream_impl.hh"

NS_SERVER_BEGIN

//TODO:
//ds<<x<<y<<Manip::offset_value(off, in.size())

namespace Manip{

    //��/д���飨�޳����ֶΣ�
    template<class T, size_t N>
    inline NS_IMPL::CManipulatorRawPtr<T> raw(T (&c)[N]){
        return NS_IMPL::CManipulatorRawPtr<T>(c, N);
    }

    template<class T>
    inline NS_IMPL::CManipulatorRawPtr<T> raw(T * c, size_t sz){
        return NS_IMPL::CManipulatorRawPtr<T>(c, sz);
    }

    //��ȡ/д��vector���޳����ֶΣ�
    template<class T>
    inline NS_IMPL::CManipulatorRawPtr<T> raw(std::vector<T> & c, size_t sz){
        const size_t old = c.size();
        c.resize(old + sz);
        return NS_IMPL::CManipulatorRawPtr<T>(&c[old], sz);
    }

    template<class T>
    inline NS_IMPL::CManipulatorRawPtr<const T> raw(const std::vector<T> & c, size_t * sz = NULL){
        if(sz)
            *sz = c.size();
        return NS_IMPL::CManipulatorRawPtr<const T>(&c[0], c.size());
    }

    //��ȡ/д��basic_string���޳����ֶΣ�
    template<typename Char>
    inline NS_IMPL::CManipulatorRawPtr<Char> raw(std::basic_string<Char> & c, size_t len){
        const size_t old = c.size();
        c.append(len, 0);
        return NS_IMPL::CManipulatorRawPtr<Char>(&c[old], len);
    }

    template<typename Char>
    inline NS_IMPL::CManipulatorRawPtr<const Char> raw(const std::basic_string<Char> & c, size_t * sz = NULL){
        if(sz)
            *sz = c.size();
        return NS_IMPL::CManipulatorRawPtr<const Char>(c.c_str(), c.size());
    }

    //��ȡ/д���������Χ[first, last)���޳����ֶΣ�
    template<class ForwardIter>
    inline NS_IMPL::CManipulatorRawRange<ForwardIter> raw(ForwardIter first, ForwardIter last, size_t * sz = NULL){
        return NS_IMPL::CManipulatorRawRange<ForwardIter>(first, last, sz);
    }

    //��ȡ/д���������޳����ֶΣ�
    template<class T>
    inline NS_IMPL::CManipulatorRawCont<T> raw(T & c, size_t sz){
        return NS_IMPL::CManipulatorRawCont<T>(c, sz, NULL);
    }

    template<class T>
    inline NS_IMPL::CManipulatorRawCont<const T> raw(const T & c, size_t * sz = NULL){
        return NS_IMPL::CManipulatorRawCont<const T>(c, 0, sz);
    }

    //��ȡ/д�����飨�г����ֶΣ�
    template<typename LenT, class T, size_t N>
    inline NS_IMPL::CManipulatorArrayPtr<LenT, T> array(T (&c)[N], LenT * real_sz){
        return NS_IMPL::CManipulatorArrayPtr<LenT, T>(c, N, real_sz);
    }

    template<typename LenT, class T, size_t N>
    inline NS_IMPL::CManipulatorArrayPtr<LenT, const T> array(const T (&c)[N]){
        return NS_IMPL::CManipulatorArrayPtr<LenT, const T>(c, N, NULL);
    }

    template<typename LenT, class T>
    inline NS_IMPL::CManipulatorArrayPtr<LenT, T> array(T * c, size_t sz, LenT * real_sz){
        return NS_IMPL::CManipulatorArrayPtr<LenT, T>(c, sz, real_sz);
    }

    template<typename LenT, class T>
    inline NS_IMPL::CManipulatorArrayPtr<LenT, const T> array(const T * c, size_t sz){
        return NS_IMPL::CManipulatorArrayPtr<LenT, const T>(c, sz, NULL);
    }

    //��ȡ/д���������г����ֶΣ�
    template<typename LenT, class T>
    inline NS_IMPL::CManipulatorArrayCont<LenT, T> array(T & c, LenT max_size = 0){
        return NS_IMPL::CManipulatorArrayCont<LenT, T>(c, max_size);
    }

    template<typename LenT, class T>
    inline NS_IMPL::CManipulatorArrayCont<LenT, const T> array(const T & c){
        return NS_IMPL::CManipulatorArrayCont<LenT, const T>(c, 0);
    }

    template<class T>
    inline NS_IMPL::CManipulatorArrayCont<uint16_t, T> array(T & c, uint16_t max_size = 0){
        return NS_IMPL::CManipulatorArrayCont<uint16_t, T>(c, max_size);
    }

    template<class T>
    inline NS_IMPL::CManipulatorArrayCont<uint16_t, const T> array(const T & c){
        return NS_IMPL::CManipulatorArrayCont<uint16_t, const T>(c, 0);
    }

    //д���������Χ[first, last)���г����ֶΣ�
    template<typename LenT, class ForwardIter>
    inline NS_IMPL::CManipulatorArrayRange<LenT, ForwardIter> array(ForwardIter first, ForwardIter last, LenT * sz = NULL){
        return NS_IMPL::CManipulatorArrayRange<LenT, ForwardIter>(first, last, sz);
    }

    //����CDataStreamBase������Ϊ�����ֽ���
    inline void net_order(NS_IMPL::CDataStreamBase & ds){ds.netByteOrder(true);}

    //����CDataStreamBase������Ϊ�����ֽ���
    inline void host_order(NS_IMPL::CDataStreamBase & ds){ds.netByteOrder(false);}

    //����CDataStreamBase������ΪС���ֽ���(little endian)
    inline void little_endian(NS_IMPL::CDataStreamBase & ds){ds.littleEndian(true);}

    //����CDataStreamBase������Ϊ����ֽ���(big endian��������)
    inline void big_endian(NS_IMPL::CDataStreamBase & ds){ds.littleEndian(false);}

    //��ʱ����CDataStreamBase������Ϊ�����ֽ���Ȼ���д�ֶ�
    template<class T>
    inline NS_IMPL::CManipulatorValueByteOrder<T> net_order_value(T & val){
        return NS_IMPL::CManipulatorValueByteOrder<T>(val, net_order);
    }

    template<class T>
    inline NS_IMPL::CManipulatorValueByteOrder<const T> net_order_value(const T & val){
        return NS_IMPL::CManipulatorValueByteOrder<const T>(val, net_order);
    }

    //��ʱ����CDataStreamBase������Ϊ�����ֽ���Ȼ���д�ֶ�
    template<class T>
    inline NS_IMPL::CManipulatorValueByteOrder<T> host_order_value(T & val){
        return NS_IMPL::CManipulatorValueByteOrder<T>(val, host_order);
    }

    template<class T>
    inline NS_IMPL::CManipulatorValueByteOrder<const T> host_order_value(const T & val){
        return NS_IMPL::CManipulatorValueByteOrder<const T>(val, host_order);
    }

    //��ʱ����CDataStreamBase������ΪС�˽���(little endian)��Ȼ���д�ֶ�
    template<class T>
    inline NS_IMPL::CManipulatorValueByteOrder<T> little_endian_value(T & val){
        return NS_IMPL::CManipulatorValueByteOrder<T>(val, little_endian);
    }

    template<class T>
    inline NS_IMPL::CManipulatorValueByteOrder<const T> little_endian_value(const T & val){
        return NS_IMPL::CManipulatorValueByteOrder<const T>(val, little_endian);
    }

    //��ʱ����CDataStreamBase������Ϊ��˽���(big endian��������)��Ȼ���д�ֶ�
    template<class T>
    inline NS_IMPL::CManipulatorValueByteOrder<T> big_endian_value(T & val){
        return NS_IMPL::CManipulatorValueByteOrder<T>(val, big_endian);
    }

    template<class T>
    inline NS_IMPL::CManipulatorValueByteOrder<const T> big_endian_value(const T & val){
        return NS_IMPL::CManipulatorValueByteOrder<const T>(val, big_endian);
    }

    //����curָ���ֵ
    inline NS_IMPL::CManipulatorSeek seek(size_t pos){return NS_IMPL::CManipulatorSeek(pos);}

    //�޸�curָ���ֵΪ(cur + off)
    inline NS_IMPL::CManipulatorSkip skip(ssize_t off){return NS_IMPL::CManipulatorSkip(off);}

    inline NS_IMPL::CManipulatorSkipFill skip(ssize_t off, int fill){return NS_IMPL::CManipulatorSkipFill(off, fill);}

    template<typename T>
    inline NS_IMPL::CManipulatorSkipPtr<T> skip(T * off){return NS_IMPL::CManipulatorSkipPtr<T>(off);}

    template<typename T>
    inline NS_IMPL::CManipulatorSkipPtrFill<T> skip(T * off, int fill){return NS_IMPL::CManipulatorSkipPtrFill<T>(off);}

    //��ָ��λ�ö�д�ֶΣ����ı�curָ���ֵ
    template<class T>
    inline NS_IMPL::CManipulatorOffsetValue<T> offset_value(size_t pos, T & val){
        return NS_IMPL::CManipulatorOffsetValue<T>(pos, val);
    }

    template<class T>
    inline NS_IMPL::CManipulatorOffsetValue<const T> offset_value(size_t pos, const T & val){
        return NS_IMPL::CManipulatorOffsetValue<const T>(pos, val);
    }

    //��ָ��λ�ò����ֶ�
    //���pos��curָ��֮ǰ����д��val�����Һ���������Ӧ����
    //���pos��curָ��֮������seek(pos)��Ȼ��д��val
    template<class T>
    inline NS_IMPL::CManipulatorInsert<T> insert(size_t pos, const T & val){
        return NS_IMPL::CManipulatorInsert<T>(pos, val);
    }

    //��дprotobuf��Ϣ
    template<class T>
    inline NS_IMPL::CManipulatorProtobuf<T> protobuf(T & msg, size_t size = size_t(-1)){
        return NS_IMPL::CManipulatorProtobuf<T>(msg, size);
    }

    //ʹ��Base 128 Varints���������
    template<typename T>
    inline NS_IMPL::CManipulatorVarint<const T> varint(const T & val){
        return NS_IMPL::CManipulatorVarint<const T>(val);
    }

    template<typename T>
    inline NS_IMPL::CManipulatorVarint<T> varint(T & val){
        return NS_IMPL::CManipulatorVarint<T>(val);
    }

    //����CInByteStream: �����ʣ�����ݣ������ô����룻���򣬺���
    //����COutByteStreamXXX: ���ö�Ӧ��finish()
    inline NS_IMPL::CManipulatorEnd<void, void> end(){
        return NS_IMPL::CManipulatorEnd<void, void>();
    }

    template<typename SizeT>
    inline NS_IMPL::CManipulatorEnd<SizeT *, void> end(SizeT * sz){
        return NS_IMPL::CManipulatorEnd<SizeT *, void>(sz);
    }

    template<class BufT>
    inline NS_IMPL::CManipulatorEnd<BufT, void> end(BufT & buf){
        return NS_IMPL::CManipulatorEnd<BufT, void>(buf);
    }

    template<typename CharT>
    inline NS_IMPL::CManipulatorEnd<CharT *, size_t *> end(CharT * buf, size_t * sz){
        return NS_IMPL::CManipulatorEnd<CharT *, size_t *>(buf, sz);
    }

}//namespace Manip

class CInByteStream : public NS_IMPL::CDataStreamBase
{
    typedef NS_IMPL::CDataStreamBase __MyBase;
    typedef CInByteStream __Myt;
    const char *    data_;
    size_t          len_;
    size_t          cur_;
public:
    //��ʼ��
    //buf: Դ����
    //sz: buf�ֽڳ���
    //net:
    //  true    Դ����Ϊ�����ֽ���
    //  false   Դ����Ϊ�����ֽ���
    CInByteStream()
        : __MyBase(kByteOrderDefault, 1)
        , data_(NULL)
        , len_(0)
        , cur_(0)
    {}
    CInByteStream(const char * buf, size_t sz, bool net = kByteOrderDefault){
        setSource(buf, sz, net);
    }
    CInByteStream(const unsigned char * buf, size_t sz, bool net = kByteOrderDefault){
        setSource(buf, sz, net);
    }
    CInByteStream(const signed char * buf, size_t sz, bool net = kByteOrderDefault){
        setSource(buf, sz, net);
    }
    CInByteStream(const std::string & buf, bool net = kByteOrderDefault){
        setSource(buf, net);
    }
    void setSource(const char * buf, size_t sz, bool net = kByteOrderDefault){
        data_ = buf;
        len_ = sz;
        cur_ = 0;
        netByteOrder(net);
        status(0);
    }
    void setSource(const unsigned char * buf, size_t sz, bool net = kByteOrderDefault){
        setSource(reinterpret_cast<const char *>(buf), sz, net);
    }
    void setSource(const signed char * buf, size_t sz, bool net = kByteOrderDefault){
        setSource(reinterpret_cast<const char *>(buf), sz, net);
    }
    void setSource(const std::string & buf, bool net = kByteOrderDefault){
        setSource(buf.c_str(), buf.size(), net);
    }
    //���ô�����
    __Myt & bad(int code){
        status(code);
        return *this;
    }
    //����curָ���ֵ
    //ע�⣺���cur��С���൱������ʹ��ǰ������ݣ����cur����ˣ��൱������ָ��������
    //return:
    //  <0      ����ʧ�ܣ�������statusΪ��0
    //  ����    ���ú��cur()
    ssize_t seek(size_t pos){
        if(!good())
            return -1;
        if(pos <= len_)
            return (cur_ = pos);
        status(1);
        return -1;
    }
    //�޸�curָ���ֵΪ(cur + off)
    //ע�⣺���cur��С���൱������ʹ��֮ǰ�����ݣ����cur����ˣ��൱������ָ��������
    //off: curָ��Ҫ���ϵ�ƫ��
    //return:
    //  <0      �޸�ʧ�ܣ�������statusΪ��0
    //  ����    �޸ĺ��cur()
    ssize_t skip(ssize_t off){return seek(cur_ + off);}
    //��ȡcurָ���ֵ
    size_t cur() const{return cur_;}
    //��ȡʣ������
    const char * data() const{return (data_ + cur_);}
    //��ȡʣ����ֽ���
    size_t left() const{return (len_ - cur_);}
    //��ȡ��������
    __Myt & operator >>(char & c)               {return readPod(c);}
    __Myt & operator >>(signed char & c)        {return readPod(c);}
    __Myt & operator >>(unsigned char & c)      {return readPod(c);}
    __Myt & operator >>(short & c)              {return readPod(c);}
    __Myt & operator >>(unsigned short & c)     {return readPod(c);}
    __Myt & operator >>(int & c)                {return readPod(c);}
    __Myt & operator >>(unsigned int & c)       {return readPod(c);}
    __Myt & operator >>(long & c)               {return readPod(c);}
    __Myt & operator >>(unsigned long & c)      {return readPod(c);}
    __Myt & operator >>(long long & c)          {return readPod(c);}
    __Myt & operator >>(unsigned long long & c) {return readPod(c);}
    __Myt & operator >>(wchar_t & c)            {return readPod(c);}
    //��ȡ��������(�޳����ֶ�)
    template<class T, size_t N>
    __Myt & operator >>(T (&c)[N]){return readRaw(c, N);}
    template<class T>
    __Myt & operator >>(const NS_IMPL::CManipulatorRawPtr<T> & m){
        return readRaw(m.c_, m.sz_);
    }
    template<class T>
    __Myt & operator >>(const NS_IMPL::CManipulatorRawCont<T> & m){
        typedef typename T::value_type __Val;
        for(size_t i = 0;good() && i < m.sz_;++i){
            __Val v;
            if(*this>>v)
                m.c_.insert(m.c_.end(), v);
        }
        return *this;
    }
    template<class ForwardIter>
    __Myt & operator >>(const NS_IMPL::CManipulatorRawRange<ForwardIter> & m){
        size_t sz = 0;
        for(ForwardIter i = m.beg_;good() && i != m.end_;++i, ++sz)
            *this>>*i;
        if(m.sz_)
            *m.sz_ = sz;
        return *this;
    }
    //��ȡ�䳤����(�г����ֶ�)
    template<typename LenT, class T>
    __Myt & operator >>(const NS_IMPL::CManipulatorArrayPtr<LenT, T> & m){
        LenT sz;
        if(*this>>sz){
            if(size_t(sz) > m.sz1_)
                return bad(1);
            if((*this>>Manip::raw(m.c_, sz)) && m.sz2_)
                *m.sz2_ = sz;
        }
        return *this;
    }
    template<typename LenT, class T>
    __Myt & operator >>(const NS_IMPL::CManipulatorArrayCont<LenT, T> & m){
        __UNUSED typedef typename T::value_type __Val;  //make sure T is a Container
        LenT sz;
        if(*this>>sz){
            if(m.max_ && sz > m.max_)
                return bad(1);
            *this>>Manip::raw(m.c_, sz);
        }
        return *this;
    }
    __Myt & operator >>(std::string & c){return (*this>>Manip::array(c));}
    __Myt & operator >>(std::wstring & c){return (*this>>Manip::array(c));}
    //����ָ���Ͳ�����
    __Myt & operator >>(void (*m)(__MyBase &)){
        if(m)
            m(*this);
        return *this;
    }
    //ָ���ֽ����ȡ�ֶ�
    template<class T>
    __Myt & operator >>(const NS_IMPL::CManipulatorValueByteOrder<T> & m){
        const bool old = littleEndian();
        *this>>m.fun_>>m.c_;
        littleEndian(old);
        return *this;
    }
    //����curָ���ֵ
    __Myt & operator >>(const NS_IMPL::CManipulatorSeek & m){
        seek(m.pos_);
        return *this;
    }
    //�޸�curָ���ֵ
    __Myt & operator >>(const NS_IMPL::CManipulatorSkip & m){
        skip(m.off_);
        return *this;
    }
    template<typename T>
    __Myt & operator >>(const NS_IMPL::CManipulatorSkipPtr<T> & m){
        if(m.off_)
            skip(*m.off_);
        return *this;
    }
    //��ָ��λ�ö�ȡ�ֶΣ����ı�curָ���ֵ
    template<class T>
    __Myt & operator >>(const NS_IMPL::CManipulatorOffsetValue<T> & m){
        const size_t old = cur();
        if(seek(m.pos_) >= 0 && (*this>>m.c_))
            seek(old);
        return *this;
    }
    //��ȡprotobuf��Ϣ
    template<class T>
    __Myt & operator >>(const NS_IMPL::CManipulatorProtobuf<T> & m){
        size_t sz = left();
        if(sz > m.sz_)
            sz = m.sz_;
        if(!m.c_.ParseFromArray(data(), sz))
            return bad(1);
        skip(sz);
        return *this;
    }
    //ʹ��Base 128 Varints��������
    template<typename T>
    __Myt & operator >>(const NS_IMPL::CManipulatorVarint<T> & m){
        typename NS_IMPL::CManipulatorVarint<T>::__Unsigned v = 0;
        if(readVarint(v))
            m.fromUnsigned(v);
        return *this;
    }
    //�����ʣ�����ݣ������ô����룻�������
    template<class T, class S>
    __Myt & operator >>(const NS_IMPL::CManipulatorEnd<T, S> & m){
        if(good() && left())
            status(1);
        return *this;
    }
    __Myt & operator >>(NS_IMPL::CManipulatorEnd<void, void> (*m)()){
        if(!m)
            return bad(1);
        return (*this>>m());
    }
    //����ɶ��ַ���
    std::string toString() const{
        CToString oss;
        oss<<"{CDataStreamBase="<<__MyBase::toString()
            <<", cur_="<<cur_
            <<", data_="<<NS_IMPL::dumpBufPart(data_, len_, cur_)
            <<"}";
        return oss.str();
    }
private:
    CInByteStream(const char * buf);    //maybe you miss 'sz'. Disable implicit cast to std::string
    template<typename T>
    __Myt & readPod(T & c){
        if(ensure(sizeof(T))){
            memcpy(&c, data(), sizeof(T));
            if(sizeof(T) > 1 && needReverse())
                c = tools::SwapByteOrder(c);
            cur_ += sizeof(T);
        }
        return *this;
    }
    __Myt & readVarint(unsigned char & c){
        if(ensure(1)){
            c = data_[cur_++];
            if(c < (1 << 7))
                return *this;
            if(ensure(1))
                c |= data_[cur_++] << 7;
        }
        return *this;
    }
#if 0
    template<typename T>
    __Myt & readVarint(T & c){
        for(uint8_t v = -1, s = 0;v > 0x7F && readPod(v);s += 7)
            c |= T(v & 0x7F) << s;
        return *this;
    }
#endif
#if 1
    template<typename T>
    __Myt & readVarint(T & c){
        if(ensure(1)){
            c = data_[cur_++];
            if(c < (1 << 7))
                return *this;
            if(ensure(1)){
                c = (c & ((1 << 7) - 1)) | (data_[cur_++] << 7);
                if(c < (1 << 14))
                    return *this;
                c &= (1 << 14) - 1;
                for(uint8_t v = -1, s = 14;v > 0x7F && readPod(v);s += 7)
                    c |= T(v & 0x7F) << s;
            }
        }
        return *this;
    }
#endif
#if 0
    template<typename T>
    __Myt & readVarint(T & c){
        const size_t kMaxBytes = (CTypeTraits<T>::kMaxBits + 6) / 7;
        if(left() >= kMaxBytes){

        }else{
            for(uint8_t v = -1, s = 0;v > 0x7F && readPod(v);s += 7)
                c |= T(v & 0x7F) << s;
        }
        return *this;
    }
#endif
    template<typename T>
    __Myt & readRaw(T * c, size_t sz){
        if(!sz)
            return *this;
        if(!c)
            return bad(1);
        if(!CTypeTraits<T>::kCanMemcpy
                || (sizeof(T) > 1 && needReverse())){
            for(size_t i = 0;good() && i < sz;++i, ++c)
                *this>>*c;
        }else{
            sz *= sizeof(T);
            if(ensure(sz)){
                memcpy(c, data(), sz);
                cur_ += sz;
            }
        }
        return *this;
    }
    bool ensure(size_t sz){
        if(good()){
            if(left() >= sz)
                return true;
            status(1);
        }
        return false;
    }
};

template<class Data>
class COutByteStreamBasic : public NS_IMPL::CDataStreamBase
{
    typedef NS_IMPL::CDataStreamBase    __MyBase;
    typedef COutByteStreamBasic<Data>   __Myt;
    typedef Data                        __Data;
public:
    typedef typename Data::__Buf        buffer_type;
    typedef typename Data::__Char       char_type;
    //��ʼ��
    //net:
    //  true    Ŀ������Ϊ�����ֽ���
    //  false   Ŀ������Ϊ�����ֽ���
    //reserve: ����ײ�����������ʾԤ���Ļ�������С���ɱ���Ƶ�������ڴ�
    explicit COutByteStreamBasic(size_t reserve = 1024, bool net = kByteOrderDefault)
        : __MyBase(net)
        , data_(reserve)
    {}
    //buf: ����ײ���ָ�����������󣬱�ʾ�ö���
    explicit COutByteStreamBasic(buffer_type & buf, bool net = kByteOrderDefault)
        : __MyBase(net)
        , data_(buf)
    {}
    //buf: ����ײ��Ƕ�������������ʾ��������ַ
    //sz: ����ײ��Ƕ�������������ʾ��������С
    COutByteStreamBasic(char_type * buf, size_t sz, bool net = kByteOrderDefault)
        : __MyBase(net)
        , data_(buf, sz)
    {}
    //���ô�����
    __Myt & bad(int code){
        status(code);
        return *this;
    }
    //����curָ���ֵ
    //ע�⣺���cur��С���൱��Ĩ���˺�������ݣ����cur����ˣ��൱������ָ���Ŀռ�
    //return:
    //  <0      ����ʧ�ܣ�������statusΪ��0
    //  ����    ���ú��cur()
    ssize_t seek(size_t pos){
        if(!good())
            return -1;
        if(data_.seek(pos))
            return cur();
        status(1);
        return -1;
    }
    //��ȡcurָ���ֵ������ǰ��д������ݴ�С
    size_t cur() const{return data_.cur();}
    size_t size() const{return cur();}
    //�޸�curָ���ֵΪ(cur + off)
    //fill: ����ַ�
    //ע�⣺���cur��С���൱��Ĩ���˺�������ݣ����cur����ˣ��൱������ָ���Ŀռ�
    //return:
    //  <0      ����ʧ�ܣ�������statusΪ��0
    //  ����    ���ú��cur()
    ssize_t skip(ssize_t off){
        if(!good())
            return -1;
        if(off < 0 && size_t(-off) > cur()){
            bad(1);
            return -1;
        }
        return seek(cur() + off);
    }
    ssize_t skip(ssize_t off, int fill){
        const size_t old = cur();
        const ssize_t ret = skip(off);
        if(ret < 0)
            return ret;
        if(old < size_t(ret))
            std::fill(data_.data(old), data_.data(ret), fill);
        return ret;
    }
    //д���������
    __Myt & operator <<(char c)                 {return writePod(c);}
    __Myt & operator <<(signed char c)          {return writePod(c);}
    __Myt & operator <<(unsigned char c)        {return writePod(c);}
    __Myt & operator <<(short c)                {return writePod(c);}
    __Myt & operator <<(unsigned short c)       {return writePod(c);}
    __Myt & operator <<(int c)                  {return writePod(c);}
    __Myt & operator <<(unsigned int c)         {return writePod(c);}
    __Myt & operator <<(long c)                 {return writePod(c);}
    __Myt & operator <<(unsigned long c)        {return writePod(c);}
    __Myt & operator <<(long long c)            {return writePod(c);}
    __Myt & operator <<(unsigned long long c)   {return writePod(c);}
    __Myt & operator <<(wchar_t c)              {return writePod(c);}
    //д�붨������(�޳����ֶ�)
    template<class T, size_t N>
    __Myt & operator <<(const T (&c)[N]){return writeRaw(c, N);}
    template<class T>
    __Myt & operator <<(const NS_IMPL::CManipulatorRawPtr<T> & m){
        return writeRaw(m.c_, m.sz_);
    }
    template<class T>
    __Myt & operator <<(const NS_IMPL::CManipulatorRawCont<T> & m){
        const size_t sz = writeRange(m.c_.begin(), m.c_.end());
        if(m.sz2_)
            *m.sz2_ = sz;
        return *this;
    }
    template<class ForwardIter>
    __Myt & operator <<(const NS_IMPL::CManipulatorRawRange<ForwardIter> & m){
        const size_t sz = writeRange(m.beg_, m.end_);
        if(m.sz_)
            *m.sz_ = sz;
        return *this;
    }
    //д��䳤����(�г����ֶ�)
    template<typename LenT, class T>
    __Myt & operator <<(const NS_IMPL::CManipulatorArrayPtr<LenT, T> & m){
        if(*this<<LenT(m.sz1_))
            writeRaw(m.c_, m.sz1_);
        return *this;
    }
    template<typename LenT, class T>
    __Myt & operator <<(const NS_IMPL::CManipulatorArrayCont<LenT, T> & m){
        const size_t off = cur();
        *this<<LenT(0);
        if(!m.c_.empty()){
            size_t sz = 0;
            if(good() && *this<<Manip::raw(m.c_, &sz))   //NOTE: m.c_.size() may be O(N) complexity
                *this<<Manip::offset_value(off, LenT(sz));
        }
        return *this;
    }
    template<typename LenT, class ForwardIter>
    __Myt & operator <<(const NS_IMPL::CManipulatorArrayRange<LenT, ForwardIter> & m){
        const size_t off = cur();
        size_t sz = 0;
        *this<<LenT(0)<<Manip::raw(m.beg_, m.end_, &sz);
        if(good()){
            *this<<Manip::offset_value(off, LenT(sz));
            if(m.sz_)
                *m.sz_ = sz;
        }
        return *this;
    }
    __Myt & operator <<(const std::string & c){return (*this<<Manip::array(c));}
    __Myt & operator <<(const std::wstring & c){return (*this<<Manip::array(c));}
    //����ָ���Ͳ�����
    __Myt & operator <<(void (*m)(__MyBase &)){
        if(m)
            m(*this);
        return *this;
    }
    //ָ���ֽ���д������
    template<class T>
    __Myt & operator <<(const NS_IMPL::CManipulatorValueByteOrder<T> & m){
        const bool old = littleEndian();
        *this<<m.fun_<<m.c_;
        littleEndian(old);
        return *this;
    }
    //����curָ���ֵ
    __Myt & operator <<(const NS_IMPL::CManipulatorSeek & m){
        seek(m.pos_);
        return *this;
    }
    //�޸�curָ���ֵ
    __Myt & operator <<(const NS_IMPL::CManipulatorSkip & m){
        skip(m.off_);
        return *this;
    }
    __Myt & operator <<(const NS_IMPL::CManipulatorSkipFill & m){
        skip(m.off_, m.fill_);
        return *this;
    }
    template<typename T>
    __Myt & operator <<(const NS_IMPL::CManipulatorSkipPtr<T> & m){
        if(m.off_)
            skip(*m.off_);
        return *this;
    }
    template<typename T>
    __Myt & operator <<(const NS_IMPL::CManipulatorSkipPtrFill<T> & m){
        if(m.off_)
            skip(*m.off_, m.fill_);
        return *this;
    }
    //��ָ��λ��д���ֶΣ����ı�curָ���ֵ
    template<class T>
    __Myt & operator <<(const NS_IMPL::CManipulatorOffsetValue<T> & m){
        const size_t old = cur();
        if(seek(m.pos_) >= 0 && (*this<<m.c_))
            seek(old);
        return *this;
    }
    //��ָ��λ�ò����ֶΣ�����������Ӧ����
    template<class T>
    __Myt & operator <<(const NS_IMPL::CManipulatorInsert<T> & m){
        typedef COutByteStreamBasic<NS_IMPL::__buf_ref_data<std::string> > __OutStream;
        const size_t old = cur();
        if(old <= m.pos_){
            if(old == m.pos_ || seek(m.pos_) >= 0)
                *this<<m.c_;
        }else{
            std::string buf;
            __OutStream ds(buf);
            if(!(ds<<m.c_) || !ds.finish())
                return bad(1);
            if(!buf.empty() && ensure(buf.size()))
                data_.insert(m.pos_, cast(buf.c_str()), buf.size());
        }
        return *this;
    }
    //д��protobuf��Ϣ
    template<class T>
    __Myt & operator <<(const NS_IMPL::CManipulatorProtobuf<T> & m){
        const size_t old = size();
        const size_t sz = m.c_.ByteSize();
        if(skip(sz) >= 0 && !m.c_.SerializeToArray(data_.data(old), sz)){
            seek(old);
            status(1);
        }
        return *this;
    }
    //ʹ��Base 128 Varints��������
    template<typename T>
    __Myt & operator <<(const NS_IMPL::CManipulatorVarint<T> & m){
        return writeVarint(m.toUnsigned());
    }
    //���ö�Ӧ��finish()
    __Myt & operator <<(const NS_IMPL::CManipulatorEnd<void, void> & m){
        finish();
        return *this;
    }
    __Myt & operator <<(NS_IMPL::CManipulatorEnd<void, void> (*m)()){
        if(!m)
            return bad(1);
        return (*this<<m());
    }
    template<typename SizeT>
    __Myt & operator <<(const NS_IMPL::CManipulatorEnd<SizeT *, void> & m){
        finish(m.sz_);
        return *this;
    }
    __Myt & operator <<(const NS_IMPL::CManipulatorEnd<buffer_type, void> & m){
        finish(m.buf_);
        return *this;
    }
    template<typename CharT>
    __Myt & operator <<(const NS_IMPL::CManipulatorEnd<CharT *, size_t *> & m){
        finish(m.buf_, m.sz_);
        return *this;
    }
    //��������д������ݣ�������Լ�
    bool finish(){
        if(good() && !data_.exportData())
            status(1);
        return good();
    }
    //sz: ���ص������ݵ��ֽڳ���
    template<typename SizeT>
    bool finish(SizeT * sz){
        if(good() && !data_.exportData(sz))
            status(1);
        return good();
    }
    //dst: ����������׷�ӵ�dst�������ݺ���
    bool finish(buffer_type & dst){
        if(good() && !data_.exportData(dst))
            status(1);
        return good();
    }
    //dst: ʹ�õ������ݸ���dst���е�����
    //sz: �����ʾdst���ֽڳ��ȣ����ر�ʾ�������ݵ��ֽڳ���
    template<typename CharT>
    bool finish(CharT * dst, size_t * sz){
        if(good() && !data_.exportData(dst, sz))
            status(1);
        return good();
    }
    //����ɶ��ַ���
    std::string toString() const{
        CToString oss;
        oss<<"{CDataStreamBase="<<__MyBase::toString()
            <<", data_="<<data_.toString()
            <<"}";
        return oss.str();
    }
private:
    template<typename T>
    static const char_type * cast(const T * c){
        return reinterpret_cast<const char_type *>(c);
    }
    template<typename T>
    __Myt & writePod(T c){
        if(ensure(sizeof(T))){
            if(sizeof(T) > 1 && needReverse())
                c = tools::SwapByteOrder(c);
            data_.append(cast(&c), sizeof(T));
        }
        return *this;
    }
#if 1
    __Myt & writeVarint(unsigned char c){
        if(c < 0x80){
            if(ensure(1))
                data_.append(c);
        }else if(ensure(2)){
            data_.append(c | 0x80);
            data_.append(c >> 7);
        }
        return *this;
    }
    __Myt & writeVarint(unsigned short c){
        if(c < 0x80){
            if(ensure(1))
                data_.append(c);
        }else if(c < (1 << 14)){
            if(ensure(2)){
                data_.append(c | 0x80);
                data_.append(c >> 7);
            }
        }else if(ensure(3)){
            data_.append(c | 0x80);
            data_.append((c >> 7) | 0x80);
            data_.append(c >> 14);
        }
        return *this;
    }
#endif
#if 0
    template<typename T>
    __Myt & writeVarint(T c){
        while(ensure(1)){
            if(c < 0x80){
                data_.append(c);
                break;
            }else
                data_.append(c | 0x80);
            c >>= 7;
        }
        return *this;
    }
#endif
#if 0
    template<typename T>
    __Myt & writeVarint(T c){
        if(c < 0x80){
            if(ensure(1))
                data_.append(c);
        }else if(ensure(2)){
            data_.append(c | 0x80);
            do{
                if((c >>= 7) < 0x80){
                    data_.append(c);
                    break;
                }else
                    data_.append(c | 0x80);
            }while(ensure(1));
        }
        return *this;
    }
#endif
#if 0
    template<typename T>
    __Myt & writeVarint(T c){
        if(c < 0x80){
            if(ensure(1))
                data_.append(c);
        }else if(c < (1 << 14)){
            if(ensure(2)){
                data_.append(c | 0x80);
                data_.append(c >> 7);
            }
        }else if(ensure(3)){
            data_.append(c | 0x80);
            data_.append((c >> 7) | 0x80);
            c >>= 14;
            do{
                if(c < 0x80){
                    data_.append(c);
                    break;
                }else
                    data_.append(c | 0x80);
                c >>= 7;
            }while(ensure(1));
        }
        return *this;
    }
#endif
#if 1
    template<typename T>
    __Myt & writeVarint(T c){
        if(c < 0x80){
            if(ensure(1))
                data_.append(c);
        }else if(c < (1 << 14)){
            if(ensure(2)){
                data_.append(c | 0x80);
                data_.append(c >> 7);
            }
        }else if(c < (1 << 21)){
            if(ensure(3)){
                data_.append(c | 0x80);
                data_.append((c >> 7) | 0x80);
                data_.append(c >> 14);
            }
        }else if(c < (1 << 28)){
            if(ensure(4)){
                data_.append(c | 0x80);
                data_.append((c >> 7) | 0x80);
                data_.append((c >> 14) | 0x80);
                data_.append(c >> 21);
            }
        }else if(ensure(5)){
            data_.append(c | 0x80);
            data_.append((c >> 7) | 0x80);
            data_.append((c >> 14) | 0x80);
            data_.append((c >> 21) | 0x80);
            c >>= 28;
            do{
                if(c < 0x80){
                    data_.append(c);
                    break;
                }else
                    data_.append(c | 0x80);
                c >>= 7;
            }while(ensure(1));
        }
        return *this;
    }
#endif
#if 0
    template<typename T>
    __Myt & writeVarint(T c){
        if(c < (1 << 7)){
            if(ensure(1)){
                data_.append(c);
                return *this;
            }
        }else if(c < (1 << 14)){
            if(ensure(2)){
                data_.append(c | 0x80);
                data_.append(c >> 7);
                return *this;
            }
        }
        for(;ensure(1);c >>= 7){
            if(c < 0x80){
                data_.append(c);
                break;
            }else
                data_.append(c | 0x80);
        }
        return *this;
/*
        const size_t kMaxBytes = (CTypeTraits<T>::kMaxBits + 6) / 7;
        if(ensure(kMaxBytes)){
            const size_t old = data_.cur();
            if(c < 0x80){
                data_.append(c);
                return *this;
            }
            data_.append(c | 0x80);
            if(c < (1 << 14)){
                data_.append(c >> 7);
                return *this;
            }
            data_.append((c >> 7) | 0x80);
            if(c < (1 << 21)){
                data_.append(c >> 14);
                return *this;
            }
            data_.append((c >> 14) | 0x80);
            if(c < (1 << 28)){
                data_.append(c >> 21);
                return *this;
            }
            data_.seek(old);    //fall back
        }
        return writeVarintSlow(c);*/
    }
    template<typename T>
    __Myt & writeVarintSlow(T c){
        for(;ensure(1);c >>= 7){
            if(c < 0x80){
                data_.append(c);
                break;
            }else
                data_.append(c | 0x80);
        }
        return *this;
    }
#endif
    template<typename T>
    __Myt & writeRaw(const T * c, size_t sz){
        if(!sz)
            return *this;
        if(!c)
            return bad(1);
        if(!CTypeTraits<T>::kCanMemcpy
                || (sizeof(T) > 1 && needReverse())){
            for(size_t i = 0;good() && i < sz;++i,++c)
                *this<<*c;
        }else{
            sz *= sizeof(T);
            if(ensure(sz))
                data_.append(cast(c), sz);
        }
        return *this;
    }
    template<class ForwardIter>
    size_t writeRange(ForwardIter first, ForwardIter last){
        size_t c = 0;
        for(ForwardIter i = first;good() && i != last;++i, ++c)
            *this<<*i;
        return c;
    }
    bool ensure(size_t len){
        if(!good())
            return false;
        if(data_.ensure(len))
            return true;
        status(1);
        return false;
    }
    //members
    __Data data_;
};

//COutByteStream, COutByteStreamStr
typedef COutByteStreamBasic<NS_IMPL::__buf_data<std::string> > COutByteStreamStr;

typedef COutByteStreamStr COutByteStream;

//COutByteStreamStr
typedef COutByteStreamBasic<NS_IMPL::__buf_ref_data<std::string> > COutByteStreamStrRef;

//COutByteStreamVec
typedef COutByteStreamBasic<NS_IMPL::__buf_data<std::vector<char> > > COutByteStreamVec;

//COutByteStreamVecRef
typedef COutByteStreamBasic<NS_IMPL::__buf_ref_data<std::vector<char> > > COutByteStreamVecRef;

//COutByteStreamBuf
typedef COutByteStreamBasic<NS_IMPL::__buf_data<CCharBuffer<char> > > COutByteStreamBuf;

NS_SERVER_END

#endif

