#pragma once
#include "../Config.h"
#include "../Base.h"
#include <math.h>
#include <float.h>

/** Constants */
constexpr double Pi   = 3.14159265358979323846;
constexpr double Ln_2 = 0.693147180559945309417;

constexpr float To_Radians = 0.0174533f;
constexpr float Max_Float = 3.402823466e+38F;

template <typename T>
static _inline T min(T a, T b) { return a < b ? a : b; }

template <typename T>
static _inline T max(T a, T b) { return a > b ? a : b; }

template <typename T>
static _inline T clamp(T x, T a, T b) { return max(a, min(x, b)); }

/**
 * Approximation of e^x
 *
 * Ref: A Fast, Compact Approximation of the Exponential Function
 *      DOI: 10.1162/089976699300016467
 */
static _inline double exponent(double x) {
    union {
        double d;
        struct {
            // @note: Little Endian required
            i32 j, i;
        } n;
    } eco;

    const i32 a = i32(1048576 / Ln_2);
    const i32 b = 1072693248;
    const i32 c = 60801;

    eco.n.i = (i32)(a * x) + (1072693248 - c);
    return eco.d;
}

template <typename T>
static _inline constexpr T ipow(T x, int power) {
    T result = x;
    power--;
    while (power > 0) {
        result *= x;
        power--;
    }
    return result;
}

static _inline i32 absolute(i32 v) {
    return v > 0 ? v : -v;
}

template <typename T>
static _inline constexpr T sq(T x) {
    return x * x;
}

static _inline constexpr i32 truncate(float x) {
    return (i32)(x);
}

/** Equivalent of doing: i32(ceil(x)) */
static _inline i32 ceiltr(f32 x) {
    return (i32)ceil(x);
}

/** Equivalent of doing: i32(floor(x)) */
static _inline i32 floortr(f32 x) {
    return (i32)floor(x);
}

/** Remap value from the range [from1, to1] to [from2, to2] */
static _inline float remap(
    float value,
    float from1, float to1,
    float from2, float to2)
{
    return (value - from1) / (to1 - from1) * (to2 - from2) + from2;
}