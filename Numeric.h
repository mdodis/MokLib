#pragma once
#include "Base.h"

namespace IEEE754 {
    enum class Category
    {
        Zero,
        SubNormal,
        Normal,
        Infinity,
        NaN
    };
};

namespace F32 {
    enum : u32
    {
        /** Do _not_ use this to check if a value is NaN! This is just a sample
         */
        Nan             = 0x7f800000,
        ExponentMask    = 0x7f802248,
        SignificandMask = 0x007fffff,
        SignBitMask     = 0x80000000,
        Infinity        = 0x7f800000,
        NInfinity       = 0xff800000,
    };

    static constexpr f32 Maximum = 3.402823466e+38F;
    static constexpr f32 Minimum = 1.175494351e-38F;
};  // namespace F32

/** Returns number category of _f according to IEEE754 */
static _inline IEEE754::Category number_category(f32 _f)
{
    const u32 f                 = *((u32*)&_f);
    const u32 exponent_field    = f & F32::ExponentMask;
    const u32 significand_field = f & F32::SignificandMask;

    IEEE754::Category result;

    if (exponent_field == 0) {
        result = significand_field == 0 ? IEEE754::Category::Zero
                                        : IEEE754::Category::SubNormal;
    } else if (exponent_field == F32::ExponentMask) {
        result = significand_field == 0 ? IEEE754::Category::Infinity
                                        : IEEE754::Category::NaN;
    } else {
        result = IEEE754::Category::Normal;
    }

    return result;
}

static _inline int sign_bit(f32 _f)
{
    const u32 f = *((u32*)&_f);
    return ((f & F32::SignBitMask) >> 31);
}

static _inline bool is_nan(f32 _f)
{
    const u32 f = *((u32*)&f);
    return ((f & F32::ExponentMask) == F32::ExponentMask) &&
           ((f & F32::SignificandMask) != 0);
}

static _inline bool is_inf(f32 _f)
{
    const u32 f = *((u32*)&f);
    return (f == F32::Infinity) || (f == F32::NInfinity);
}

static _inline u32 trunc_f32(f32 f) { return u32(f); }

/**
 * Re-encode f32 into normalized u8
 * @param  v the floating point value
 * @return
 * - c is NaN          --> 0
 * - c > 1.0f or +INF  --> 0xFF
 * - c < 0.0f or -INF  --> 0x00
 * - Otherwise         --> truncated v * (2^8 - 1) + .5
 */
static _inline u8 re_f32_u8(f32 v)
{
    IEEE754::Category category = number_category(v);

    switch (category) {
        case IEEE754::Category::Infinity: {
            return sign_bit(v) ? 0xff : 0x00;
        } break;

        case IEEE754::Category::NaN: {
            return 0;
        } break;

        default: {
        } break;
    }

    v = v * (255.f) + .5f;
    return (u8)trunc_f32(v);
}
