#include <stdint.h>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <vector>

const uint32_t kNode = 500;
const uint32_t kVNode = 1500;

class CBitset
{
public:
    CBitset():bits_(1 << 29){}
    void set(uint32_t index){
        bits_[index >> 3] |= (1 << (index & 7));
    }
    void reset(uint32_t index){
        bits_[index >> 3] &= ~(1 << (index & 7));
    }
    bool test(uint32_t index) const{
        return (0 != (bits_[index >> 3] & (1 << (index & 7))));
    }
private:
    std::vector<char> bits_;
};

static uint32_t Hash32bit_Simple(uint32_t);

static uint32_t hash_1(uint32_t key, uint32_t index, uint32_t last)
{
    if(!index && !last)
        last = key;
    return Hash32bit_Simple(last + index);
}

static uint32_t hash_2(uint32_t key, uint32_t index, uint32_t last)
{
    return Hash32bit_Simple(last + key + index);
}

static uint32_t hash_3(uint32_t key, uint32_t index, uint32_t last)
{
    return Hash32bit_Simple(last + key + (index << 16) + (index >> 16));
}

static uint32_t u2013(uint32_t i)
{
    return (((i & 0xFF) << 16)
        | (i & 0xFF00)
        | ((i << 8) & 0xFF000000)
        | ((i >> 24) & 0xFF));
}

static uint32_t hash_4(uint32_t key, uint32_t index, uint32_t last)
{
    return Hash32bit_Simple(last + key + u2013(index));
}

static uint32_t Rotate32(uint32_t val, int shift)
{
    // Avoid shifting by 32: doing so yields an undefined result.
    return (shift == 0 ? val : ((val >> shift) | (val << (32 - shift))));
}

static uint32_t hash_5(uint32_t key, uint32_t index, uint32_t last)
{
    uint32_t a = (!index && !last ? key : last);
    uint32_t h = index;
    a *= 0xcc9e2d51;
    a = Rotate32(a, 17);
    a *= 0x1b873593;
    h ^= a;
    h = Rotate32(h, 19);
    return Hash32bit_Simple(h * 5 + 0xe6546b64);
}

typedef uint32_t (*__Hash)(uint32_t key, uint32_t index, uint32_t last);

static const __Hash kHash[] = {hash_1, hash_2, hash_3, hash_4, hash_5};

