#ifndef DOZERG_TOOLS_OTHER_H_20070905
#define DOZERG_TOOLS_OTHER_H_20070905

/*
    ���õ���ͺ���
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
    //����buf��crc
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

    //����v�Ƿ�����
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

    //����С�ڵ���v���������
    //���û�У�����0
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

    //���ش��ڵ���v����С����
    //return:
    //  0       ����Int���ֵ��Χ��δ�ҵ�
    //  ����    ��������������
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

    //�������ɸ�����������results�У�Ҫ��:
    //  1. ���������������
    //  2. �ܺ� >= total
    //total: ���������ĺͱ�����ڻ���ڵ���ֵ
    //count: �����ĸ���������������߽ӽ�
    //results: ���е��������Լ����
    //  1. ���results���е���ֵ���ظ�����Ӱ����
    //  2. ���results���е���ֵ������������Ӱ����
    //return:
    //  0       ʧ�ܣ�total��count��������
    //  ����    �ɹ������������ĺ�
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

    //��std::typeinfo::name()�ķ���ֵת���ɿɶ���������
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

