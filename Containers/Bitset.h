#pragma once
#include "Base.h"
#include "Config.h"
#include "Math/Base.h"

template <u32 NumBits>
struct Bitset {
    // Wide for clarity reasons
    u8 bits[NumBits];

    Bitset(u32 from) {
        ASSERT(NumBits >= 32);

        for (u32 i = 0; i < 32; ++i) {
            bits[i] = (from >> i) & 1;
        }
    }

    u32 to_integral() const {
        u32 result = 0;
        for (i32 bit_index = 0; bit_index < NumBits; ++bit_index) {
            if (bits[bit_index])
                result += ipow(2, bit_index);
        }
        return result;
    }
};