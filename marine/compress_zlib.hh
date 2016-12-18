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
 * @brief [zlib](http://www.zlib.net) compression algorithm.
 * @author Zhao DAI
 */

#ifndef DOZERG_COMPRESSOR_ZLIB_H_20090223
#define DOZERG_COMPRESSOR_ZLIB_H_20090223

#include <stdint.h>
#include <vector>
#include <string>
#include <cstring>
#include <zlib.h>
#include "tools/net.hh"   //Hton, Ntoh
#include "data_stream.hh" //varint

NS_SERVER_BEGIN

/**
 * @brief A convenient interface for [zlib](http://www.zlib.net) compression algorithm.
 * You may need to link to zlib library (e.g. @c -lz) if you use @c CCompressorZlib.
 * @note CCompressorZlib object can @em NOT be copied.
 */
class CCompressorZlib
{
    typedef ::uLongf __ZSize;
    typedef ::Bytef  __ZChar;
public:
    /**
     * @brief Constructor.
     * The compression @c level must be @c Z_DEFAULT_COMPRESSION, or between 0 and 9:
     * @n @c 1 gives best speed, @c 9 gives best compression, @c 0 gives no compression at all (the
     * input data is simply copied a block at a time). @c Z_DEFAULT_COMPRESSION requests a default
     * compromise between speed and compression (currently equivalent to level 6).
     * @param level Compression level (0 to 9, or @c Z_DEFAULT_COMPRESSION)
     */
    CCompressorZlib(int level = Z_DEFAULT_COMPRESSION):level_(level){}
    /**
     * @brief Set compression level.
     * @param lv Compression level (0 to 9, or @c Z_DEFAULT_COMPRESSION)
     */
    void setLevel(int lv){level_ = lv;}
    /**
     * @brief Get compression level.
     * @return Compression level
     */
    int getLevel() const{return level_;}
    /**
     * @name Compression
     * @{
     * @brief Compress given data.
     * If @c output is not empty, the result will append to @c output.
     * @param[in] input Data to be compressed
     * @param[out] output A buffer to receive the result
     * @return @c true if succeeded; otherwise @c false
     */
    bool compress(const std::vector<char> & input, std::vector<char> & output) const{
        return compressTemplate(input, output);
    }
    bool compress(const std::vector<signed char> & input, std::vector<signed char> & output) const{
        return compressTemplate(input, output);
    }
    bool compress(const std::vector<unsigned char> & input, std::vector<unsigned char> & output) const{
        return compressTemplate(input, output);
    }
    bool compress(const std::string & input, std::string & output) const{
        return compressTemplate(input, output);
    }
    /**  @} */
    /**
     * @name Decompression
     * @{
     * @brief Decompress given data.
     * If @c output is not empty, the result will append to @c output.
     * @param[in] input Data to be decompressed
     * @param[out] output A buffer to receive the result
     * @return @c true if succeeded; otherwise @c false
     */
    bool decompress(const std::vector<char> & input, std::vector<char> & output) const{
        return decompressTemplate(input, output);
    }
    bool decompress(const std::vector<signed char> & input, std::vector<signed char> & output) const{
        return decompressTemplate(input, output);
    }
    bool decompress(const std::vector<unsigned char> & input, std::vector<unsigned char> & output) const{
        return decompressTemplate(input, output);
    }
    bool decompress(const std::string & input, std::string & output) const{
        return decompressTemplate(input, output);
    }
    /**  @} */
private:
    template<class Buffer>
    bool compressTemplate(const Buffer & input, Buffer & output) const{
        output.resize(32);
        const __ZSize in_len = __ZSize(input.size());
        COutByteStreamBuf ds(reinterpret_cast<char *>(&output[0]), output.size());
        size_t begin = 0;
        if(!(ds<<Manip::varint(in_len)<<Manip::end(&begin)))
            return false;
        __ZSize out_len = ::compressBound(in_len);
        output.resize(begin + out_len);
        const int ret = ::compress2(
                reinterpret_cast<__ZChar *>(&output[begin]),
                &out_len,
                reinterpret_cast<const __ZChar *>(&input[0]),
                in_len,
                level_);
        if(Z_OK == ret){
            output.resize(begin + out_len);
            return true;
        }
        return false;
    }
    template<class Buffer>
    bool decompressTemplate(const Buffer & input, Buffer & output) const{
        __ZSize out_len = 0;
        CInByteStream ds(reinterpret_cast<const char *>(&input[0]), input.size());
        if(!(ds>>Manip::varint(out_len)))
            return false;
        const size_t begin = ds.cur();
        output.resize(out_len);
        const int ret = ::uncompress(
                reinterpret_cast<__ZChar *>(&output[0]),
                &out_len,
                reinterpret_cast<const __ZChar *>(&input[begin]),
                __ZSize(input.size() - begin));
        if(Z_OK == ret){
            output.resize(out_len);
            return true;
        }
        return false;
    }
    CCompressorZlib(const CCompressorZlib &);   //disable copy and assignment
    CCompressorZlib & operator =(const CCompressorZlib &);
    //members
    int level_;     //compress level of zlib, default to Z_DEFAULT_COMPRESSION, which means level 6
};

NS_SERVER_END

#endif
