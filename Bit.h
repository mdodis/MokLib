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

#else
#warning "Bit intrin not present"
#endif