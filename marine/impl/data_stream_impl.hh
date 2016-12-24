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
 * @brief Implementation of CInByteStream, COutByteStreamBasic.
 * @warning This file is @em NOT intended for direct use.
 * @author Zhao DAI
 */

#ifndef DOZERG_DATA_STREAM_IMPL_H_20081016
#define DOZERG_DATA_STREAM_IMPL_H_20081016

#include <string>
#include "../to_string.hh"
#include "../char_buffer.hh"
#include "../tools/net.hh"       //tools::HostByteOrder
#include "../tools/debug.hh"     //tools::DumpXXX
#include "../template.hh"

NS_IMPL_BEGIN

/**
 * @brief Base class of CInByteStream, COutByteStreamBasic.
 * @warning This class is @em NOT intended for direct use.
 */
class CDataStreamBase
{
    typedef bool (CDataStreamBase::*__SafeBool)() const;
public:
    /** Byte order of the underlying data buffer, default Net Byte Order. */
    static const bool kByteOrderDefault = true;
#ifndef DOX_UNDOCUMENT_FLAG
    explicit CDataStreamBase(bool netByteOrder = kByteOrderDefault, int code = 0)
        : status_(code)
        , hostByteOrder_(tools::HostByteOrder())
        , dataByteOrder_(bo(netByteOrder))
    {}
#endif
    /**
     * @brief Set error status.
     * If status is @em NOT 0, all packing/unpacking operations will fail.
     * @param code New status
     */
    void status(int code){status_ = code;}
    /**
     * @brief Get error status.
     * @return Current status
     */
    int status() const{return status_;}
    /**
     * @brief Test if current error status is @em NOT 0.
     * Sample code:
     * @code{.cpp}
     * CInByteStream in(buf);
     *
     * if(!in){
     *     // 'in' is in error status, something is wrong
     * }
     * @endcode
     * @return
     *   @li @c true: Status is @em NOT 0
     *   @li @c false: Status is 0
     */
    bool operator !() const{return !good();}
    /**
     * @brief Test if current error status is 0, same as @ref good().
     * Sample code:
     * @code{.cpp}
     * CInByteStream in(buf);
     *
     * if(in){
     *     // 'in' is in good status
     * }
     * @endcode
     * @return
     *   @li @c true: Status is 0
     *   @li @c false: Status is @em NOT 0
     * @sa good()
     */
    operator __SafeBool() const{return (good() ? &CDataStreamBase::good : NULL);}
    /**
     * @brief Test if current error status is 0, same as `operator __SafeBool()`.
     * Sample code:
     * @code{.cpp}
     * CInByteStream in(buf);
     *
     * if(in.good(){
     *     // 'in' is in good status
     * }
     * @endcode
     * @return
     *   @li @c true: Status is 0
     *   @li @c false: Status is @em NOT 0
     */
    bool good() const{return (0 == status_);}
    /**
     * @brief Set byte order.
     * @param nb
     *   @li @c true: Net Byte Order (Little Endian)
     *   @li @c false: Host Byte Order
     */
    void netByteOrder(bool nb){dataByteOrder_ = bo(nb);}
    /**
     * @brief Set byte order.
     * @param endian
     *   @li @c true: Little endian (Net Byte Order)
     *   @li @c false: Big endian
     */
    void littleEndian(bool endian){dataByteOrder_ = endian;}
    //return:
    //  true    little endian
    //  false   big endian(net byte order)
    /**
     * @brief Get byte order.
     * @return
     *   @li @c true: Little endian
     *   @li @c false: Big endian
     */
    bool littleEndian() const{return dataByteOrder_;}
#ifndef DOX_UNDOCUMENT_FLAG
    std::string toString() const{
        CToString oss;
        oss<<"{status_="<<status_
            <<", hostByteOrder_="<<(hostByteOrder_ ? "LittleEndian" : "BigEndian")
            <<", dataByteOrder_="<<(dataByteOrder_ ? "LittleEndian" : "BigEndian");
        if(!stubs_.empty()){
            oss<<", stubs_={";
            for(size_t i = 0;i < stubs_.size();++i){
                if(i)
                    oss<<", ";
                oss<<stubs_[i];
            }
            oss<<'}';
        }
        oss<<'}';
        return oss.str();
    }
protected:
    bool needReverse() const{return (hostByteOrder_ != dataByteOrder_);}
    void pushStub(size_t pos){
        if(good())
            stubs_.push_back(pos);
    }
    bool popStub(size_t * pos){
        if(!good())
            return false;
        if(stubs_.empty()){
            status(1);
            return false;
        }
        if(pos)
            *pos = stubs_.back();
        stubs_.pop_back();
        return true;
    }
    bool checkStub(size_t pos){
        if(!good())
            return false;
        if(stubs_.empty())
            return true;
        if(pos > stubs_.back()){
            status(1);
            return false;
        }
        return true;
    }
    size_t getStub(size_t end) const{
        if(stubs_.empty())
            return end;
        return std::min(end, stubs_.back());
    }
    bool clearStub(){
        if(!good())
            return false;
        stubs_.clear();
        return true;
    }
#endif
private:
    bool bo(bool net){return (net ? false : hostByteOrder_);}
    //fields
    int status_;                // Error status
    const bool hostByteOrder_;  // Byte order of the environment:
                                //     true: little endian
                                //     false: big endian
    bool dataByteOrder_;        // Byte order of the data:
                                //     true: little endian
                                //     false: big endian
    std::vector<size_t> stubs_;
};

#ifndef DOX_UNDOCUMENT_FLAG

template<typename Char>
inline void __buf_copy(Char * dst, const Char * src, size_t sz)
{
    typedef std::char_traits<Char> __Traits;
    assert(sz);
    if(1 == sz)
        __Traits::assign(*dst, *src);
    else
        __Traits::copy(dst, src, sz);
}

template<typename Char>
inline void __buf_move(Char * dst, const Char * src, size_t sz)
{
    typedef std::char_traits<Char> __Traits;
    assert(sz);
    if(1 == sz)
        __Traits::assign(*dst, *src);
    else
        __Traits::move(dst, src, sz);
}

inline std::string dumpBufPart(const char * buf, size_t sz, size_t part)
{
    if(!buf)
        return std::string("(NULL)");
    CToString oss;
    oss<<'('<<sz<<')'
        <<tools::DumpHex(buf, part, ' ', false)
        <<" | ";
    if(sz > part)
        oss<<tools::DumpHex(buf +  part, sz - part, ' ', false);
    return oss.str();
}

template<class Buf>
class __buf_ref_data
{
public:
    typedef Buf __Buf;
    typedef typename __Buf::value_type __Char;
private:
    typedef __buf_ref_data<__Buf> __Myt;
public:
    explicit __buf_ref_data(__Buf & buf)
        : buf_(buf)
        , begin_(buf.size())
        , cur_(0)
    {}
    __buf_ref_data(__Buf & buf, size_t begin)
        : buf_(buf)
        , begin_(begin)
        , cur_(0)
    {}
    size_t cur() const{return cur_;}
    __Char * data(size_t i){return &buf_[offset(i)];}
    bool ensure(size_t len){
        const size_t old = buf_.size();
        if(offset(cur_ + len) > old)
            buf_.resize(old + (old >> 1) + len);
        return true;
    }
    bool seek(size_t pos){
        if(pos > cur_ && !ensure(pos - cur_))
            return false;
        cur_ = pos;
        return true;
    }
    void append(const __Char * buf, size_t sz){
        insert(cur_, buf, sz);
    }
    void append(__Char c){
        assert(offset(cur_ + 1) <= buf_.size());
        *data(cur_++) = c;
    }
    void insert(size_t off, const __Char * buf, size_t sz){
        assert(off <= cur_);
        assert(sz && offset(cur_ + sz) <= buf_.size());
        assert(buf);
        const size_t c = cur_ - off;
        if(c)
            __buf_move(&buf_[offset(off + sz)], data(off), c);
        __buf_copy(data(off), buf, sz);
        cur_ += sz;
    }
    bool exportData(){
        buf_.resize(offset(cur_));
        begin_ = cur_ = 0;
        return true;
    }
    template<typename SizeT>
    bool exportData(SizeT * sz){
        if(!exportData())
            return false;
        if(sz)
            *sz = buf_.size();
        return true;
    }
    bool exportData(__Buf & buf){
        if(!exportData())
            return false;
        const size_t old = buf.size();
        buf.resize(old + buf_.size());
        __buf_copy(&buf[old], &buf_[0], buf_.size());
        return true;
    }
    template<typename CharT>
    bool exportData(CharT * buf, size_t * sz){
        if(!buf || !sz || *sz < offset(cur_) || !exportData())
            return false;
        __buf_copy(buf, &buf_[0], buf_.size());
        *sz = buf_.size();
        return true;
    }
    std::string toString() const{
        CToString oss;
        oss<<"{begin_="<<begin_
            <<", buf_="<<dumpBufPart(&buf_[0], begin_ + cur_, begin_)
            <<"}";
        return oss.str();
    }
private:
    size_t offset(size_t i) const{return begin_ + i;}
    //members
    __Buf & buf_;
    size_t begin_;
    size_t cur_;
};

template<class Buf>
class __buf_data
{
public:
    typedef Buf __Buf;
    typedef typename __Buf::value_type __Char;
private:
    typedef __buf_ref_data<__Buf> __Ref;
    typedef __buf_data<__Buf> __Myt;
public:
    explicit __buf_data(size_t reserve)
        : buf_(reserve, 0)
        , ref_(buf_, 0)
    {}
    size_t cur() const{return ref_.cur();}
    __Char * data(size_t i){return ref_.data(i);}
    bool ensure(size_t len){return ref_.ensure(len);}
    bool seek(size_t pos){return ref_.seek(pos);}
    void append(const __Char * buf, size_t sz){ref_.append(buf, sz);}
    void append(__Char c){ref_.append(c);}
    void insert(size_t offset, const __Char * buf, size_t sz){ref_.insert(offset, buf, sz);}
    bool exportData(__Buf & buf){
        ref_.exportData();
        if(buf.empty()){
            buf.swap(buf_);
        }else{
            const size_t old = buf.size();
            buf.resize(old + buf_.size());
            __buf_copy(&buf[old], data(0), buf_.size());
        }
        return true;
    }
    template<typename CharT>
    bool exportData(CharT * buf, size_t * sz){
        if(!buf || !sz || *sz < cur() || !ref_.exportData())
            return false;
        __buf_copy(buf, data(0), buf_.size());
        *sz = buf_.size();
        return true;
    }
    std::string toString() const{return ref_.toString();}
private:
    //members
    __Buf buf_;
    __Ref ref_;
};

template<typename Char>
class __buf_data<NS_SERVER::CCharBuffer<Char> >
{
public:
    typedef CCharBuffer<Char> __Buf;
    typedef typename __Buf::value_type __Char;
private:
    typedef __buf_data<__Buf> __Myt;
public:
    explicit __buf_data(__Char * buf, size_t sz)
        : buf_(buf, sz, sz)
        , cur_(0)
    {}
    size_t cur() const{return cur_;}
    __Char * data(size_t i){return &buf_[i];}
    const __Char * data(size_t i) const{return &buf_[i];}
    bool ensure(size_t len){return (cur_ + len <= buf_.size());}
    bool seek(size_t pos){
        if(pos > buf_.size())
            return false;
        cur_ = pos;
        return true;
    }
    void append(const __Char * buf, size_t sz){
        insert(cur_, buf, sz);
    }
    void append(__Char c){
        assert(cur_ + 1 <= buf_.size());
        *data(cur_++) = c;
    }
    void insert(size_t off, const __Char * buf, size_t sz){
        assert(off <= cur_);
        assert(sz && cur_ + sz <= buf_.size());
        assert(buf);
        const size_t c = cur_ - off;
        if(c)
            __buf_move(data(off + sz), data(off), c);
        __buf_copy(data(off), buf, sz);
        cur_ += sz;
    }
    bool exportData(){
        cur_ = 0;
        return true;
    }
    template<typename SizeT>
    bool exportData(SizeT * sz){
        if(sz)
            *sz = cur();
        return exportData();
    }
    template<typename CharT>
    bool exportData(CharT * buf, size_t * sz){
        if(!buf || !sz || *sz < cur())
            return false;
        __buf_copy(buf, data(0), cur());
        *sz = cur();
        return exportData();
    }
    std::string toString() const{
        CToString oss;
        oss<<"{capacity="<<buf_.capacity()
            <<", buf_="<<tools::DumpHex(data(0), cur_)
            <<"}";
        return oss.str();
    }
private:
    //members
    __Buf buf_;
    size_t cur_;
};

//manipulators

template<typename LenT, class T>
struct CManipulatorArrayPtr
{
    T *         c_;
    size_t    sz1_;
    LenT *    sz2_;
    CManipulatorArrayPtr(T * c, size_t sz, LenT * p)
        : c_(c)
        , sz1_(sz)
        , sz2_(p)
    {}
};

template<typename LenT, class T>
struct CManipulatorArrayCont
{
    T & c_;
    LenT max_;
    CManipulatorArrayCont(T & c, LenT max_size)
        : c_(c)
        , max_(max_size)
    {}
};

template<typename LenT, class Iter>
struct CManipulatorArrayRange
{
    Iter beg_, end_;
    LenT * sz_;
    CManipulatorArrayRange(Iter first, Iter last, LenT * sz)
        : beg_(first)
        , end_(last)
        , sz_(sz)
    {}
};

template<class T>
struct CManipulatorRawPtr
{
    T *     c_;
    size_t  sz_;
    CManipulatorRawPtr(T * c, size_t sz):c_(c),sz_(sz){}
};

template<class T>
struct CManipulatorRawCont
{
    T & c_;
    size_t sz_;
    size_t * sz2_;
    CManipulatorRawCont(T & c, size_t sz, size_t * sz2)
        : c_(c)
        , sz_(sz)
        , sz2_(sz2)
    {}
};

template<class Iter>
struct CManipulatorRawRange
{
    Iter beg_,end_;
    size_t * sz_;
    CManipulatorRawRange(Iter first, Iter last, size_t * sz)
        : beg_(first)
        , end_(last)
        , sz_(sz)
    {}
};

template<class T>
struct CManipulatorValueByteOrder
{
    typedef void (*__Fun)(CDataStreamBase &);
    T & c_;
    __Fun fun_;
    CManipulatorValueByteOrder(T & c, __Fun f):c_(c),fun_(f){}
};

struct CManipulatorSeek
{
    size_t pos_;
    explicit CManipulatorSeek(size_t p):pos_(p){}
};

struct CManipulatorSkip
{
    ssize_t off_;
    explicit CManipulatorSkip(ssize_t off):off_(off){}
};

struct CManipulatorSkipFill
{
    ssize_t off_;
    int fill_;
    explicit CManipulatorSkipFill(ssize_t off, int fill):off_(off),fill_(fill){}
};

template<typename T>
struct CManipulatorSkipPtr
{
    T * off_;
    explicit CManipulatorSkipPtr(T * off):off_(off){}
};

template<typename T>
struct CManipulatorSkipPtrFill
{
    T * off_;
    int fill_;
    explicit CManipulatorSkipPtrFill(T * off, int fill):off_(off),fill_(fill){}
};

template<class T>
struct CManipulatorOffsetValue
{
    T &     c_;
    size_t  pos_;
    CManipulatorOffsetValue(size_t p, T & c):c_(c),pos_(p){}
};

template<class T>
struct CManipulatorInsert
{
    const T &   c_;
    size_t      pos_;
    CManipulatorInsert(size_t p, const T & c):c_(c),pos_(p){}
};

template<class T>
struct CManipulatorProtobuf
{
    typedef T __Msg;
    __Msg & c_;
    size_t  sz_;
    CManipulatorProtobuf(__Msg & c, size_t sz):c_(c),sz_(sz){}
};

template<typename T>
class CManipulatorVarint
{
    typedef CTypeTraits<typename COmitCV<T>::result_type>  __Traits;
public:
    typedef typename __Traits::__Signed     __Signed;
    typedef typename __Traits::__Unsigned   __Unsigned;
    CManipulatorVarint(T & c):c_(c){}
    __Unsigned toUnsigned() const{
        __Unsigned r = 0;
        return trans(c_, r);
    }
    void fromUnsigned(__Unsigned v) const{
        trans(v, c_);
    }
private:
    template<typename U>
    static U trans(U v, U & t){return (t = v);}
    static __Unsigned trans(__Signed v, __Unsigned & t){
        return (t = (v << 1) ^ (v >> (__Traits::kMaxBits - 1)));
    }
    static __Signed trans(__Unsigned v, __Signed & t){
        return (t = (v >> 1) ^ (-(v & 1)));
    }
    T & c_;
};

struct CManipulatorStubPush
{
    size_t sz_;
    explicit CManipulatorStubPush(size_t sz):sz_(sz){}
};

struct CManipulatorStubPop
{
    bool align_, check_;
    CManipulatorStubPop(bool align, bool check):align_(align), check_(check){}
};

template<class T, class S>
struct CManipulatorEnd;

template<>
struct CManipulatorEnd<void, void>{};

template<class SizeT>
struct CManipulatorEnd<SizeT *, void>
{
    SizeT * sz_;
    explicit CManipulatorEnd(SizeT * s):sz_(s){}
};

template<class CharT>
struct CManipulatorEnd<CharT *, size_t *>
{
    CharT * buf_;
    size_t * sz_;
    CManipulatorEnd(CharT * buf, size_t * sz):buf_(buf),sz_(sz){}
};

template<class BufT>
struct CManipulatorEnd<BufT, void>
{
    BufT & buf_;
    explicit CManipulatorEnd(BufT & buf):buf_(buf){}
};

#endif  // DOX_UNDOCUMENT_FLAG

NS_IMPL_END

#endif
