#ifndef DOZERG_COMPRESSOR_ZLIB_H_20090223
#define DOZERG_COMPRESSOR_ZLIB_H_20090223

/*
    zlibÑ¹ËõËã·¨·â×°
        CCompressorZlib
//*/

#include <stdint.h>
#include <vector>
#include <string>
#include <cstring>
#include <zlib.h>
#include "tools/net.hh"   //Hton, Ntoh
#include "data_stream.hh" //varint

NS_SERVER_BEGIN

class CCompressorZlib
{
    typedef ::uLongf __ZSize;
    typedef ::Bytef  __ZChar;
public:
    CCompressorZlib(int level = -1):level_(level){}
    void setLevel(int lv){level_ = lv;}
    int getLevel() const{return level_;}
    bool compress(const std::vector<char> & input, std::vector<char> & output) const{
        return compressTemplate(input, output);
    }
    bool decompress(const std::vector<char> & input, std::vector<char> & output) const{
        return decompressTemplate(input, output);
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
    int level_;     //compress level of zlib, default to -1 which means level 6
};

NS_SERVER_END

#endif
