#pragma once
#include "Base.h"

template <typename T>
struct NumProps {
    static const T      max = void;
    static const T      min = void;
    static const bool   is_signed = void;
};

template <>
struct NumProps<u32> {
    static const u32    max = 0xffffffff;
    static const u32    min = 0x00000000;
    static const bool   is_signed = false;
};

template <>
struct NumProps<u64> {
    static const u64    max = 0xffffffffffffffff;
    static const u64    min = 0x0000000000000000;
    static const bool   is_signed = false;
};

template <>
struct NumProps<i32> {
    static const i32    max = 0x7FFFFFFF;
    static const i32    min = 0x80000000;
    static const bool   is_signed = true;
};

template <>
struct NumProps<i64> {
    static const i64    max = 0x7FFFFFFFFFFFFFFF;
    static const i64    min = 0x8000000000000000;
    static const bool   is_signed = true;
};