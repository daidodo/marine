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
 * @brief An STL-like encapsulation of byte array.
 * @author Zhao DAI
 */

#ifndef DOZERG_CHAR_BUFFER_H_20121218
#define DOZERG_CHAR_BUFFER_H_20121218

#include <cassert>
#include <algorithm>
#include <stdexcept>
#include <cstddef>
#include <iterator>
#include <memory>
#include <cstring>
#include "impl/environment.hh"

NS_SERVER_BEGIN

/**
 * @brief Provide interfaces similar to std::string for raw byte array.
 * CCharBuffer provides convenient interfaces, like @c begin/end, @c append, @c insert, to
 * manipulate a byte array, and performs necessary boundary checks to prevent access violation. It
 * does @em NOT manage memory, so the user is responsible for the validation of the underlying
 * buffer.
 * @tparam CharT A character type, must be `char`, `unsigned char` or `signed char`. Other wider
 * sized types are @em NOT supported, e.g. `wchar_t`.
 */
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
    /**
     * @brief Construct a @a null object with zero capacity.
     */
    CCharBuffer()
        : buf_(NULL)
        , capa_(0)
        , sz_(0)
    {}
    /**
     * @brief Construct from a string.
     * Both size and capacity are set to @c strlen(buf).
     * @param buf Pointer to a string
     */
    CCharBuffer(pointer buf)
        : buf_(buf)
        , capa_(traits_type::length(buf))
        , sz_(capa_)
    {}
    /**
     * @brief Construct from a byte buffer.
     * @param buf Pointer to a byte buffer. Must @em NOT be NULL.
     * @param capacity Size of bytes that this object can manipulate. It could be smaller than the
     * allocated size of @c buf, but not greater.
     * @param size Size of used bytes. It should be no more that @c capacity, or @c capacity will be
     * used instead.
     */
    CCharBuffer(pointer buf, size_type capacity, size_type size = 0)
        : buf_(buf)
        , capa_(capacity)
        , sz_(std::min(size, capacity))
    {}
    /**
     * @brief Set this object to manage a byte buffer.
     * @param buf Pointer to a byte buffer
     * @param capacity Size of bytes that this object can manipulate. It could be different from the
     * real allocated size of @c buf.
     * @param size Size of used bytes. It should be no more that @c capacity, or @c capacity will be
     * used instead.
     * @return A reference to itself
     */
    __Myt & assign(pointer buf, size_type capacity, size_type size = 0){
        buf_ = buf;
        capa_ = capacity;
        sz_ = std::min(size, capacity);
        return *this;
    }
    /**
     * @brief Set this object to manage a buffer from another object.
     * These two objects will manipulate the same byte buffer, so if one changes the content of
     * the buffer, the other will reflect the change. But they can have different @c size, for
     * example, one can @c resize to a new size, the other object's @c size won't get affected.
     * @param other Reference to another CCharBuffer object.
     * @return A reference to itself
     */
    __Myt & assign(const __Myt & other){
        if(this != &other)
            assign(other.buf_, other.capa_, other.sz_);
        return *this;
    }
    /**
     * @brief Copy content of a buffer to self.
     * This function tries to copy at most @c count bytes from @c buf, to the buffer managed by this
     * object, starting from @c offset, which is similar to:
     * @code std::copy(buf, buf + count, this->begin() + offset); @endcode
     * except that if this object cannot hold as many as @c count bytes from @c offset, it will
     * copy less bytes, which will be indicated in the return value.
     * @param buf Pointer to a byte buffer.
     * @param count Size of bytes to copy from @c buf.
     * @param offset Offset of content in @c buf.
     * @return Size of bytes actually copied.
     */
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
    /**
     * @brief Swap two object.
     * @param other Reference to another object.
     */
    void swap(__Myt & other) throw() {
        if(this != &other){
            std::swap(buf_, other.buf_);
            std::swap(capa_, other.capa_);
            std::swap(sz_, other.sz_);
        }
    }
    /**
     * @brief Get the beginning of the data.
     * @return An mutable iterator pointing to the beginning of the data.
     */
    iterator begin(){return iterator(&buf_[0]);}
    /**
     * @brief Get the ending of the data.
     * @return An mutable iterator pointing to the ending of the data, which is one position after
     * the last used byte.
     */
    iterator end(){return iterator(&buf_[sz_]);}
    /**
     * @brief Get the beginning of the data.
     * @return An constant iterator pointing to the beginning of the data.
     */
    const_iterator begin() const{return const_iterator(&buf_[0]);}
    /**
     * @brief Get the ending of the data.
     * @return An constant iterator pointing to the ending of the data, which is one position after the
     * last used byte.
     */
    const_iterator end() const{return const_iterator(&buf_[sz_]);}
    /**
     * @brief Get the ending of the data.
     * @return An mutable reverse iterator pointing to the ending of the data.
     */
    reverse_iterator rbegin(){return reverse_iterator(end());}
    /**
     * @brief Get the beginning of the data.
     * @return An mutable reverse iterator pointing to the beginning of the data.
     */
    reverse_iterator rend(){return reverse_iterator(begin());}
    /**
     * @brief Get the ending of the data.
     * @return An constant reverse iterator pointing to the ending of the data.
     */
    const_reverse_iterator rbegin() const{return const_reverse_iterator(end());}
    /**
     * @brief Get the beginning of the data.
     * @return An constant reverse iterator pointing to the beginning of the data.
     */
    const_reverse_iterator rend() const{return const_reverse_iterator(begin());}
    /**
     * @brief Test if the data is empty.
     * @return @c true if @c size is 0; otherwise @c false.
     */
    bool empty() const{return !sz_;}
    /**
     * @brief Get the size of the data, same as @c size().
     * @return Size of used bytes.
     * @sa size
     */
    size_type length() const{return size();}
    /**
     * @brief Get the size of the data.
     * @return Size of used bytes.
     * @sa length
     */
    size_type size() const{return sz_;}
    /**
     * @brief Get the capacity of the underlying byte buffer.
     * @return Size of bytes that this object can hold at most.
     */
    size_type capacity() const{return capa_;}
    /**
     * @brief Get the maximum number of bytes.
     * @return Maximum number of bytes.
     */
    size_type max_size() const{return size_type(-1);}
    /**
     * @brief Empty the data.
     * This function will @em NOT change the content of the underlying byte buffer, only reset @c
     * size of this object.
     */
    void clear(){sz_ = 0;}
    /**
     * @brief Access a byte.
     * @param i Index of the byte.
     * @return Mutable reference to a byte.
     */
    reference operator [](size_type i){return buf_[i];}
    /**
     * @brief Access a byte.
     * @param i Index of the byte.
     * @return Constant reference to a byte.
     */
    const_reference operator [](size_type i) const{return buf_[i];}
    /**
     * @brief Access the first byte.
     * @return Mutable reference to the first byte.
     */
    reference front(){return operator [](0);}
    /**
     * @brief Access the first byte.
     * @return Constant reference to the first byte.
     */
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

