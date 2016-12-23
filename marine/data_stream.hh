/*
 * Copyright (c) 2016 Zhao DAI <daidodo@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or any
 * later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see accompanying file LICENSE.txt
 * or <http://www.gnu.org/licenses/>.
 */

/**
 * @file
 * @brief Data packing library using stream style interfaces.
 * @author Zhao DAI
 */

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

/**
 * @brief Manipulators for stream interface APIs.
 */
namespace Manip{

    /**
     * @name Pack/unpack a range of elements, without leading length field.
     * @{ */

    /**
     * @brief Pack/unpack an array with fixed size.
     * Sample code for CInByteStream:
     * @code{.cpp}
     * CInByteStream in(buf, sz);
     *
     * int c[5];    // an array to receive unpacked results
     *
     * in >> Mapip::raw(c);  // unpack 5 integers
     * // This is equivalent to:
     * // for(int i = 0;i < 5;++i)
     * //     in >> c[i];
     * @endcode
     * Sample code for COutByteStreamBasic:
     * @code{.cpp}
     * COutByteStream out;
     *
     * int c[5];    // an array to pack
     *
     * out << Mapip::raw(c);    // pack 5 integers
     * // This is equivalent to:
     * // for(int i = 0;i < 5;++i)
     * //     out << c[i];
     * @endcode
     * @param[inout] c An array
     */
    template<class T, size_t N>
    inline NS_IMPL::CManipulatorRawPtr<T> raw(T (&c)[N]){
        return NS_IMPL::CManipulatorRawPtr<T>(c, N);
    }

    /**
     * @brief Pack/unpack an array with variable size.
     * Sample code for CInByteStream:
     * @code{.cpp}
     * CInByteStream in(buf, sz);
     *
     * int * c = new int[sz];    // an array to receive unpacked results
     *
     * in >> Mapip::raw(c, sz);  // unpack 'sz' integers
     * // This is equivalent to:
     * // for(int i = 0;i < sz;++i)
     * //     in >> c[i];
     * @endcode
     * Sample code for COutByteStreamBasic:
     * @code{.cpp}
     * COutByteStream out;
     *
     * int * c = new int[sz];    // an array to pack
     *
     * out << Mapip::raw(c, sz);    // pack 'sz' integers
     * // This is equivalent to:
     * // for(int i = 0;i < sz;++i)
     * //     out << c[i];
     * @endcode
     * @param[inout] c Pointer to an array of elements
     * @param[in] sz Number of elements in @c c
     */
    template<class T>
    inline NS_IMPL::CManipulatorRawPtr<T> raw(T * c, size_t sz){
        return NS_IMPL::CManipulatorRawPtr<T>(c, sz);
    }

    /**
     * @brief Unpack elements and append to an @c std::vector.
     * Sample code for CInByteStream:
     * @code{.cpp}
     * CInByteStream in(buf, sz);
     *
     * vector<int> c;    // an object to receive unpacked results
     *
     * in >> Mapip::raw(c, sz);  // unpack 'sz' integers, and append them to 'c'
     * // This is equivalent to:
     * // for(int i = 0, v;i < sz;++i){
     * //     in >> v;
     * //     c.push_back(v);
     * // }
     * @endcode
     * @param[out] c Object to receive unpacked elements
     * @param[in] sz Number of elements to be unpacked
     */
    template<class T>
    inline NS_IMPL::CManipulatorRawPtr<T> raw(std::vector<T> & c, size_t sz){
        const size_t old = c.size();
        c.resize(old + sz);
        return NS_IMPL::CManipulatorRawPtr<T>(&c[old], sz);
    }

    /**
     * @brief Pack elements in an @c std::vector.
     * Sample code for COutByteStreamBasic:
     * @code{.cpp}
     * COutByteStream out;
     *
     * vector<int> c;    // an vector to pack
     *
     * out << Mapip::raw(c);    // pack elements in 'c'
     * // This is equivalent to:
     * // for(int i = 0;i < c.size();++i)
     * //     out << c[i];
     * @endcode
     * @param[in] c Object to pack
     * @param[out] sz If not @c NULL, receive the number of elements packed.
     */
    template<class T>
    inline NS_IMPL::CManipulatorRawPtr<const T> raw(const std::vector<T> & c, size_t * sz = NULL){
        if(sz)
            *sz = c.size();
        return NS_IMPL::CManipulatorRawPtr<const T>(&c[0], c.size());
    }

    /**
     * @brief Unpack a string.
     * The unpacked data will append to @c c.
     * Sample code for CInByteStream:
     * @code{.cpp}
     * CInByteStream in(buf, sz);
     *
     * string c;    // an object to receive unpacked data
     *
     * in >> Mapip::raw(c, len);  // unpack 'len' bytes of data, and append them to 'c'
     * // This is equivalent to:
     * // for(int i = 0;i < len;++i){
     * //     char v;
     * //     in >> v;
     * //     c.push_back(v);
     * // }
     * @endcode
     * @param c An object to receive unpacked data
     * @param len Size of bytes to unpack
     */
    template<typename Char>
    inline NS_IMPL::CManipulatorRawPtr<Char> raw(std::basic_string<Char> & c, size_t len){
        const size_t old = c.size();
        c.append(len, 0);
        return NS_IMPL::CManipulatorRawPtr<Char>(&c[old], len);
    }

    /**
     * @brief Pack a string.
     * Sample code for COutByteStreamBasic:
     * @code{.cpp}
     * COutByteStream out;
     *
     * string c;    // a string to pack
     *
     * out << Mapip::raw(c);    // pack the string
     * // This is equivalent to:
     * // for(int i = 0;i < c.size();++i)
     * //     out << c[i];
     * @endcode
     * @param[in] c Object to pack
     * @param[out] sz If not @c NULL, receive size of bytes packed.
     */
    template<typename Char>
    inline NS_IMPL::CManipulatorRawPtr<const Char> raw(const std::basic_string<Char> & c, size_t * sz = NULL){
        if(sz)
            *sz = c.size();
        return NS_IMPL::CManipulatorRawPtr<const Char>(c.c_str(), c.size());
    }

    //读取/写入迭代器范围[first, last)（无长度字段）
    /**
     * @brief Pack/unpack elements in a range of iterators.
     * Sample code for CInByteStream:
     * @code{.cpp}
     * CInByteStream in(buf, sz);
     *
     * list<int> c;
     * auto first = c.begin();  // start iterator
     * auto last = c.end();     // end iterator
     *
     * in >> Mapip::raw(first, last);  // unpack certain integers to range [first, last)
     * // This is equivalent to:
     * // for(auto it = first;it != last;++it)
     * //     in >> *it;
     * @endcode
     * Sample code for COutByteStreamBasic:
     * @code{.cpp}
     * COutByteStream out;
     *
     * list<int> c;
     * auto first = c.begin();  // start iterator
     * auto last = c.end();     // end iterator
     *
     * out << Mapip::raw(first, last);    // pack certain integers from range [first, last)
     * // This is equivalent to:
     * // for(auto it = first;it != last;++it)
     * //     out << *it;
     * @endcode
     * @param[inout] first Start iterator
     * @param[inout] last End iterator
     * @param[out] sz If not @c NULL, receive the number of elements packed/unpacked
     */
    template<class ForwardIter>
    inline NS_IMPL::CManipulatorRawRange<ForwardIter> raw(ForwardIter first, ForwardIter last, size_t * sz = NULL){
        return NS_IMPL::CManipulatorRawRange<ForwardIter>(first, last, sz);
    }

    /**
     * @brief Unpack elements and append to a container.
     * Sample code for CInByteStream:
     * @code{.cpp}
     * CInByteStream in(buf, sz);
     *
     * deque<int> c;    // a container to receive unpacked elements
     *
     * in >> Mapip::raw(c, sz);  // unpack 'sz' integers, and append them to 'c'
     * // This is equivalent to:
     * // for(int i = 0, v;i < sz;++i){
     * //     in >> v;
     * //     c.push_back(v);
     * // }
     * @endcode
     * @param c A container to receive unpacked elements
     * @param sz Number of elements to unpack
     */
    template<class T>
    inline NS_IMPL::CManipulatorRawCont<T> raw(T & c, size_t sz){
        return NS_IMPL::CManipulatorRawCont<T>(c, sz, NULL);
    }

    template<class T>
    inline NS_IMPL::CManipulatorRawCont<const T> raw(const T & c, size_t * sz = NULL){
        return NS_IMPL::CManipulatorRawCont<const T>(c, 0, sz);
    }
    /**  @} */

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

/**
 * @brief Data unpacking interfaces.
 * CInByteStream is super convenient for unpacking data. It receives a byte buffer containing
 * serialized data, and tries to decode whatever you want from it.
 * @n CInByteStream manages data [endianness](https://en.wikipedia.org/wiki/Endianness)
 * automatically, so you don't need to call @c hton/ntoh family APIs manually.It also performs
 * sufficient boundary checks so you won't end up with memory access violations.
 * @n What makes CInByteStream really powerful is that it integrates a number of manipulators,
 * which makes unpacking complex structures a joy of life.
 * @n Sample code:
 * @code{.cpp}
 * CInByteStream in(buf, sz);   // initialize from a byte buffer
 *
 * // Following are the data we want to unpack
 * uint32_t version;        // a 32-bit integer
 * string name;             // a string
 * vector<uint64_t> orders; // an array of 64-bit integers
 *
 * // Then we extract all data in ONE line!
 * in >> version >> name >> Manip::array(orders);
 * @endcode
 */
class CInByteStream : public NS_IMPL::CDataStreamBase
{
    typedef NS_IMPL::CDataStreamBase __MyBase;
    typedef CInByteStream __Myt;
    const char *    data_;
    size_t          len_;
    size_t          cur_;
public:
    /**
     * @brief Construct an empty object.
     * You need to call @ref setSource before you can use this object to unpack data.
     * @sa setSource
     */
    CInByteStream()
        : __MyBase(kByteOrderDefault, 1)
        , data_(NULL)
        , len_(0)
        , cur_(0)
    {}
    /**
     * @brief Construct from a byte buffer.
     * @param buf Pointer to a byte buffer containing data to be unpacked
     * @param sz Byte size of @c buf, if present
     * @param net Byte order of the data in @c buf:
     *   @li @c true: Net byte order, this is the default;
     *   @li @c false: Host byte order;
     */
    CInByteStream(const char * buf, size_t sz, bool net = kByteOrderDefault){
        setSource(buf, sz, net);
    }
    /**
     * @brief Construct from a byte buffer.
     * @param buf Pointer to a byte buffer containing data to be unpacked
     * @param sz Byte size of @c buf, if present
     * @param net Byte order of the data in @c buf:
     *   @li @c true: Net byte order, this is the default;
     *   @li @c false: Host byte order;
     */
    CInByteStream(const unsigned char * buf, size_t sz, bool net = kByteOrderDefault){
        setSource(buf, sz, net);
    }
    /**
     * @brief Construct from a byte buffer.
     * @param buf Pointer to a byte buffer containing data to be unpacked
     * @param sz Byte size of @c buf
     * @param net Byte order of the data in @c buf:
     *   @li @c true: Net byte order, this is the default;
     *   @li @c false: Host byte order;
     */
    CInByteStream(const signed char * buf, size_t sz, bool net = kByteOrderDefault){
        setSource(buf, sz, net);
    }
    /**
     * @brief Construct from a byte buffer.
     * @param buf A byte buffer containing data to be unpacked
     * @param net Byte order of the data in @c buf:
     *   @li @c true: Net byte order, this is the default;
     *   @li @c false: Host byte order;
     */
    CInByteStream(const std::string & buf, bool net = kByteOrderDefault){
        setSource(buf, net);
    }
    /**
     * @brief Initialize with a byte buffer.
     * This function will also reset current pointer and status.
     * @param buf Pointer to a byte buffer containing data to be unpacked
     * @param sz Byte size of @c buf, if present
     * @param net Byte order of the data in @c buf:
     *   @li @c true: Net byte order, this is the default;
     *   @li @c false: Host byte order;
     */
    void setSource(const char * buf, size_t sz, bool net = kByteOrderDefault){
        data_ = buf;
        len_ = sz;
        cur_ = 0;
        netByteOrder(net);
        status(0);
    }
    /**
     * @brief Initialize with a byte buffer.
     * This function will also reset current pointer and status.
     * @param buf Pointer to a byte buffer containing data to be unpacked
     * @param sz Byte size of @c buf, if present
     * @param net Byte order of the data in @c buf:
     *   @li @c true: Net byte order, this is the default;
     *   @li @c false: Host byte order;
     */
    void setSource(const unsigned char * buf, size_t sz, bool net = kByteOrderDefault){
        setSource(reinterpret_cast<const char *>(buf), sz, net);
    }
    /**
     * @brief Initialize with a byte buffer.
     * This function will also reset *current pointer* (current reading position) and @a status.
     * @param buf Pointer to a byte buffer containing data to be unpacked
     * @param sz Byte size of @c buf, if present
     * @param net Byte order of the data in @c buf:
     *   @li @c true: Net byte order, this is the default;
     *   @li @c false: Host byte order;
     */
    void setSource(const signed char * buf, size_t sz, bool net = kByteOrderDefault){
        setSource(reinterpret_cast<const char *>(buf), sz, net);
    }
    /**
     * @brief Initialize with a byte buffer.
     * @param buf A byte buffer containing data to be unpacked
     * @param net Byte order of the data in @c buf:
     *   @li @c true: Net byte order, this is the default;
     *   @li @c false: Host byte order;
     */
    void setSource(const std::string & buf, bool net = kByteOrderDefault){
        setSource(buf.c_str(), buf.size(), net);
    }
    /**
     * @brief Set @a status.
     * If @a status is not @c 0, all unpacking operations will fail.
     * @param code
     *   @li @c 0: Reset status.
     *   @li Otherwise: A number indicating error status.
     * @return Reference of self
     */
    __Myt & bad(int code){
        status(code);
        return *this;
    }
    /**
     * @brief Set *current pointer*.
     * If this function makes *current pointer* smaller, old data will be read again; And if
     * *current pointer* becomes larger, some data will be skipped.
     * @param pos New current pointer
     * @return
     *   @li Negative: Operation failed, *status* will be set to a nonzero value;
     *   @li Otherwise: New *current pointer*;
     */
    ssize_t seek(size_t pos){
        if(!checkStub(pos))
            return -1;
        if(pos > len_){
            status(1);
            return -1;
        }
        return (cur_ = pos);
    }
    /**
     * @brief Skip some data.
     * This function is equivalent to `seek(cur() + off)`.
     * @param off Bytes size of data to skip.
     * @return
     *   @li Negative: Operation failed, *status* will be set to a nonzero value;
     *   @li Otherwise: New *current pointer*;
     * @note You can also give a negative @c off to jump back to old data.
     */
    ssize_t skip(ssize_t off){return seek(cur() + off);}
    /**
     * @brief Get *current pointer*.
     * *Current pointer* is the index of underlying data where next unpacking operation will take
     * place. Every unpacking operation increases *current pointer* by the amount of data it reads.
     * @return Current pointer
     */
    size_t cur() const{return cur_;}
    /**
     * @brief Get a pointer to @em left data.
     * This function returns a pointer to the first unread byte, which is also indicated by
     * *current pointer*.
     * @return A pointer to the unread data
     * @sa cur
     */
    const char * data() const{return (data_ + cur());}
    /**
     * @brief Get byte size of left data.
     * @return Bytes size of left data
     */
    size_t left() const{return (getStub(len_) - cur());}
    /**
     * @brief Get a readable description of this object.
     * @return A readable description of this object
     */
    std::string toString() const{
        CToString oss;
        oss<<"{CDataStreamBase="<<__MyBase::toString()
            <<", cur_="<<cur_
            <<", data_="<<NS_IMPL::dumpBufPart(data_, len_, cur_)
            <<"}";
        return oss.str();
    }
    /**
     * @name Unpack primitive types
     * @{
     * These functions read `sizeof c` bytes from underlying data, and store result in @c c, with
     * appropriate byte order transformation.
     * @param[out] c An integer to receive the result
     * @return Reference to self
     */
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
    /**  @} */
    /**
     * @name Unpack compound types
     * @{ */
    /**
     * @brief Unpack a string.
     * This function first reads a @c uint16_t as the length of the result string, then reads the
     * whole string.
     * @param[out] c A string to receive the result
     * @return Reference to self
     */
    __Myt & operator >>(std::string & c){return (*this>>Manip::array(c));}
    /**
     * @brief Unpack a wide string.
     * This function first reads a @c uint16_t as the length of the result wide string, then reads
     * the whole wide string.
     * @param[out] c A wide string to receive the result
     * @return Reference to self
     */
    __Myt & operator >>(std::wstring & c){return (*this>>Manip::array(c));}
    /**
     * @brief Unpack an array.
     * This function reads @c N elements directly (without leading length field), and stores them
     * in @c c.
     * @n @c T must be a type supported by `operator >>` of CInByteStream.
     * @tparam T Type of element in array
     * @tparam N Number of elements in array
     * @param[out] c An array to receive the result
     * @return Reference to self
     */
    template<class T, size_t N>
    __Myt & operator >>(T (&c)[N]){return readRaw(c, N);}
    /**  @} */
    /**
     * @name Manipulators
     * @{
     * These functions operates on different manipulators to achieve special purpose.
     * Please see the corresponding manipulator generators in @ref Manip for more information.
     * @param m Manipulator object
     * @return Reference of self
     */
    __Myt & operator >>(void (*m)(__MyBase &)){
        if(m)
            m(*this);
        return *this;
    }
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
    template<class T>
    __Myt & operator >>(const NS_IMPL::CManipulatorValueByteOrder<T> & m){
        const bool old = littleEndian();
        *this>>m.fun_>>m.c_;
        littleEndian(old);
        return *this;
    }
    __Myt & operator >>(const NS_IMPL::CManipulatorSeek & m){
        seek(m.pos_);
        return *this;
    }
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
    template<class T>
    __Myt & operator >>(const NS_IMPL::CManipulatorOffsetValue<T> & m){
        const size_t old = cur();
        if(seek(m.pos_) >= 0 && (*this>>m.c_))
            seek(old);
        return *this;
    }
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
    template<typename T>
    __Myt & operator >>(const NS_IMPL::CManipulatorVarint<T> & m){
        typename NS_IMPL::CManipulatorVarint<T>::__Unsigned v = 0;
        if(readVarint(v))
            m.fromUnsigned(v);
        return *this;
    }
    __Myt & operator >>(const NS_IMPL::CManipulatorStubPush & m){
        pushStub(m.sz_ + cur());
        return *this;
    }
    __Myt & operator >>(const NS_IMPL::CManipulatorStubPop & m){
        size_t pos;
        if(popStub(&pos)){
            if(m.check_ && cur() != pos)
                return bad(1);
            seek(m.align_ ? pos : cur());
        }
        return *this;
    }
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
    /**  @} */
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

/**
 * @brief Data packing interfaces.
 * COutByteStreamBasic is for easy data packing/encoding/serialization. It manages data [endianness]
 * (https://en.wikipedia.org/wiki/Endianness) automatically, so you don't need to call @c hton/ntoh
 * family APIs manually. It also performs sufficient boundary checks so you won't end up with memory
 * access violations.
 * @n COutByteStream supports a number of manipulators to operate on compound types and complex
 * structures, which will make your code really concise and efficient.
 * @n The type of underlying byte buffer determines which variant of COutByteStreamBasic you may
 * choose:
 * @li Internal @c std::string
 *
 * @c COutByteStream (or @c COutByteStreamStr) uses internal @c std::string as the underlying byte
 * buffer, which means you don't need to provide a byte buffer when constructing a @c COutByteStream
 * object, instead you could indicate a byte size to the internal @c std::string object to reserve,
 * so it might not need to expand (and copy data) later on.
 * @li External @c std::string
 *
 * In the contrary, @c COutByteStreamStrRef needs an external @c std::string object to store data,
 * provided in the constructor. @c COutByteStreamStrRef will hold a reference to the @c std::string
 * object and manipulate it during the process of serialization/encoding. New data will append to
 * the @c std::string object, so it's possible to deal with some legacy code and avoid performance
 * loss.
 * @li Internal @c std::vector<char>
 *
 * Similar to @c COutByteStream, @c COutByteStreamVec uses internal @c std::vector<char> as the
 * underlying buffer. You can treat them quite the same, and the purpose of @c COutByteStreamVec is
 * only for completion: some people prefer using @c std::vector<char> as a buffer.
 * @li External @c std::vector<char>
 *
 * @c COutByteStreamVecRef needs an external @c std::vector<char> object to construct, just like @c
 * COutByteStreamStrRef does. And every aspect is just the same for both of them, as described
 * before.
 * @li External @c char array
 *
 * @c COutByteStreamBuf is different from all above. It receives a @c char array as the underlying
 * buffer, with fixed size, wrapped by CCharBuffer. It's impossible to @a expand the underlying
 * buffer, and when it's full, all serialization operations will fail and status is set.
 * @n Here is an example use of @c COutByteStream:
 * @code{.cpp}
 * COutByteStream out;   // using internal std::string as data buffer
 *
 * // Following are the data we want to pack
 * uint32_t version;        // a 32-bit integer
 * string name;             // a string
 * vector<uint64_t> orders; // an array of 64-bit integers
 *
 * // This is to receive the final data buffer.
 * std::string buf;
 *
 * // Then we serialize all data in ONE line!
 * out << version << name << Manip::array(orders)
 *     << Manip::end(buf);  // And export the data to 'buf'
 * @endcode
 * Other variants are similar, except for the constructor and @ref Manip:end part.
 * @tparam Data Type of underlying byte buffer
 * @note COutByteStreamBasic is not intended for direct use, try variants introduced above.
 */
template<class Data>
class COutByteStreamBasic : public NS_IMPL::CDataStreamBase
{
    typedef NS_IMPL::CDataStreamBase    __MyBase;
    typedef COutByteStreamBasic<Data>   __Myt;
    typedef Data                        __Data;
public:
    typedef typename Data::__Buf        buffer_type;
    typedef typename Data::__Char       char_type;
    /**
     * @brief Constructor for COutByteStream/COutByteStreamStr, COutByteStreamVec.
     * @param reserve Byte size for the underlying buffer to reserve.
     * @param net Byte order of the data in the underlying buffer:
     *   @li @c true: Net byte order, this is the default;
     *   @li @c false: Host byte order;
     */
    explicit COutByteStreamBasic(size_t reserve = 1024, bool net = kByteOrderDefault)
        : __MyBase(net)
        , data_(reserve)
    {}
    /**
     * @brief Constructor for COutByteStreamStrRef, COutByteStreamVecRef.
     * @param buf Reference to external byte buffer
     * @param net Byte order of the data in the underlying buffer:
     *   @li @c true: Net byte order, this is the default;
     *   @li @c false: Host byte order;
     */
    explicit COutByteStreamBasic(buffer_type & buf, bool net = kByteOrderDefault)
        : __MyBase(net)
        , data_(buf)
    {}
    /**
     * @brief Constructor for COutByteStreamBuf.
     * @param buf Pointer to external byte buffer
     * @param sz Length of @c buf
     * @param net Byte order of the data in the underlying buffer:
     *   @li @c true: Net byte order, this is the default;
     *   @li @c false: Host byte order;
     */
    COutByteStreamBasic(char_type * buf, size_t sz, bool net = kByteOrderDefault)
        : __MyBase(net)
        , data_(buf, sz)
    {}
    /**
     * @brief Set @a status.
     * If @a status is not @c 0, all packing operations will fail.
     * @param code
     *   @li @c 0: Reset status.
     *   @li Otherwise: A number indicating error status.
     * @return Reference of self
     */
    __Myt & bad(int code){
        status(code);
        return *this;
    }
    /**
     * @brief Set *current pointer*.
     * If this function makes *current pointer* smaller, some data are erased; And if
     * *current pointer* becomes larger, some room are reserved.
     * @param pos New current pointer
     * @return
     *   @li Negative: Operation failed, *status* will be set to a nonzero value;
     *   @li Otherwise: New *current pointer*;
     */
    ssize_t seek(size_t pos){
        if(!checkStub(pos))
            return -1;
        if(data_.seek(pos))
            return cur();
        status(1);
        return -1;
    }
    /**
     * @brief Get *current pointer*.
     * *Current pointer* is the size of data that have been written to the underlying byte buffer.
     * Every packing operation increases *current pointer* by the amount of data it writes.
     * @return Current pointer
     * @sa size
     */
    size_t cur() const{return data_.cur();}
    /**
     * @brief Get byte size of data have been written, same as @ref cur.
     * @return Byte size written data
     * @sa cur
     */
    size_t size() const{return cur();}
    /**
     * @brief Skip some bytes.
     * This function is equivalent to `seek(cur() + off)`. If `off > 0`, some room are reserved; if
     * `off < 0`, some data are erased.
     * @n The content of the underlying buffer will @em NOT be modified by this function.
     * @param off Size of bytes to skip.
     * @return
     *   @li Negative: Operation failed, *status* will be set to a nonzero value;
     *   @li Otherwise: New *current pointer*;
     */
    ssize_t skip(ssize_t off){
        if(!checkStub(cur() + off))
            return -1;
        if(off < 0 && size_t(-off) > cur()){
            bad(1);
            return -1;
        }
        return seek(cur() + off);
    }
    /**
     * @brief Skip some bytes.
     * This function is similar to `seek(cur() + off)`. If `off > 0`, some room are reserved, and
     * @c fill are used to fill in the room; if `off < 0`, some data are erased.
     * @param off Size of bytes to skip.
     * @param fill Character to fill in the reserved room, if any
     * @return
     *   @li Negative: Operation failed, *status* will be set to a nonzero value;
     *   @li Otherwise: New *current pointer*;
     */
    ssize_t skip(ssize_t off, int fill){
        const size_t old = cur();
        const ssize_t ret = skip(off);
        if(ret < 0)
            return ret;
        if(old < size_t(ret))
            std::fill(data_.data(old), data_.data(ret), fill);
        return ret;
    }
    /**
     * @brief Get a readable description of this object.
     * @return A readable description of this object
     */
    std::string toString() const{
        CToString oss;
        oss<<"{CDataStreamBase="<<__MyBase::toString()
            <<", data_="<<data_.toString()
            <<"}";
        return oss.str();
    }
    /**
     * @name Pack primitive types
     * @{
     * These functions write @c c to the underlying buffer, and extend it by `sizeof c` bytes, with
     * appropriate byte order transformation.
     * @param c An integer to be packed
     * @return Reference to self
     */
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
    /**  @} */
    /**
     * @name Pack compound types
     * @{ */
    /**
     * @brief Pack a string.
     * This function first writes a @c uint16_t equal to the length of @c c, then copy the
     * whole string to the underlying buffer.
     * @param c A string to be packed
     * @return Reference to self
     */
    __Myt & operator <<(const std::string & c){return (*this<<Manip::array(c));}
    /**
     * @brief Pack a wide string.
     * This function first writes a @c uint16_t equal to the length of @c c, then copy the
     * whole string to the underlying buffer.
     * @param c A wide string to be packed
     * @return Reference to self
     */
    __Myt & operator <<(const std::wstring & c){return (*this<<Manip::array(c));}
    /**
     * @brief Pack an array.
     * This function writes @c N elements directly (without leading length field) to the underlying
     * buffer.
     * @n @c T must be a type supported by `operator <<` of COutByteStreamBasic.
     * @tparam T Type of element in array
     * @tparam N Number of elements in array
     * @param c An array to be packed
     * @return Reference to self
     */
    template<class T, size_t N>
    __Myt & operator <<(const T (&c)[N]){return writeRaw(c, N);}
    /**  @} */
    /**
     * @name Manipulators
     * @{
     * These functions operates on different manipulators to achieve special purpose.
     * Please see the corresponding manipulator generators in @ref Manip for more information.
     * @param m Manipulator object
     * @return Reference of self
     */
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
    __Myt & operator <<(void (*m)(__MyBase &)){
        if(m)
            m(*this);
        return *this;
    }
    template<class T>
    __Myt & operator <<(const NS_IMPL::CManipulatorValueByteOrder<T> & m){
        const bool old = littleEndian();
        *this<<m.fun_<<m.c_;
        littleEndian(old);
        return *this;
    }
    __Myt & operator <<(const NS_IMPL::CManipulatorSeek & m){
        seek(m.pos_);
        return *this;
    }
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
    template<class T>
    __Myt & operator <<(const NS_IMPL::CManipulatorOffsetValue<T> & m){
        const size_t old = cur();
        if(seek(m.pos_) >= 0 && (*this<<m.c_))
            seek(old);
        return *this;
    }
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
    template<typename T>
    __Myt & operator <<(const NS_IMPL::CManipulatorVarint<T> & m){
        return writeVarint(m.toUnsigned());
    }
    __Myt & operator <<(const NS_IMPL::CManipulatorStubPush & m){
        pushStub(m.sz_ + cur());
        return *this;
    }
    __Myt & operator <<(const NS_IMPL::CManipulatorStubPop & m){
        size_t pos;
        if(popStub(&pos)){
            if(m.check_ && cur() != pos)
                return bad(1);
            seek(m.align_ ? pos : cur());
        }
        return *this;
    }
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
    /**  @} */
    /**
     * @brief Export data for COutByteStreamStrRef, COutByteStreamVecRef.
     * Although COutByteStreamStrRef and COutByteStreamVecRef use external byte buffer, it's still
     * necessary to call this function or use @ref Manip::end() explicitly, to do some finishing
     * work, e.g. correcting the size of the byte buffer object.
     * @return @c true if succeeded; otherwise @c false
     */
    bool finish(){
        if(good() && !data_.exportData())
            status(1);
        return clearStub();
    }
    /**
     * @brief Export data for COutByteStreamBuf, COutByteStreamStrRef, COutByteStreamVecRef.
     * This function does some finishing work, e.g. correcting the size of the underlying buffer,
     * and return its real size to @c sz.
     * @n The returned size may be different to @c size(), in case that there were data already in
     * the external buffer before serialization.
     * @tparam SizeT An integer type
     * @param[out] sz Pointer to an integer to receive the size of the data
     * @return @c true if succeeded; otherwise @c false
     */
    template<typename SizeT>
    bool finish(SizeT * sz){
        if(good() && !data_.exportData(sz))
            status(1);
        return clearStub();
    }
    /**
     * @brief Export data for COutByteStream/COutByteStreamStr, COutByteStreamStrRef,
     * COutByteStreamVec, COutByteStreamVecRef.
     * This function appends the underlying data to @c dst.
     * @n For COutByteStream and COutByteStreamVec, if @c dst is empty before exporting, @c swap
     * will be used to avoid data copy.
     * @param[out] dst A buffer to receive data
     * @return @c true if succeeded; otherwise @c false
     */
    bool finish(buffer_type & dst){
        if(good() && !data_.exportData(dst))
            status(1);
        return clearStub();
    }
    /**
     * @brief Export data for COutByteStream/COutByteStreamStr, COutByteStreamStrRef,
     * COutByteStreamVec, COutByteStreamVecRef, COutByteStreamBuf.
     * This function copies the underlying data to a byte buffer pointed by @c dst, and returns the
     * real size of data to @c sz.
     * @tparam CharT Must be the same as the underlying character type, e.g. @c char
     * @param[out] dst Pointer to a byte array to receive the data
     * @param[inout] sz Pass in as the length of @c dst; pass out as the real size of exported data
     * @return @c true if succeeded; otherwise @c false
     */
    template<typename CharT>
    bool finish(CharT * dst, size_t * sz){
        if(good() && !data_.exportData(dst, sz))
            status(1);
        return clearStub();
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

/** @brief Use internal @c std::string as the underlying data buffer. */
typedef COutByteStreamBasic<NS_IMPL::__buf_data<std::string> > COutByteStreamStr;

/** @brief Same as COutByteStreamStr */
typedef COutByteStreamStr COutByteStream;

/** @brief Use external @c std::string as the underlying data buffer. */
typedef COutByteStreamBasic<NS_IMPL::__buf_ref_data<std::string> > COutByteStreamStrRef;

/** @brief Use internal @c std::vector<char> as the underlying data buffer. */
typedef COutByteStreamBasic<NS_IMPL::__buf_data<std::vector<char> > > COutByteStreamVec;

/** @brief Use external @c std::vector<char> as the underlying data buffer. */
typedef COutByteStreamBasic<NS_IMPL::__buf_ref_data<std::vector<char> > > COutByteStreamVecRef;

/** @brief Use external @c char array as the underlying data buffer. */
typedef COutByteStreamBasic<NS_IMPL::__buf_data<CCharBuffer<char> > > COutByteStreamBuf;

NS_SERVER_END

#endif

