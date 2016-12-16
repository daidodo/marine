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
 * @brief An STL-like byte array.
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
 * @brief Provide interfaces similar to @c std::string for raw byte array.
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
     * @brief Construct from a C-style string.
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
     * @param buf Pointer to a byte buffer.
     * @param capacity Size of bytes that this object can manipulate. It could be smaller than the
     * allocated size of @c buf, but not greater.
     * @param size Size of used bytes. It should be no more that @c capacity, otherwise @c capacity
     * will be used instead.
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
     * @param size Size of used bytes. It should be no more that @c capacity, otherwise @c capacity
     * will be used instead.
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
     * @param other Another CCharBuffer object.
     * @return A reference to itself
     */
    __Myt & assign(const __Myt & other){
        if(this != &other)
            assign(other.buf_, other.capa_, other.sz_);
        return *this;
    }
    /**
     * @brief Swap two CCharBuffer objects.
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
     * @name Iterators
     * @{ */
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
    /**  @} */
    /**
     * @name Capacity
     * @{ */
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
    /**  @} */
    /**
     * @name Element access
     * @{ */
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
    /**
     * @brief Access the last byte.
     * @return Mutable reference to the back byte.
     */
    reference back(){return operator [](sz_ - 1);}
    /**
     * @brief Access the last byte.
     * @return Constant reference to the last byte.
     */
    const_reference back() const{return operator [](sz_ - 1);}
    /**
     * @brief Access a certain byte.
     * @param i Index of the byte
     * @return Mutable reference of the byte
     * @note If @c i exceed @c size, an @em exception will be thrown.
     */
    reference at(size_type i){
        assert(buf_);
        check_offset_throw(i);
        return buf_[i];
    }
    /**
     * @brief Access a certain byte.
     * @param i Index of the byte
     * @return Constant reference of the byte
     * @note If @c i exceed @c size, an @em exception will be thrown.
     */
    const_reference at(size_type i) const{
        assert(buf_);
        check_offset_throw(i);
        return buf_[i];
    }
    /**
     * @brief Get the underlying C-style string.
     * @return Pointer to the data
     * @sa data
     */
    const_pointer c_str() const{return buf_;}
    /**
     * @brief Get the data pointer.
     * @return Pointer to the data
     * @sa c_str
     */
    const_pointer data() const{return buf_;}
    /**  @} */
    /**
     * @name Operations
     * If any of the following operations makes @c size exceed @c capacity, an @em exception
     * will be thrown to indicate the error.
     * @{ */
    /**
     * @brief Empty the data.
     * This function will @em NOT change the content of the underlying byte buffer, only reset @c
     * size of this object.
     */
    void clear(){sz_ = 0;}
    /**
     * @brief Resize the data.
     * If data shrinks, no change will be made to the content of the underlying buffer. If data
     * expands, new data will be filled with @c val.
     * @param count New size of the data
     * @param val Value to fill in the expanded data
     */
    void resize(size_type count, value_type val = 0){
        check_capa_throw(count);
        if(count > sz_){
            assert(buf_);
            assign_aux(buf_ + sz_, count - sz_, val);
        }
        sz_ = count;
    }
    /**
     * @brief Copy content of a buffer to self.
     * This function tries to copy at most @c count bytes from @c buf, to the buffer managed by this
     * object, starting from @c offset, which is similar to:
     * @code std::copy(buf, buf + count, this->begin() + offset); @endcode
     * except that if self cannot hold as many as @c count bytes from @c offset, it will
     * copy less bytes, which will be indicated in the return value.
     * @param buf Pointer to a byte buffer.
     * @param count Size of bytes to copy from @c buf.
     * @param offset Offset of destination in self.
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
     * @brief Append a byte to the end of the data.
     * @param val Value to append to the data.
     */
    void push_back(value_type val){
        assert(buf_);
        check_capa_throw(sz_ + 1);
        buf_[sz_++] = val;
    }
    /**
     * @brief Append a number of bytes to the end of the data.
     * @param count Number of bytes to append.
     * @param val Value of bytes to append.
     * @return Reference to self
     */
    __Myt & append(size_type count, value_type val){return replace(sz_, 0, count, val);}
    /**
     * @brief Append content of a buffer to the end of the data.
     * @param buf Pointer to a byte buffer.
     * @param count Size of the buff.
     * @return Reference to self
     */
    __Myt & append(const_pointer buf, size_type count){return replace(sz_, 0, buf, count);}
    /**
     * @brief Append a C-style string to the end of the data.
     * @param buf Pointer to a string.
     * @return Reference to self
     */
    __Myt & append(const_pointer buf){return replace(sz_, 0, buf);}
    /**
     * @brief  Append content of another CCharBuffer content to the end of the data.
     * This function appends contents between `other.begin() + offset` and `other.begin() + offset +
     * count` to the data.
     * @param other Another CCharBuffer object.
     * @param offset Offset of the content in @c other.
     * @param count Size of the content in @c other.
     * @return Reference to self
     */
    __Myt & append(const __Myt & other, size_type offset, size_type count){return replace(sz_, 0, other, offset, count);}
    /**
     * @brief Append content of another CCharBuffer content to the end of the data.
     * @param other Another CCharBuffer object.
     * @return Reference to self
     */
    __Myt & append(const __Myt & other){return replace(sz_, 0, other);}
    /**
     * @brief Append content of another CCharBuffer to the end of the data.
     * @param other Another CCharBuffer object.
     * @return Reference to self
     * @sa append(const __Myt & other)
     */
    __Myt & operator +=(const __Myt & other){return append(other);}
    /**
     * @brief Append a C-style string to the end of the data.
     * The trailing @c '\0' is @em NOT included.
     * @param buf Pointer to a string.
     * @return Reference to self
     * @sa append(const_pointer buf)
     */
    __Myt & operator +=(const_pointer buf){return append(buf);}
    /**
     * @brief Append a byte to the end of the data.
     * @param val Value to append to the data.
     * @return Reference to self
     * @sa push_back
     */
    __Myt & operator +=(value_type val){
        push_back(val);
        return *this;
    }
    /**
     * @brief Insert a number of bytes.
     * @param offset Position to insert into.
     * @param count Number of bytes to insert.
     * @param val Value of bytes to insert.
     * @return Reference to self
     */
    __Myt & insert(size_type offset, size_type count, value_type val){return replace(offset, 0, count, val);}
    /**
     * @brief Insert the content of a buffer.
     * @param offset Position to insert into.
     * @param buf Pointer to a byte buffer.
     * @param count Length of @c buf.
     * @return Reference to self
     */
    __Myt & insert(size_type offset, const_pointer buf, size_type count){return replace(offset, 0, buf, count);}
    /**
     * @brief Insert a C-style string.
     * The trailing @c '\0' is @em NOT included.
     * @param offset Position to insert into.
     * @param buf Pointer to a string.
     * @return Reference to self
     */
    __Myt & insert(size_type offset, const_pointer buf){return insert(offset, buf, traits_type::length(buf));}
    /**
     * @brief Insert the content of another CCharBuffer.
     * This function reads contents from `other.begin() + newOffset` to `other.begin() + newOffset +
     * count`, and insert it to `this->begin() + offset`.
     * @param offset Position to insert into.
     * @param other Another CCharBuffer object.
     * @param newOffset Offset of the content in @c other.
     * @param count Size of the content in @c other.
     * @return Reference to self
     */
    __Myt & insert(size_type offset, const __Myt & other, size_type newOffset, size_type count){
        return replace(offset, 0, other, newOffset, count);
    }
    /**
     * @brief Insert the content of another CCharBuffer.
     * @param offset Position to insert into.
     * @param other Another CCharBuffer object.
     * @return Reference to self
     */
    __Myt & insert(size_type offset, const __Myt & other){return insert(offset, other, 0, other.size());}
    /**
     * @brief Insert a byte.
     * @param pos Position to insert the byte.
     * @param val Value of the byte.
     * @return @c pos.
     */
    iterator insert(iterator pos, value_type val){
        replace(pos, pos, size_type(1), val);
        return pos;
    }
    /**
     * @brief Replace a range of bytes with a number of new bytes.
     * This function performs actions similar to the following:
     * @li Erasing contents from `begin() + offset` to `begin() + offset + count`;
     * @li Insert new contents to `begin() + offset`;
     *
     * The size of the data may change if @c count is not equal to @c newCount.
     * @param offset Start position of bytes to be replaced.
     * @param count Size of bytes to be replaced.
     * @param newCount Number of new bytes.
     * @param val Value of new bytes.
     * @return Reference to self
     */
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
    /**
     * @brief Replace a range of bytes with the content of a byte buffer.
     * This function performs actions similar to the following:
     * @li Erasing contents from `begin() + offset` to `begin() + offset + count`;
     * @li Insert new contents to `begin() + offset`;
     *
     * The size of the data may change if @c count is not equal to @c newCount.
     * @param offset Start position of bytes to be replaced.
     * @param count Size of bytes to be replaced.
     * @param buf Pointer to a byte buffer.
     * @param newCount Length of @c buf.
     * @return Reference to self
     */
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
    /**
     * @brief Replace a range of bytes with the content of a C-style string.
     * This function performs actions similar to the following:
     * @li Erasing contents from `begin() + offset` to `begin() + offset + count`;
     * @li Insert new contents to `begin() + offset`;
     *
     * The size of the data may change if @c count is not equal to the length of @c buf.
     * @n The trailing @c '\0' of @c buf is not included.
     * @param offset Start position of bytes to be replaced.
     * @param count Size of bytes to be replaced.
     * @param buf Pointer to a string
     * @return Reference to self
     */
    __Myt & replace(size_type offset, size_type count, const_pointer buf){
        return replace(offset, count, buf, traits_type::length(buf));
    }
    /**
     * @brief Replace a range of bytes with the content of another CCharBuffer.
     * This function performs actions similar to the following:
     * @li Erasing contents from `begin() + offset` to `begin() + offset + count`;
     * @li Insert new contents to `begin() + offset`;
     *
     * The size of the data may change if @c count is not equal to @c newCount.
     * @param offset Start position of bytes to be replaced.
     * @param count Size of bytes to be replaced.
     * @param other Another CCharBuffer object.
     * @param newOffset Offset of content in @c other.
     * @param newCount Size of content in @c other.
     * @return Reference to self
     */
    __Myt & replace(size_type offset, size_type count, const __Myt & other, size_type newOffset, size_type newCount){
        other.check_size_throw(newOffset);
        other.check_size_throw(newOffset + newCount);
        return replace(offset, count, other.buf_ + newOffset, newCount);
    }
    /**
     * @brief Replace a range of bytes with the content of another CCharBuffer.
     * This function performs actions similar to the following:
     * @li Erasing contents from `begin() + offset` to `begin() + offset + count`;
     * @li Insert new contents to `begin() + offset`;
     *
     * The size of the data may change if @c count is not equal to @c other.size().
     * @param offset Start position of bytes to be replaced.
     * @param count Size of bytes to be replaced.
     * @param other Another CCharBuffer object.
     * @return Reference to self
     */
    __Myt & replace(size_type offset, size_type count, const __Myt & other){
        return replace(offset, count, other, size_type(0), other.size());
    }
    /**
     * @brief Replace a range of bytes with a number of new bytes.
     * This function performs actions similar to the following:
     * @li Erasing contents from @c first to @c last;
     * @li Insert new contents to @c first;
     *
     * The size of the data may change if `last - first` is not equal to @c count.
     * @param first Start position to be replaced.
     * @param last End position to be replaced.
     * @param count Number of new bytes.
     * @param val Value of new bytes.
     * @return Reference to self
     */
    __Myt & replace(iterator first, iterator last, size_type count, value_type val){
        check_iter_range_throw(first, last);
        return replace(size_type(first - begin()), size_type(last - first), count, val);
    }
    /**
     * @brief Replace a range of bytes with the content of a byte buffer.
     * This function performs actions similar to the following:
     * @li Erasing contents from @c first to @c last;
     * @li Insert new contents to @c first;
     *
     * The size of the data may change if `last - first` is not equal to @c count.
     * @param first Start position to be replaced.
     * @param last End position to be replaced.
     * @param buf Pointer to a byte buffer.
     * @param newCount Length of @c buf.
     * @return Reference to self
     */
    __Myt & replace(iterator first, iterator last, const_pointer buf, size_type count){
        check_iter_range_throw(first, last);
        return replace(size_type(first - begin()), size_type(last - first), buf, count);
    }
    /**
     * @brief Replace a range of bytes with the content of a C-style string.
     * This function performs actions similar to the following:
     * @li Erasing contents from @c first to @c last;
     * @li Insert new contents to @c first;
     *
     * The size of the data may change if `last - first` is not equal to the length of @c buf.
     * @n The trailing @c '\0' of @c buf is not included.
     * @param first Start position to be replaced.
     * @param last End position to be replaced.
     * @param buf Pointer to a string
     * @return Reference to self
     */
    __Myt & replace(iterator first, iterator last, const_pointer buf){
        return replace(first, last, buf, size_type(traits_type::length(buf)));
    }
    /**
     * @brief Replace a range of bytes with the content of another CCharBuffer.
     * This function performs actions similar to the following:
     * @li Erasing contents from @c first to @c last;
     * @li Insert new contents to @c first;
     *
     * The size of the data may change if `last - first` is not equal to @c count.
     * @param first Start position to be replaced.
     * @param last End position to be replaced.
     * @param other Another CCharBuffer object.
     * @param offset Offset of content in @c other.
     * @param count Size of content in @c other.
     * @return Reference to self
     */
    __Myt & replace(iterator first, iterator last, const __Myt & other, size_type offset, size_type count){
        check_iter_range_throw(first, last);
        return replace(size_type(first - begin()), size_type(last - first), other, offset, count);
    }
    /**
     * @brief Replace a range of bytes with the content of another CCharBuffer.
     * This function performs actions similar to the following:
     * @li Erasing contents from @c first to @c last;
     * @li Insert new contents to @c first;
     *
     * The size of the data may change if `last - first` is not equal to @c other.size().
     * @param first Start position to be replaced.
     * @param last End position to be replaced.
     * @param other Another CCharBuffer object.
     * @return Reference to self
     */
    __Myt & replace(iterator first, iterator last, const __Myt & other){
        return replace(first, last, other, 0, other.size());
    }
    /**
     * @brief Erase a range of bytes.
     * @param first Start position to be erased.
     * @param last End position to be erased.
     * @return @c first.
     */
    iterator erase(iterator first, iterator last){
        replace(first, last, size_type(0), value_type(0));
        return first;
    }
    /**
     * @brief Erase a byte.
     * @param pos Position of byte to be erased.
     * @return @c pos.
     */
    iterator erase(iterator pos){
        replace(pos, pos + 1, size_type(0), value_type(0));
        return pos;
    }
    /**
     * @brief Erase a range of bytes.
     * @param offset Offset of start position to be erased.
     * @param count Size of bytes to be erased.
     * @return Reference to self.
     */
    __Myt & erase(size_type offset = 0, size_type count = npos){
        check_offset_throw(offset);
        if(offset + count > sz_)
            count = sz_ - offset;
        mutate_aux(offset, limit_count(offset, count), 0);
        return *this;
    }
    /**
     * @brief Get a substring.
     * The substring has the following attributes:
     * @li @c begin() is equal to `this->begin() + offset`;
     * @li @c size() is equal to the smaller one of @c count and `this->size() - offset`;
     * @li @c capacity() is equal to `this->capacity() - offset`;
     * @param offset Offset of start position of the substring.
     * @param count Length of the substring.
     * @return A new CCharBuffer object denote the substring.
     */
    __Myt substr(size_type offset = 0, size_type count = npos){
        check_offset_throw(offset);
        return __Myt(buf_ + offset, capa_ - offset, limit_count(offset, count));
    }
    /**
     * @brief Compare a range of bytes with a buffer lexicographically.
     * This function compares content between `this->begin() + offset` and `this->begin() + offset +
     * count`, with content between @c buf and `buf + count`.
     * @param offset Start position in self to compare.
     * @param count Size of bytes in self to compare.
     * @param buf Pointer to a byte buffer to compare with.
     * @param newCount Length of @c buf to compare with.
     * @return
     *   @li Negative value if the first content is @a less than the second;
     *   @li @c 0 if two contents are equal;
     *   @li Positive value if the first content is @a greater than the second;
     */
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
    /**
     * @brief Compare a range of bytes with a C-style string lexicographically.
     * This function compares content between `this->begin() + offset` and `this->begin() + offset +
     * count`, with content of @c buf.
     * @param offset Start position in self to compare.
     * @param count Size of bytes in self to compare.
     * @param buf Pointer to a string.
     * @return
     *   @li Negative value if the first content is @a less than the second;
     *   @li @c 0 if two contents are equal;
     *   @li Positive value if the first content is @a greater than the second;
     */
    int compare(size_type offset, size_type count, const_pointer buf) const{
        return compare(offset, count, buf, traits_type::length(buf));
    }
    /**
     * @brief Compare self with a C-style string lexicographically.
     * This function compares content between @c this->begin() and @c this->end(), with content of
     * @c buf.
     * @param buf Pointer to a string.
     * @return
     *   @li Negative value if the first content is @a less than the second;
     *   @li @c 0 if two contents are equal;
     *   @li Positive value if the first content is @a greater than the second;
     */
    int compare(const_pointer buf) const{return compare(size_type(0), size(), buf);}
    /**
     * @brief Compare a range of bytes with another CCharBuffer object.
     * This function compares content between `this->begin() + offset` and `this->begin() + offset +
     * count`, with content between `other.begin() + newOffset` and `other.begin() + newOffset +
     * newCount`.
     * @param offset Start position in self to compare.
     * @param count Size of bytes in self to compare.
     * @param other Another CCharBuffer object to compare with.
     * @param newOffset Start position in @c other to compare with.
     * @param newCount Size of bytes in @c other to compare with.
     * @return
     *   @li Negative value if the first content is @a less than the second;
     *   @li @c 0 if two contents are equal;
     *   @li Positive value if the first content is @a greater than the second;
     */
    int compare(size_type offset, size_type count, const __Myt & other, size_type newOffset, size_type newCount) const{
        other.check_size_range_throw(newOffset, newCount);
        return compare(offset, count, const_pointer(other.buf_ + newOffset), newCount);
    }
    /**
     * @brief Compare a range of bytes with another CCharBuffer object.
     * This function compares content between `this->begin() + offset` and `this->begin() + offset +
     * count`, with content between @c other.begin() and @c other.end().
     * @param offset Start position in self to compare.
     * @param count Size of bytes in self to compare.
     * @param other Another CCharBuffer object to compare with.
     * @return
     *   @li Negative value if the first content is @a less than the second;
     *   @li @c 0 if two contents are equal;
     *   @li Positive value if the first content is @a greater than the second;
     */
    int compare(size_type offset, size_type count, const __Myt & other) const{
        return compare(offset, count, other, 0, other.size());
    }
    /**
     * @brief Compare self with another CCharBuffer object.
     * This function compares content between @c this->begin() and @c this->end(), with content
     * between @c other.begin() and @c other.end().
     * @param other Another CCharBuffer object to compare with.
     * @return
     *   @li Negative value if the first content is @a less than the second;
     *   @li @c 0 if two contents are equal;
     *   @li Positive value if the first content is @a greater than the second;
     */
    int compare(const __Myt & other) const{return compare(size_type(0), size(), other);}
    /**  @} */
    //TODO: find xxx, rfind
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

/**
 * @name Lexicographical Comparison
 * @{ */

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

/**  @} */

template<typename CharT>
void swap(const CCharBuffer<CharT> & left, const CCharBuffer<CharT> & right)
{
    left.swap(right);
}

NS_SERVER_END

#endif

