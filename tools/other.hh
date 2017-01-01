#ifndef DOZERG_TOOLS_OTHER_H_20070905
#define DOZERG_TOOLS_OTHER_H_20070905

/*
    有用的类和函数
        Crc
        IsPrime
        PrimeLess
        CxxDemangle
//*/

#include <stdint.h>
#include <cxxabi.h>         //abi::__cxa_demangle
#include <algorithm>        //std::find
#include <numeric>          //std::accumulate
#include <cstdlib>          //free
#include <cassert>
#include <string>
#include <vector>
#include "../template.hh"

NS_SERVER_BEGIN

namespace tools
{
    //计算buf的crc
    //from blizzard
    template<typename Int>
    Int Crc(Int init, const char * buf, size_t sz)
    {
#define __DO1 init = (init << 1) + (init & kSign ? 1 : 0) + (*buf++ & 0xFF)
#define __DO4 __DO1;__DO1;__DO1;__DO1
#define __DO8 __DO4;__DO4
        const Int kSign = (Int(1) << (CTypeTraits<Int>::kMaxBits - 1));
        for(;sz >= 8;sz -= 8){
            __DO8;
        }
        for(;sz > 0;--sz){
            __DO1;
        }
        return init;
#undef __DO1
#undef __DO4
#undef __DO8
    }

    template<typename Int>
    Int Crc(Int init, const std::string & buf)
    {
        return Crc(init, &buf[0], buf.size());
    }

    template<typename Int>
    Int Crc(Int init, const std::vector<char> & buf)
    {
        return Crc(init, &buf[0], buf.size());
    }

    //返回v是否素数
    template<typename Int>
    bool IsPrime(Int v)
    {
        if(2 == v)
            return true;
        else if(v < 2 || 0 == (v & 1))
            return false;
        for(Int i = 3;i * i <= v;i += 2)
            if(0 == (v % i))
                return false;
        return true;
    }

    //返回小于等于v的最大素数
    //如果没有，返回0
    template<typename Int>
    Int PrimeLess(Int v)
    {
        if(v < 2)
            return 0;
        else if(2 == v)
            return 2;
        else if(0 == (v & 1))
            --v;
        for(;v > 3;v -= 2)
            if(IsPrime(v))
                return v;
        return 3;
    }

    //返回大于等于v的最小素数
    //return:
    //  0       超过Int最大值范围仍未找到
    //  其他    满足条件的素数
    template<typename Int>
    Int PrimeGreater(Int v)
    {
        if(v <= 2)
            return 2;
        if(0 == (v & 1)){
            if(v + 1 < v)
                return 0;   //overflow
            ++v;
        }
        for(;;v += 2){
            if(IsPrime(v))
                return v;
            if(v + 2 < v)
                break;   //overflow
        }
        return 0;
    }

    //生成若干个素数，放入results中，要求:
    //  1. 各个素数互不相等
    //  2. 总和 >= total
    //total: 所有素数的和必须大于或等于的数值
    //count: 素数的个数，尽量满足或者接近
    //results: 已有的素数，以及结果
    //  1. 如果results已有的数值有重复，不影响结果
    //  2. 如果results已有的数值不是素数，不影响结果
    //return:
    //  0       失败，total或count参数错误
    //  其他    成功，所有素数的和
    template<typename Int>
    uint64_t PrimesGenerator(uint64_t total, int count, std::vector<Int> & results)
    {
        if(count < 0)
            return 0;
        count = (size_t(count) > results.size() ? count - results.size() : 0);
        uint64_t ret = std::accumulate(results.begin(), results.end(), 0);
        for(;count > 0;--count){
            uint64_t left = (total > ret ? total - ret : 0);
            Int v = (left + count - 1) / count;
            for(;;++v){
                v = tools::PrimeGreater(v);
                if(results.end() == std::find(results.begin(), results.end(), v))
                    break;
            }
            results.push_back(v);
            ret += v;
        }
        return ret;
    }

    //将std::typeinfo::name()的返回值转换成可读的类型名
    inline std::string CxxDemangle(const char * name)
    {
        assert(name);
        int status = 0;
        char * output = abi::__cxa_demangle(name, NULL, NULL, &status);
        std::string ret;
        if(0 == status && output)
            ret = output;
        else
            ret = name;
        if(output)
            ::free(output);
        return ret;
    }

}//namespace tools

NS_SERVER_END

#endif

