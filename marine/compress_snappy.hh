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
 * @brief [Snappy](https://github.com/google/snappy/blob/master/README) compression algorithm.
 * @author Zhao DAI
 */

#ifndef DOZERG_COMPRESSOR_SNAPPY_H_20131210
#define DOZERG_COMPRESSOR_SNAPPY_H_20131210

#include <vector>
#include <snappy.h>
#include "impl/environment.hh"

NS_SERVER_BEGIN

/**
 * @brief A convenient interface for [Snappy](https://github.com/google/snappy/blob/master/README)
 * compression algorithm.
 * You may need to link to Snappy library (e.g. @c -lsnappy) if you use @c CCompressorSnappy.
 */
class CCompressorSnappy
{
public:
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
        size_t sz = snappy::MaxCompressedLength(input.size());
        output.resize(sz);
        snappy::RawCompress(reinterpret_cast<const char *>(&input[0]), input.size(), reinterpret_cast<char *>(&output[0]), &sz);
        output.resize(sz);
        return true;
    }
    template<class Buffer>
    bool decompressTemplate(const Buffer & input, Buffer & output) const{
        if(!snappy::IsValidCompressedBuffer(reinterpret_cast<const char *>(&input[0]), input.size()))
            return false;
        size_t sz = 0;
        if(!snappy::GetUncompressedLength(reinterpret_cast<const char *>(&input[0]), input.size(), &sz))
            return false;
        output.resize(sz);
        snappy::RawUncompress(reinterpret_cast<const char *>(&input[0]), input.size(), reinterpret_cast<char *>(&output[0]));
        return true;
    }
};

NS_SERVER_END

#endif

