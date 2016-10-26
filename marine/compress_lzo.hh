#ifndef DOZERG_COMPRESSOR_LZO_H_20080221
#define DOZERG_COMPRESSOR_LZO_H_20080221

/*
    LZOÑ¹ËõËã·¨·â×°
        CCompressorLzo
//*/

#include <stdint.h>
#include <vector>
#include <string>
#include <cstring>      //memcpy
#include "minilzo/minilzo.c"
#include "tools/net.hh" //Hton, Ntoh

NS_SERVER_BEGIN

class CCompressorLzo
{
    typedef ::lzo_uint  __Uint;
    typedef uint64_t __Len;
public:
    CCompressorLzo(){}
    bool compress(const std::vector<char> & input, std::vector<char> & output) const{
        return compressTemplate(input, output);
    }
    bool decompress(const std::vector<char> & input, std::vector<char> & output) const{
        return decompressTemplate(input,output);
    }
    bool compress(const std::vector<signed char> & input, std::vector<signed char> & output) const{
        return compressTemplate(input, output);
    }
    bool decompress(const std::vector<signed char> & input, std::vector<signed char> & output) const{
        return decompressTemplate(input, output);
    }
    bool compress(const std::vector<unsigned char> & input, std::vector<unsigned char> & output) const{
        return compressTemplate(input, output);
    }
    bool decompress(const std::vector<unsigned char> & input, std::vector<unsigned char> & output) const{
        return decompressTemplate(input, output);
    }
    bool compress(const std::string & input, std::string & output) const{
        return compressTemplate(input, output);
    }
    bool decompress(const std::string & input, std::string & output) const{
        return decompressTemplate(input, output);
    }
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

