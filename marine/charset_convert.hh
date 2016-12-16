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
 * @brief Charset processing and conversion.
 * @author Zhao DAI
 */

#ifndef DOZERG_CHARSET_CONVERT_H_20130513
#define DOZERG_CHARSET_CONVERT_H_20130513

#include <iconv.h>
#include <cassert>
#include "to_string.hh"

NS_SERVER_BEGIN

/**
 * @brief Converting text between different character encodings.
 * CCharsetConvert uses @c iconv(3) to perform charset conversion operations. It manages system
 * resource allocation and deallocation, conversion adjustment and error handling.
 * @note CCharsetConvert object can @em NOT be copied.
 */
class CCharsetConvert
{
public:
    /**
     * @brief Conversion mode
     */
    enum EMode{
        /**
         * @details Default mode.
         */
        kNormal = 0,
        /**
         * @details Characters being converted are transliterated when needed and possible. This
         * means that when a character cannot be represented in the target character set, it can be
         * approximated through one or several similar looking characters. Characters that are
         * outside of the target character set and cannot be transliterated are replaced with a
         * question mark (?) in the output.
         */
        kTranslit,
        /**
         * @details Characters that cannot be converted are discarded and an error is printed after
         * conversion.
         */
        kIgnore
    };
    /**
     * @brief Construct a default object, with no ability for conversion.
     * Please use @ref init to initialize this object for use.
     * @sa init
     */
    CCharsetConvert()
        : cv_(iconv_t(-1))
        , mode_(kNormal)
    {}
    /**
     * @brief Destructor for releasing resources.
     */
    ~CCharsetConvert(){
        if(valid())
            ::iconv_close(cv_);
    }
    /**
     * @brief Construct a usable converter.
     * For a full encoding names list supported by your system, please run `iconv --list`.
     * @param fromCode Encoding name for source text, e.g. "UTF8"
     * @param toCode Encoding name for destination text, e.g. "GB2312"
     * @param mode Conversion mode, see @ref EMode
     */
    CCharsetConvert(const std::string & fromCode, const std::string & toCode, EMode mode = kNormal)
        : cv_(iconv_t(-1))
        , mode_(kNormal)
    {
        init(fromCode, toCode, mode);
    }
    /**
     * @brief Initialize a usable converter.
     * If this object is already initialized, this function will fail.
     * @n For a full encoding names list supported by your system, please run `iconv --list`.
     * @param fromCode Encoding name for source text, e.g. "UTF8"
     * @param toCode Encoding name for destination text, e.g. "GB2312"
     * @param mode Conversion mode, see @ref EMode
     * @return @c true if succeeded; otherwise @c false
     */
    bool init(const std::string & fromCode, const std::string & toCode, EMode mode = kNormal){
        if(valid())
            return false;
        if(fromCode.empty() || toCode.empty())
            return false;
        std::string t = toCode;
        switch(mode){
            case kTranslit:t += "//TRANSLIT";break;
            case kIgnore:t += "//IGNORE";break;
            default:;
        }
        from_ = fromCode;
        to_ = toCode;
        mode_ = mode;
        cv_ = ::iconv_open(t.c_str(), fromCode.c_str());
        return valid();
    }
    /**
     * @brief Test whether this object is initialized.
     * @return @c true if this object is already initialized; otherwise @c false
     */
    bool valid() const{return (iconv_t(-1) != cv_);}
    /**
     * @brief Get encoding name for source text.
     * If this object is not initialized, an empty string will be returned.
     * @return Encoding name for source text
     */
    const std::string fromCode() const{return from_;}
    /**
     * @brief Get encoding name for destination text.
     * If this object is not initialized, an empty string will be returned.
     * @return Encoding name for destination text
     */
    const std::string toCode() const{return to_;}
    /**
     * @brief Get conversion mode.
     * If this object is not initialized, @c kNormal will be returned.
     * @return Conversion mode
     */
    EMode mode() const{return mode_;}
    /**
     * @brief Convert text from source encoding to destination encoding.
     * If @c dest is not empty, the conversion result will append to @c dest.
     * @param[in] source Source text
     * @param[out] dest A String to receive the result of conversion
     * @return @c true if succeeded; otherwise @c false
     */
    bool convert(const std::string & source, std::string & dest){
        if(!valid())
            return false;
        if(source.empty())
            return true;
        ::iconv(cv_, NULL, NULL, NULL, NULL);    //reset init state;
        char * inbuf = const_cast<char *>(&source[0]);  //iconv()'s bug
        size_t inLen = source.size();
        for(char buf[64];inLen > 0;){
            const size_t inOld = inLen;
            char * outbuf = buf;
            size_t outLen = sizeof buf;
            ::iconv(cv_, &inbuf, &inLen, &outbuf, &outLen);
            if(inOld <= inLen)   //cannot proceed
                return false;
            if(buf != outbuf){
                assert(buf < outbuf);
                dest.append(buf, outbuf - buf);
            }
        }
        return true;
    }
    /**
     * @brief Get a readable description of this object.
     * @return A readable description of this object
     */
    std::string toString() const{
        CToString oss;
        oss<<"{cv_="<<cv_
            <<", from_="<<from_
            <<", to_="<<to_
            <<", mode_="<<mode_
            <<'}';
        return oss.str();
    }
private:
    CCharsetConvert(const CCharsetConvert &);   //disable copy and assignment
    CCharsetConvert & operator =(const CCharsetConvert &);
    //members
    std::string from_, to_;
    iconv_t cv_;
    EMode mode_;
};

NS_SERVER_END

#endif

