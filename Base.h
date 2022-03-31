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
#if OS_MSWINDOWS
    #define DLL_EXPORT __declspec(dllexport)
    #define DLL_IMPORT __declspec(dllimport)
    #define CEXPORT extern "C" DLL_EXPORT
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
#define MCONCAT2(x, y, z) x##y##z
#define MSTR(x) #x

namespace U64 {
    enum {
        Max = ~0ull,
        Min = 0
    };
};

namespace U32 {
    enum {
        Max = ~0ul,
        Min = 0
    };
};

namespace I32 {
    enum {
        Max = 0x7FFFFFFF,
        Min = 0x80000000
    };
};

#if COMPILER_MSVC
    #pragma warning(disable : 4251)
#endif


/**
 * Converts a class's member proc to an explicitly called
 * proc.
 *
 * Beware: this does no checking whatsoever!
 */
template <typename Target, typename T, typename RetVal, typename... Args>
Target *as_proc(RetVal(T::*member_proc)(Args...)) {
    union {
        RetVal(T::*pf)(Args...);
        Target *p;
    };
    pf = member_proc;
    return p;
}