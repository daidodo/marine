#ifndef DOZERG_COMPRESSOR_QUICKLZ_H_20090222
#define DOZERG_COMPRESSOR_QUICKLZ_H_20090222

/*
    Quicklz压缩算法封装
        CCompressorQuickLZ
//*/

#include <vector>
#include <string>

//压缩等级
#ifndef QLZ_COMPRESSION_LEVEL
// 1 gives fastest compression speed. 3 gives fastest decompression speed and best
// compression ratio.
//#   define QLZ_COMPRESSION_LEVEL 1
//#   define QLZ_COMPRESSION_LEVEL 2
#   define QLZ_COMPRESSION_LEVEL 3
#endif

//0: 不采用流式压缩
//其他: 采用流式压缩的buffer大小
//注意: QLZ_STREAMING_BUFFER过小可能会导致段错误
#ifndef QLZ_STREAMING_BUFFER
// If > 0, zero out both states prior to first call to qlz_compress() or qlz_decompress() 
// and decompress packets in the same order as they were compressed
#   define QLZ_STREAMING_BUFFER 0
//#   define QLZ_STREAMING_BUFFER 100000
#endif

// Guarantees that decompression of corrupted data cannot crash. Decreases decompression
// speed 10-20%. Compression speed not affected.
//#define QLZ_MEMORY_SAFE

#include "quicklz/quicklz.c"
#include "impl/environment.hh"

NS_SERVER_BEGIN

class CCompressorQuickLZ
{
    typedef ::qlz_state_compress __CompBuf;
    typedef ::qlz_state_decompress __DecompBuf;
public:
    CCompressorQuickLZ(){}
    bool compress(const std::vector<char> & input, std::vector<char> & output) const{
        return compressTemplate(input, output);
    }
    bool decompress(const std::vector<char> & input, std::vector<char> & output) const{
        return decompressTemplate(input, output);
    }
    bool compress(const std::vector<unsigned char> & input, std::vector<unsigned char> & output) const{
        return compressTemplate(input, output);
    }
    bool decompress(const std::vector<unsigned char> & input, std::vector<unsigned char> & output) const{
        return decompressTemplate(input, output);
    }
    bool compress(const std::vector<signed char> & input, std::vector<signed char> & output) const{
        return compressTemplate(input, output);
    }
    bool decompress(const std::vector<signed char> & input, std::vector<signed char> & output) const{
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
        output.resize(input.size() + 400);
        if(compBuf_.size() < sizeof(__CompBuf))
            compBuf_.resize(sizeof(__CompBuf));
        const size_t out_len = ::qlz_compress(
                &input[0],
                reinterpret_cast<char *>(&output[0]),
                input.size(),
                reinterpret_cast<__CompBuf *>(&compBuf_[0]));
        output.resize(out_len);
        return true;
    }
    template<class Buffer>
    bool decompressTemplate(const Buffer & input, Buffer & output) const{
        if(input.size() < 9)
            return false;
        const char * const buf = reinterpret_cast<const char *>(&input[0]);
        size_t out_len = ::qlz_size_decompressed(buf);
        output.resize(out_len);
        if(decompBuf_.size() < sizeof(__DecompBuf))
            decompBuf_.resize(sizeof(__DecompBuf));
        out_len = ::qlz_decompress(buf, &output[0], reinterpret_cast<__DecompBuf *>(&decompBuf_[0]));
        output.resize(out_len);
        return true;
    }
    CCompressorQuickLZ(const CCompressorQuickLZ &);     //disable copy and assignment
    CCompressorQuickLZ & operator =(const CCompressorQuickLZ &);
    //members
    mutable std::vector<char> compBuf_;
    mutable std::vector<char> decompBuf_;
};

NS_SERVER_END

#endif

