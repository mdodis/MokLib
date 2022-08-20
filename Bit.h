#pragma once
#include "Base.h"

#if COMPILER_MSVC
#include <intrin.h>
static _inline int32 bit_scan(uint32 value) {
    unsigned long result;
    uint8 success = _BitScanForward(&result, value);

    return success
        ? (int32)result
        : -1;
}

#elif COMPILER_GCC || COMPILER_CLANG

static _inline int32 bit_scan(uint32 value) {
    int result = __builtin_ffs(*((int*)&value));
    return result == 0
        ? result
        : -1;
}

#else
#warning "Bit intrin not present"
#endif

static _inline uint32 bit_count(uint32 value) {
    uint32 result = 0;

    while (value) {
        result += value & 1;
        value >>= 1;
    }

    return result;
}