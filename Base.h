#pragma once
#include "Host.h"

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

#if COMPILER_MSVC
    #define _inline __forceinline
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
