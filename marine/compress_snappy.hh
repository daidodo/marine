#ifndef DOZERG_COMPRESSOR_SNAPPY_H_20131210
#define DOZERG_COMPRESSOR_SNAPPY_H_20131210

/*
    snappyÑ¹ËõËã·¨·â×°
        CCompressorSnappy
//*/

#include <vector>
#include <snappy.h>
#include "impl/environment.hh"

NS_SERVER_BEGIN

class CCompressorSnappy
{
public:
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

