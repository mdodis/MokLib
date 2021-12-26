#pragma once
#include "Host.h"
#include <stddef.h>

/**
 * Generic Types
 */
#include <stdint.h>
typedef uint8_t		uint8;
typedef uint16_t	uint16;
typedef uint32_t	uint32;
typedef uint64_t	uint64;
typedef int8_t		int8;
typedef int16_t		int16;
typedef int32_t		int32;
typedef int64_t		int64;
typedef uint8_t*	umm;

typedef uint8_t     u8;
typedef uint16_t    u16;
typedef uint32_t    u32;
typedef uint64_t    u64;
typedef int8_t      i8;
typedef int16_t     i16;
typedef int32_t     i32;
typedef int64_t     i64;

typedef float       f32;
typedef double      f64;


#if COMPILER_MSVC
    #define _inline __forceinline
#elif COMPILER_GCC
    #define _inline inline __attribute__((always_inline))
#else
    #error "Unsupported compiler type for _inline!"
#endif

/**
 * C Compatibility
 */
#if OS_WINDOWS
    #define EXPORT __declspec(dllexport)
    #define CEXPORT extern "C" EXPORT
#endif

#if OS_LINUX

    #define CEXPORT extern "C"
#endif

#ifndef CEXPORT
    #define CEXPORT
#endif

/**
 * Miscellaneous
 */
#define ALIGN_UP2(X, P) ((X) + (P) - 1) & ~((P) - 1)
#define ALIGN_DOWN2(X, P) ((X) & ~((P) - 1))

#define KILOBYTES(num) ((num) * 1024)
#define MEGABYTES(num) KILOBYTES(((num) * 1024))
#define GIGABYTES(num) MEGABYTES(((num) * 1024))

#define ARRAY_COUNT(a) (sizeof(a) / sizeof((a)[0]))

#define UNUSED(x) ((void)(x))

#define MCONCAT(x, y) x##y
#define MSTR(x) #x

namespace U64 {
    enum {
        Max = ~0ull,
        Min = 0
    };
};