#ifndef DOZERG_CHAR_BUFFER_H_20121218
#define DOZERG_CHAR_BUFFER_H_20121218

/*
    封装原始字符串数组，使其更接近stl的接口
        CCharBuffer     对字节数据进行封装，提供类似std::string的接口
//*/

#include <cassert>
#include <algorithm>
#include <stdexcept>
#include <cstddef>
#include <iterator>
#include <memory>
#include <cstring>
#include "impl/environment.hh"

NS_SERVER_BEGIN

template<typename CharT>
class CCharBuffer
{
    typedef CCharBuffer<CharT> __Myt;
    typedef CharT __Char;
public:
    typedef std::char_traits<__Char>                traits_type;
    typedef typename traits_type::char_type         value_type;
    typedef value_type &                            reference;
    typedef const value_type &                      const_reference;
    typedef value_type *                            pointer;
    typedef const value_type *                      const_pointer;
    typedef __gnu_cxx::__normal_iterator<
        pointer, CCharBuffer>                       iterator;
    typedef __gnu_cxx::__normal_iterator<
        const_pointer, CCharBuffer>                 const_iterator;
    typedef size_t                                  size_type;
    typedef ptrdiff_t                               difference_type;
    typedef std::reverse_iterator<iterator>         reverse_iterator;
    typedef std::reverse_iterator<const_iterator>   const_reverse_iterator;
    static const size_type npos = static_cast<size_type>(-1);
    //functions
    CCharBuffer()
        : buf_(NULL)
        , capa_(0)
        , sz_(0)
    {}
    CCharBuffer(pointer buf)
        : buf_(buf)
        , capa_(traits_type::length(buf))
        , sz_(capa_)
    {}
    CCharBuffer(pointer buf, size_type capacity, size_type size = 0)
        : buf_(buf)
        , capa_(capacity)
        , sz_(size)
    {}
    __Myt & assign(pointer buf, size_type capacity, size_type size = 0){
        buf_ = buf;
        capa_ = capacity;
        sz_ = size;
        return *this;
    }
    __Myt & assign(const __Myt & other){
        if(this != &other)
            assign(other.buf_, other.capa_, other.sz_);
        return *this;
    }
    size_type copy(pointer buf, size_type count, size_type offset = 0) const{
        check_size_throw(offset);
        if(offset + count > sz_)
            count = sz_ - offset;
        if(count){
            assert(buf_ && buf);
            copy_aux(buf, buf_ + offset, count);
        }
        return count;
    }
    void swap(__Myt & other) throw() {
        if(this != &other){
            std::swap(buf_, other.buf_);
            std::swap(capa_, other.capa_);
            std::swap(sz_, other.sz_);
        }
    }
    iterator begin(){return iterator(&buf_[0]);}
    iterator end(){return iterator(&buf_[sz_]);}
    const_iterator begin() const{return const_iterator(&buf_[0]);}
    const_iterator end() const{return const_iterator(&buf_[sz_]);}
    reverse_iterator rbegin(){return reverse_iterator(end());}
    reverse_iterator rend(){return reverse_iterator(begin());}
    const_reverse_iterator rbegin() const{return const_reverse_iterator(end());}
    const_reverse_iterator rend() const{return const_reverse_iterator(begin());}
    bool empty() const{return !sz_;}
    size_type length() const{return size();}
    size_type size() const{return sz_;}
    size_type capacity() const{return capa_;}
    size_type max_size() const{return size_type(-1);}
    void clear(){sz_ = 0;}
    reference operator [](size_type i){return buf_[i];}
    const_reference operator [](size_type i) const{return buf_[i];}
    reference front(){return operator [](0);}
    const_reference front() const{return operator [](0);}
    reference back(){return operator [](sz_ - 1);}
    const_reference back() const{return operator [](sz_ - 1);}
    const_pointer c_str() const{return buf_;}
    const_pointer data() const{return buf_;}
    reference at(size_type i){
        assert(buf_);
        check_offset_throw(i);
        return buf_[i];
    }
    const_reference at(size_type i) const{
        assert(buf_);
        check_offset_throw(i);
        return buf_[i];
    }
    void resize(size_type count, value_type val = 0){
        check_capa_throw(count);
        if(count > sz_){
            assert(buf_);
            assign_aux(buf_ + sz_, count - sz_, val);
        }
        sz_ = count;
    }
    void push_back(value_type val){
        assert(buf_);
        check_capa_throw(sz_ + 1);
        buf_[sz_++] = val;
    }
    __Myt & append(size_type count, value_type val){return replace(sz_, 0, count, val);}
    __Myt & append(const_pointer buf, size_type count){return replace(sz_, 0, buf, count);}
    __Myt & append(const_pointer buf){return replace(sz_, 0, buf);}
    __Myt & append(const __Myt & other, size_type offset, size_type count){return replace(sz_, 0, other, offset, count);}
    __Myt & append(const __Myt & other){return replace(sz_, 0, other);}
    __Myt & operator +=(const __Myt & other){return append(other);}
    __Myt & operator +=(const_pointer buf){return append(buf);}
    __Myt & operator +=(value_type val){
        push_back(val);
        return *this;
    }
    __Myt & insert(size_type offset, size_type count, value_type val){return replace(offset, 0, count, val);}
    __Myt & insert(size_type offset, const_pointer buf, size_type count){return replace(offset, 0, buf, count);}
    __Myt & insert(size_type offset, const_pointer buf){return insert(offset, buf, traits_type::length(buf));}
    __Myt & insert(size_type offset, const __Myt & other, size_type newOffset, size_type count){
        return replace(offset, 0, other, newOffset, count);
    }
    __Myt & insert(size_type offset, const __Myt & other){return insert(offset, other, 0, other.size());}
    iterator insert(iterator pos, value_type val){
        replace(pos, pos, size_type(1), val);
        return pos;
    }
    __Myt & replace(size_type offset, size_type count, size_type newCount, value_type val){
        check_size_range_throw(offset, count);
        check_capa_throw(offset + newCount);
        check_capa_throw(sz_ + newCount - count);
        mutate_aux(offset, count, newCount);
        if(newCount){
            assert(buf_);
            assign_aux(buf_ + offset, newCount, val);
        }
        return *this;
    }
    __Myt & replace(size_type offset, size_type count, const_pointer buf, size_type newCount){
        check_size_range_throw(offset, count);
        check_capa_throw(offset + newCount);
        check_capa_throw(sz_ + newCount - count);
        mutate_aux(offset, count, newCount);
        if(newCount){
            assert(buf_ && buf);
            copy_aux(buf_ + offset, buf, newCount);
        }
        return *this;
    }
    __Myt & replace(size_type offset, size_type count, const_pointer buf){
        return replace(offset, count, buf, traits_type::length(buf));
    }
    __Myt & replace(size_type offset, size_type count, const __Myt & other, size_type newOffset, size_type newCount){
        other.check_size_throw(newOffset);
        other.check_size_throw(newOffset + newCount);
        return replace(offset, count, other.buf_ + newOffset, newCount);
    }
    __Myt & replace(size_type offset, size_type count, const __Myt & other){
        return replace(offset, count, other, size_type(0), other.size());
    }
    __Myt & replace(iterator first, iterator last, size_type count, value_type val){
        check_iter_range_throw(first, last);
        return replace(size_type(first - begin()), size_type(last - first), count, val);
    }
    __Myt & replace(iterator first, iterator last, const_pointer buf, size_type count){
        check_iter_range_throw(first, last);
        return replace(size_type(first - begin()), size_type(last - first), buf, count);
    }
    __Myt & replace(iterator first, iterator last, const_pointer buf){
        return replace(first, last, buf, size_type(traits_type::length(buf)));
    }
    __Myt & replace(iterator first, iterator last, const __Myt & other, size_type offset, size_type count){
        check_iter_range_throw(first, last);
        return replace(size_type(first - begin()), size_type(last - first), other, offset, count);
    }
    __Myt & replace(iterator first, iterator last, const __Myt & other){
        return replace(first, last, other, 0, other.size());
    }
    iterator erase(iterator first, iterator last){
        replace(first, last, size_type(0), value_type(0));
        return first;
    }
    iterator erase(iterator pos){
        replace(pos, pos + 1, size_type(0), value_type(0));
        return pos;
    }
    __Myt & erase(size_type offset = 0, size_type count = npos){
        check_offset_throw(offset);
        if(offset + count > sz_)
            count = sz_ - offset;
        mutate_aux(offset, limit_count(offset, count), 0);
        return *this;
    }
    __Myt substr(size_type offset = 0, size_type count = npos){
        check_offset_throw(offset);
        return __Myt(buf_ + offset, capa_ - offset, limit_count(offset, count));
    }
    int compare(size_type offset, size_type count, const_pointer buf, size_type newCount) const{
        check_size_range_throw(offset, count);
        size_type c = std::min(count, newCount);
        int ret = 0;
        if(c){
            assert(buf_ && buf);
            ret = traits_type::compare(buf_ + offset, buf, c);
        }
        if(!ret)
            ret = (count < newCount ? -1 : (count > newCount ? 1 : 0));
        return ret;
    }
    int compare(size_type offset, size_type count, const_pointer buf) const{
        return compare(offset, count, buf, traits_type::length(buf));
    }
    int compare(const_pointer buf) const{return compare(size_type(0), size(), buf);}
    int compare(size_type offset, size_type count, const __Myt & other, size_type newOffset, size_type newCount) const{
        other.check_size_range_throw(newOffset, newCount);
        return compare(offset, count, const_pointer(other.buf_ + newOffset), newCount);
    }
    int compare(size_type offset, size_type count, const __Myt & other) const{
        return compare(offset, count, other, 0, other.size());
    }
    int compare(const __Myt & other) const{return compare(size_type(0), size(), other);}
    //find xxx
    //rfind
private:
    void check_offset_throw(size_type i) const{
        if(i >= sz_)
            throw std::out_of_range("CCharBuffer offset out of range");
    }
    void check_size_throw(size_type i) const{
        if(i > sz_)
            throw std::out_of_range("CCharBuffer size out of range");
    }
    void check_capa_throw(size_type i) const{
        if(i > capa_)
            throw std::out_of_range("CCharBuffer capacity out of range");
    }
    void check_iter_throw(iterator it) const{
        if(it < begin() || it > end())
            throw std::out_of_range("CCharBuffer iterator out of range");
    }
    void check_size_range_throw(size_type offset, size_type count) const{
        check_size_throw(offset);
        if(count)
            check_size_throw(offset + count);
    }
    void check_iter_range_throw(iterator first, iterator last) const{
        assert(first <= last);
        check_iter_throw(first);
        if(last != first)
            check_iter_throw(last);
    }
    static void assign_aux(pointer pos, size_type count, value_type val){
        assert(pos);
        if(1 == count)
            traits_type::assign(*pos, val);
        else
            traits_type::assign(pos, count, val);
    }
    static void copy_aux(pointer dst, const_pointer src, size_type count){
        assert(dst && src);
        if(1 == count)
            traits_type::assign(*dst, *src);
        else
            traits_type::copy(dst, src, count);
    }
    static void move_aux(pointer dst, const_pointer src, size_type count){
        assert(dst && src);
        if(1 == count)
            traits_type::assign(*dst, *src);
        else
            traits_type::move(dst, src, count);
    }
    void mutate_aux(size_type offset, size_type count, size_type newCount){
        assert(sz_ >= offset + count);
        const size_type c = sz_ - offset - count;
        if(c && count != newCount)
            move_aux(buf_ + offset + newCount, buf_ + offset + count, c);
        sz_ += newCount - count;
    }
    size_type limit_count(size_type offset, size_type count) const{
        const size_type c = sz_ - offset;
        return std::min(c, count);
    }
    //members
    __Char * buf_;
    size_t capa_;
    size_t sz_;
};

//no operator +

template<typename CharT>
bool operator ==(const CCharBuffer<CharT> & left, const CCharBuffer<CharT> & right)
{
    return (left.size() == right.size() && 0 == left.compare(right));
}

template<typename CharT>
bool operator ==(const CharT * left, const CCharBuffer<CharT> & right)
{
    return (0 == right.compare(left));
}

template<typename CharT>
bool operator ==(const CCharBuffer<CharT> & left, const CharT * right)
{
    return (0 == left.compare(right));
}

template<typename CharT>
bool operator !=(const CCharBuffer<CharT> & left, const CCharBuffer<CharT> & right)
{
    return !(left == right);
}

template<typename CharT>
bool operator !=(const CharT * left, const CCharBuffer<CharT> & right)
{
    return !(left == right);
}

template<typename CharT>
bool operator !=(const CCharBuffer<CharT> & left, const CharT * right)
{
    return !(left == right);
}

template<typename CharT>
bool operator <(const CCharBuffer<CharT> & left, const CCharBuffer<CharT> & right)
{
    return (left.compare(right) < 0);
}

template<typename CharT>
bool operator <(const CharT * left, const CCharBuffer<CharT> & right)
{
    return (0 < right.compare(left));
}

template<typename CharT>
bool operator <(const CCharBuffer<CharT> & left, const CharT * right)
{
    return (left.compare(right) < 0);
}

template<typename CharT>
bool operator <=(const CCharBuffer<CharT> & left, const CCharBuffer<CharT> & right)
{
    return !(left > right);
}

template<typename CharT>
bool operator <=(const CharT * left, const CCharBuffer<CharT> & right)
{
    return !(left > right);
}

template<typename CharT>
bool operator <=(const CCharBuffer<CharT> & left, const CharT * right)
{
    return !(left > right);
}

template<typename CharT>
bool operator >(const CCharBuffer<CharT> & left, const CCharBuffer<CharT> & right)
{
    return (right < left);
}

template<typename CharT>
bool operator >(const CharT * left, const CCharBuffer<CharT> & right)
{
    return (right < left);
}

template<typename CharT>
bool operator >(const CCharBuffer<CharT> & left, const CharT * right)
{
    return (right < left);
}

template<typename CharT>
bool operator >=(const CCharBuffer<CharT> & left, const CCharBuffer<CharT> & right)
{
    return !(left < right);
}

template<typename CharT>
bool operator >=(const CharT * left, const CCharBuffer<CharT> & right)
{
    return !(left < right);
}

template<typename CharT>
bool operator >=(const CCharBuffer<CharT> & left, const CharT * right)
{
    return !(left < right);
}

template<typename CharT>
void swap(const CCharBuffer<CharT> & left, const CCharBuffer<CharT> & right)
{
    left.swap(right);
}

NS_SERVER_END

#endif

