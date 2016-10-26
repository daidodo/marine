#ifndef DOZERG_DATA_STREAM_H_20070905
#define DOZERG_DATA_STREAM_H_20070905

/*
    封装数据流的读取和写入
    注意测试operator !(), 在错误状态下, 所有读写数据操作都会无效
    类型:
        CInByteStream           以字节为单位的输入流
        COutByteStream          以std::string为底层buf的字节输出流
        COutByteStreamStr       同COutByteStream
        COutByteStreamStrRef    以外部std::string对象为底层buf的字节输出流
        COutByteStreamVec       以std::vector<char>为底层buf的字节输出流
        COutByteStreamVecRef    以外部std::vector<char>对象为底层buf的字节输出流
        COutByteStreamBuf       以(char *, size_t)为底层buf的字节输出流
    操作符:
        raw                     输入/输出数组数据，无长度字段
        array                   输入/输出数组数据，有长度字段
        net_order               设置输入/输出流为网络字节序
        host_order              设置输入/输出流为本地字节序
        little_endian           设置输入/输出流为小端字节序(little endian)
        big_endian              设置输入/输出流为大端字节序(big endian，网络序)
        net_order_value         以网络字节序读写字段，不改变输入/输出流的字节序
        host_order_value        以本地字节序读写字段，不改变输入/输出流的字节序
        little_endian_value     以小端字节序(little endian)读写字段，不改变输入/输出流的字节序
        big_endian_value        以大端字节序(big endian，网络序)读写字段，不改变输入/输出流的字节序
        seek                    设置输入/输出流的cur指针
        skip                    修改输入/输出流的cur指针
        offset_value            在指定位置输入/输出数据，不改变cur指针
        insert                  在指定位置插入数据
        protobuf                封装protobuf类的输入输出
        varint                  使用Base 128 Varints编解码整数
        end                     对于输入流: 如果有剩余数据，则设置错误状态；否则忽略
                                对于输出流: 调用对应的finish()
    Manual:
        请参考"document/data_stream-manual.txt"

//*/

#include <cassert>
#include <cstring>  //memcpy
#include <vector>
#include "impl/data_stream_impl.hh"

NS_SERVER_BEGIN

//TODO:
//ds<<x<<y<<Manip::offset_value(off, in.size())

namespace Manip{

    //读/写数组（无长度字段）
    template<class T, size_t N>
    inline NS_IMPL::CManipulatorRawPtr<T> raw(T (&c)[N]){
        return NS_IMPL::CManipulatorRawPtr<T>(c, N);
    }

    template<class T>
    inline NS_IMPL::CManipulatorRawPtr<T> raw(T * c, size_t sz){
        return NS_IMPL::CManipulatorRawPtr<T>(c, sz);
    }

    //读取/写入vector（无长度字段）
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

    //读取/写入basic_string（无长度字段）
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

    //读取/写入迭代器范围[first, last)（无长度字段）
    template<class ForwardIter>
    inline NS_IMPL::CManipulatorRawRange<ForwardIter> raw(ForwardIter first, ForwardIter last, size_t * sz = NULL){
        return NS_IMPL::CManipulatorRawRange<ForwardIter>(first, last, sz);
    }

    //读取/写入容器（无长度字段）
    template<class T>
    inline NS_IMPL::CManipulatorRawCont<T> raw(T & c, size_t sz){
        return NS_IMPL::CManipulatorRawCont<T>(c, sz, NULL);
    }

    template<class T>
    inline NS_IMPL::CManipulatorRawCont<const T> raw(const T & c, size_t * sz = NULL){
        return NS_IMPL::CManipulatorRawCont<const T>(c, 0, sz);
    }

    //读取/写入数组（有长度字段）
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

    //读取/写入容器（有长度字段）
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

    //写入迭代器范围[first, last)（有长度字段）
    template<typename LenT, class ForwardIter>
    inline NS_IMPL::CManipulatorArrayRange<LenT, ForwardIter> array(ForwardIter first, ForwardIter last, LenT * sz = NULL){
        return NS_IMPL::CManipulatorArrayRange<LenT, ForwardIter>(first, last, sz);
    }

    //设置CDataStreamBase的数据为网络字节序
    inline void net_order(NS_IMPL::CDataStreamBase & ds){ds.netByteOrder(true);}

    //设置CDataStreamBase的数据为主机字节序
    inline void host_order(NS_IMPL::CDataStreamBase & ds){ds.netByteOrder(false);}

    //设置CDataStreamBase的数据为小端字节序(little endian)
    inline void little_endian(NS_IMPL::CDataStreamBase & ds){ds.littleEndian(true);}

    //设置CDataStreamBase的数据为大端字节序(big endian，网络序)
    inline void big_endian(NS_IMPL::CDataStreamBase & ds){ds.littleEndian(false);}

    //临时设置CDataStreamBase的数据为网络字节序，然后读写字段
    template<class T>
    inline NS_IMPL::CManipulatorValueByteOrder<T> net_order_value(T & val){
        return NS_IMPL::CManipulatorValueByteOrder<T>(val, net_order);
    }

    template<class T>
    inline NS_IMPL::CManipulatorValueByteOrder<const T> net_order_value(const T & val){
        return NS_IMPL::CManipulatorValueByteOrder<const T>(val, net_order);
    }

    //临时设置CDataStreamBase的数据为主机字节序，然后读写字段
    template<class T>
    inline NS_IMPL::CManipulatorValueByteOrder<T> host_order_value(T & val){
        return NS_IMPL::CManipulatorValueByteOrder<T>(val, host_order);
    }

    template<class T>
    inline NS_IMPL::CManipulatorValueByteOrder<const T> host_order_value(const T & val){
        return NS_IMPL::CManipulatorValueByteOrder<const T>(val, host_order);
    }

    //临时设置CDataStreamBase的数据为小端节序(little endian)，然后读写字段
    template<class T>
    inline NS_IMPL::CManipulatorValueByteOrder<T> little_endian_value(T & val){
        return NS_IMPL::CManipulatorValueByteOrder<T>(val, little_endian);
    }

    template<class T>
    inline NS_IMPL::CManipulatorValueByteOrder<const T> little_endian_value(const T & val){
        return NS_IMPL::CManipulatorValueByteOrder<const T>(val, little_endian);
    }

    //临时设置CDataStreamBase的数据为大端节序(big endian，网络序)，然后读写字段
    template<class T>
    inline NS_IMPL::CManipulatorValueByteOrder<T> big_endian_value(T & val){
        return NS_IMPL::CManipulatorValueByteOrder<T>(val, big_endian);
    }

    template<class T>
    inline NS_IMPL::CManipulatorValueByteOrder<const T> big_endian_value(const T & val){
        return NS_IMPL::CManipulatorValueByteOrder<const T>(val, big_endian);
    }

    //设置cur指针的值
    inline NS_IMPL::CManipulatorSeek seek(size_t pos){return NS_IMPL::CManipulatorSeek(pos);}

    //修改cur指针的值为(cur + off)
    inline NS_IMPL::CManipulatorSkip skip(ssize_t off){return NS_IMPL::CManipulatorSkip(off);}

    inline NS_IMPL::CManipulatorSkipFill skip(ssize_t off, int fill){return NS_IMPL::CManipulatorSkipFill(off, fill);}

    template<typename T>
    inline NS_IMPL::CManipulatorSkipPtr<T> skip(T * off){return NS_IMPL::CManipulatorSkipPtr<T>(off);}

    template<typename T>
    inline NS_IMPL::CManipulatorSkipPtrFill<T> skip(T * off, int fill){return NS_IMPL::CManipulatorSkipPtrFill<T>(off);}

    //在指定位置读写字段，不改变cur指针的值
    template<class T>
    inline NS_IMPL::CManipulatorOffsetValue<T> offset_value(size_t pos, T & val){
        return NS_IMPL::CManipulatorOffsetValue<T>(pos, val);
    }

    template<class T>
    inline NS_IMPL::CManipulatorOffsetValue<const T> offset_value(size_t pos, const T & val){
        return NS_IMPL::CManipulatorOffsetValue<const T>(pos, val);
    }

    //在指定位置插入字段
    //如果pos在cur指针之前，则写入val，并且后面数据相应后移
    //如果pos在cur指针之后，则先seek(pos)，然后写入val
    template<class T>
    inline NS_IMPL::CManipulatorInsert<T> insert(size_t pos, const T & val){
        return NS_IMPL::CManipulatorInsert<T>(pos, val);
    }

    //读写protobuf消息
    template<class T>
    inline NS_IMPL::CManipulatorProtobuf<T> protobuf(T & msg, size_t size = size_t(-1)){
        return NS_IMPL::CManipulatorProtobuf<T>(msg, size);
    }

    //使用Base 128 Varints编解码整数
    template<typename T>
    inline NS_IMPL::CManipulatorVarint<const T> varint(const T & val){
        return NS_IMPL::CManipulatorVarint<const T>(val);
    }

    template<typename T>
    inline NS_IMPL::CManipulatorVarint<T> varint(T & val){
        return NS_IMPL::CManipulatorVarint<T>(val);
    }

    //设置临时边界，在撤除之前，编解码数据不得超过边界
    //sz: (边界位置 - cur指针)的值
    inline NS_IMPL::CManipulatorStubPush stub(size_t sz){
        return NS_IMPL::CManipulatorStubPush(sz);
    }

    //撤除最近一次设置的临时边界
    //align: 是否将cur()调整到边界位置
    //check: 是否检查cur()等于边界位置
    inline NS_IMPL::CManipulatorStubPop stub_pop(bool align = false, bool check = false){
        return NS_IMPL::CManipulatorStubPop(align, check);
    }

    //对于CInByteStream: 如果有剩余数据，则设置错误码；否则，忽略
    //对于COutByteStreamXXX: 调用对应的finish()
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
    //初始化
    //buf: 源数据
    //sz: buf字节长度
    //net:
    //  true    源数据为网络字节序
    //  false   源数据为本地字节序
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
    //设置错误码
    __Myt & bad(int code){
        status(code);
        return *this;
    }
    //设置cur指针的值
    //注意：如果cur变小，相当于重新使用前面的数据；如果cur变大了，相当于跳过指定的数据
    //return:
    //  <0      设置失败，并设置status为非0
    //  其他    设置后的cur()
    ssize_t seek(size_t pos){
        if(!checkStub(pos))
            return -1;
        if(pos > len_){
            status(1);
            return -1;
        }
        return (cur_ = pos);
    }
    //修改cur指针的值为(cur + off)
    //注意：如果cur变小，相当于重新使用之前的数据；如果cur变大了，相当于跳过指定的数据
    //off: cur指针要加上的偏移
    //return:
    //  <0      修改失败，并设置status为非0
    //  其他    修改后的cur()
    ssize_t skip(ssize_t off){return seek(cur() + off);}
    //获取cur指针的值
    size_t cur() const{return cur_;}
    //获取剩余数据
    const char * data() const{return (data_ + cur());}
    //获取剩余的字节数
    size_t left() const{return (getStub(len_) - cur());}
    //读取基本类型
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
    //读取定长数组(无长度字段)
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
    //读取变长数组(有长度字段)
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
        LenT sz(0);
        if(*this>>sz){
            if(m.max_ && sz > m.max_)
                return bad(1);
            *this>>Manip::raw(m.c_, sz);
        }
        return *this;
    }
    __Myt & operator >>(std::string & c){return (*this>>Manip::array(c));}
    __Myt & operator >>(std::wstring & c){return (*this>>Manip::array(c));}
    //函数指针型操作符
    __Myt & operator >>(void (*m)(__MyBase &)){
        if(m)
            m(*this);
        return *this;
    }
    //指定字节序读取字段
    template<class T>
    __Myt & operator >>(const NS_IMPL::CManipulatorValueByteOrder<T> & m){
        const bool old = littleEndian();
        *this>>m.fun_>>m.c_;
        littleEndian(old);
        return *this;
    }
    //设置cur指针的值
    __Myt & operator >>(const NS_IMPL::CManipulatorSeek & m){
        seek(m.pos_);
        return *this;
    }
    //修改cur指针的值
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
    //在指定位置读取字段，不改变cur指针的值
    template<class T>
    __Myt & operator >>(const NS_IMPL::CManipulatorOffsetValue<T> & m){
        const size_t old = cur();
        if(seek(m.pos_) >= 0 && (*this>>m.c_))
            seek(old);
        return *this;
    }
    //读取protobuf消息
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
    //使用Base 128 Varints解码整数
    template<typename T>
    __Myt & operator >>(const NS_IMPL::CManipulatorVarint<T> & m){
        typename NS_IMPL::CManipulatorVarint<T>::__Unsigned v = 0;
        if(readVarint(v))
            m.fromUnsigned(v);
        return *this;
    }
    //设置临时边界
    __Myt & operator >>(const NS_IMPL::CManipulatorStubPush & m){
        pushStub(m.sz_ + cur());
        return *this;
    }
    //撤除临时边界
    __Myt & operator >>(const NS_IMPL::CManipulatorStubPop & m){
        size_t pos;
        if(popStub(&pos)){
            if(m.check_ && cur() != pos)
                return bad(1);
            seek(m.align_ ? pos : cur());
        }
        return *this;
    }
    //如果有剩余数据，则设置错误码；否则忽略
    template<class T, class S>
    __Myt & operator >>(const NS_IMPL::CManipulatorEnd<T, S> & m){
        if(good() && left())
            status(1);
        clearStub();
        return *this;
    }
    __Myt & operator >>(NS_IMPL::CManipulatorEnd<void, void> (*m)()){
        if(!m)
            return bad(1);
        return (*this>>m());
    }
    //输出可读字符串
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
        if(checkStub(cur() + sz)){
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
    //初始化
    //net:
    //  true    目标数据为网络字节序
    //  false   目标数据为本地字节序
    //reserve: 如果底层是容器，表示预留的缓冲区大小，可避免频繁分配内存
    explicit COutByteStreamBasic(size_t reserve = 1024, bool net = kByteOrderDefault)
        : __MyBase(net)
        , data_(reserve)
    {}
    //buf: 如果底层是指定的容器对象，表示该对象
    explicit COutByteStreamBasic(buffer_type & buf, bool net = kByteOrderDefault)
        : __MyBase(net)
        , data_(buf)
    {}
    //buf: 如果底层是定长缓冲区，表示缓冲区地址
    //sz: 如果底层是定长缓冲区，表示缓冲区大小
    COutByteStreamBasic(char_type * buf, size_t sz, bool net = kByteOrderDefault)
        : __MyBase(net)
        , data_(buf, sz)
    {}
    //设置错误码
    __Myt & bad(int code){
        status(code);
        return *this;
    }
    //设置cur指针的值
    //注意：如果cur变小，相当于抹掉了后面的数据；如果cur变大了，相当于留出指定的空间
    //return:
    //  <0      设置失败，并设置status为非0
    //  其他    设置后的cur()
    ssize_t seek(size_t pos){
        if(!checkStub(pos))
            return -1;
        if(data_.seek(pos))
            return cur();
        status(1);
        return -1;
    }
    //获取cur指针的值，即当前已写入的数据大小
    size_t cur() const{return data_.cur();}
    size_t size() const{return cur();}
    //修改cur指针的值为(cur + off)
    //fill: 填充字符
    //注意：如果cur变小，相当于抹掉了后面的数据；如果cur变大了，相当于留出指定的空间
    //return:
    //  <0      设置失败，并设置status为非0
    //  其他    设置后的cur()
    ssize_t skip(ssize_t off){
        if(!checkStub(cur() + off))
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
    //写入基本类型
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
    //写入定长数组(无长度字段)
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
    //写入变长数组(有长度字段)
    template<typename LenT, class T>
    __Myt & operator <<(const NS_IMPL::CManipulatorArrayPtr<LenT, T> & m){
        if(*this<<LenT(m.sz1_))
            writeRaw(m.c_, m.sz1_);
        return *this;
    }
    template<typename LenT, class T>
    __Myt & operator <<(const NS_IMPL::CManipulatorArrayCont<LenT, T> & m){
        const size_t off = cur();
        //NOTE: m.c_.size() may be O(N) complexity, e.g. std::list
        if((*this<<LenT(0)) && !m.c_.empty()){
            size_t sz = 0;
            if((*this<<Manip::raw(m.c_, &sz)))
                *this<<Manip::offset_value(off, LenT(sz));
        }
        return *this;
    }
    template<typename LenT, class ForwardIter>
    __Myt & operator <<(const NS_IMPL::CManipulatorArrayRange<LenT, ForwardIter> & m){
        const size_t off = cur();
        size_t sz = 0;
        if((*this<<LenT(0)<<Manip::raw(m.beg_, m.end_, &sz))){
            *this<<Manip::offset_value(off, LenT(sz));
            if(m.sz_)
                *m.sz_ = sz;
        }
        return *this;
    }
    __Myt & operator <<(const std::string & c){return (*this<<Manip::array(c));}
    __Myt & operator <<(const std::wstring & c){return (*this<<Manip::array(c));}
    //函数指针型操作符
    __Myt & operator <<(void (*m)(__MyBase &)){
        if(m)
            m(*this);
        return *this;
    }
    //指定字节序写入数据
    template<class T>
    __Myt & operator <<(const NS_IMPL::CManipulatorValueByteOrder<T> & m){
        const bool old = littleEndian();
        *this<<m.fun_<<m.c_;
        littleEndian(old);
        return *this;
    }
    //设置cur指针的值
    __Myt & operator <<(const NS_IMPL::CManipulatorSeek & m){
        seek(m.pos_);
        return *this;
    }
    //修改cur指针的值
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
    //在指定位置写入字段，不改变cur指针的值
    template<class T>
    __Myt & operator <<(const NS_IMPL::CManipulatorOffsetValue<T> & m){
        const size_t old = cur();
        if(seek(m.pos_) >= 0 && (*this<<m.c_))
            seek(old);
        return *this;
    }
    //在指定位置插入字段，后面数据相应后移
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
    //写入protobuf消息
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
    //使用Base 128 Varints遍码整数
    template<typename T>
    __Myt & operator <<(const NS_IMPL::CManipulatorVarint<T> & m){
        return writeVarint(m.toUnsigned());
    }
    //设置临时边界
    __Myt & operator <<(const NS_IMPL::CManipulatorStubPush & m){
        pushStub(m.sz_ + cur());
        return *this;
    }
    //撤除临时边界
    __Myt & operator <<(const NS_IMPL::CManipulatorStubPop & m){
        size_t pos;
        if(popStub(&pos)){
            if(m.check_ && cur() != pos)
                return bad(1);
            seek(m.align_ ? pos : cur());
        }
        return *this;
    }
    //调用对应的finish()
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
    //导出所有写入的数据，并清空自己
    bool finish(){
        if(good() && !data_.exportData())
            status(1);
        return clearStub();
    }
    //sz: 返回导出数据的字节长度
    template<typename SizeT>
    bool finish(SizeT * sz){
        if(good() && !data_.exportData(sz))
            status(1);
        return clearStub();
    }
    //dst: 将导出数据追加到dst已有数据后面
    bool finish(buffer_type & dst){
        if(good() && !data_.exportData(dst))
            status(1);
        return clearStub();
    }
    //dst: 使用导出数据覆盖dst已有的数据
    //sz: 输入表示dst的字节长度，返回表示导出数据的字节长度
    template<typename CharT>
    bool finish(CharT * dst, size_t * sz){
        if(good() && !data_.exportData(dst, sz))
            status(1);
        return clearStub();
    }
    //输出可读字符串
    std::string toString() const{
        CToString oss;
        oss<<"{CDataStreamBase="<<__MyBase::toString()
            <<", data_="<<data_.toString()
            <<"}";
        return oss.str();
    }
private:
    __Myt & operator <<(bool);  //prevent from abusing
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
        if(!checkStub(cur() + len))
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

