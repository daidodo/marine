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
 * @brief [miniLZO](http://www.oberhumer.com/opensource/lzo/#minilzo) compression algorithm.
 * @author Zhao DAI
 */

#ifndef DOZERG_COMPRESSOR_LZO_H_20080221
#define DOZERG_COMPRESSOR_LZO_H_20080221

#include <stdint.h>
#include <vector>
#include <string>
#include <cstring>      //memcpy
#include "minilzo/minilzo.c"
#include "tools/net.hh" //Hton, Ntoh

NS_SERVER_BEGIN

/**
 * @brief A convenient interface for [miniLZO](http://www.oberhumer.com/opensource/lzo/#minilzo)
 * compression algorithm.
 * @note CCompressorLzo object can @em NOT be copied.
 */
class CCompressorLzo
{
    typedef ::lzo_uint  __Uint;
    typedef uint64_t __Len;
public:
    /**
     * @brief Default constructor.
     */
    CCompressorLzo(){}
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
        return decompressTemplate(input,output);
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
    static bool init(){
        static const bool ok = (LZO_E_OK == ::lzo_init());
        return ok;
    }
    template<class Buffer>
    bool compressTemplate(const Buffer & input, Buffer & output) const{
        if(!init())
            return false;
        __Uint out_len = __Uint(outLength(input.size()));
        output.resize(sizeof(__Len) + size_t(out_len));
        const __Uint in_len = __Uint(input.size());
        if(workmem_.size() < LZO1X_1_MEM_COMPRESS)
            workmem_.resize(LZO1X_1_MEM_COMPRESS);
        if(LZO_E_OK != ::lzo1x_1_compress(
                    reinterpret_cast<const unsigned char *>(&input[0]),
                    in_len,
                    reinterpret_cast<unsigned char *>(&output[sizeof(__Len)]),
                    &out_len,
                    &workmem_[0]))
            return false;
        output.resize(sizeof(__Len) + out_len);
        const __Len lsz = tools::Hton(__Len(in_len));
        memcpy(&output[0], &lsz, sizeof lsz);
        return true;
    }
    template<class Buffer>
    bool decompressTemplate(const Buffer & input, Buffer & output) const{
        if(input.size() < sizeof(__Len))
            return false;
        __Uint out_len = __Uint(tools::Ntoh(*reinterpret_cast<const __Len *>(&input[0])));
        output.resize(out_len);
        if(LZO_E_OK != ::lzo1x_decompress(
                    reinterpret_cast<const unsigned char *>(&input[sizeof(__Len)]),
                    __Uint(input.size() - sizeof(__Len)),
                    reinterpret_cast<unsigned char *>(&output[0]),
                    &out_len,
                    NULL))
            return false;
        output.resize(out_len);
        return true;
    }
    //link: http://www.oberhumer.com/opensource/lzo/lzofaq.php
    size_t outLength(size_t inLen) const{
        return inLen + (inLen >> 4) + 67;
    }
    CCompressorLzo(const CCompressorLzo &);     //disable copy and assignment
    CCompressorLzo & operator =(const CCompressorLzo &);
    //members
    mutable std::vector<unsigned char>  workmem_;   //memory required for the wrkmem parameter
};

NS_SERVER_END

#endif

