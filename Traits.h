#pragma once
#include "Base.h"

template <typename T>
struct NumProps {
    // static constexpr T max;
    // static constexpr T min;
    // static constexpr T is_signed;
};

template <>
struct NumProps<u32> {
    static constexpr u32  max = 0xffffffff;
    static constexpr u32  min = 0x00000000;
    static constexpr bool is_signed = false;
};

template <>
struct NumProps<u64> {
    static constexpr u64  max = 0xffffffffffffffff;
    static constexpr u64  min = 0x0000000000000000;
    static constexpr bool is_signed = false;
};

template <>
struct NumProps<i32> {
    static constexpr i32  max = 0x7FFFFFFF;
    static constexpr i32  min = 0x80000000;
    static constexpr bool is_signed = true;
};

template <>
struct NumProps<i64> {
    static constexpr i64  max = 0x7FFFFFFFFFFFFFFF;
    static constexpr i64  min = 0x8000000000000000;
    static constexpr bool is_signed = true;
};