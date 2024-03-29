#pragma once
#include <stddef.h>

#include "Host.h"

/**
 * Generic Types
 */
#include <stdint.h>
typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;
typedef int8_t   int8;
typedef int16_t  int16;
typedef int32_t  int32;
typedef int64_t  int64;
typedef uint8_t* umm;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;

typedef float  f32;
typedef double f64;

typedef char*       CStr;
typedef const char* ConstCStr;

/**
 * C Compatibility
 */
#if OS_MSWINDOWS
#define DLL_EXPORT __declspec(dllexport)
#define DLL_IMPORT __declspec(dllimport)
#define CEXPORT extern "C" DLL_EXPORT
#endif

#if OS_LINUX
#define DLL_EXPORT
#define DLL_IMPORT
#define CEXPORT extern "C"
#endif

#ifndef CEXPORT
#define CEXPORT
#endif

/**
 * Miscellaneous
 */
#define ALIGN_UP2(X, P) ((X) + (P)-1) & ~((P)-1)
#define ALIGN_DOWN2(X, P) ((X) & ~((P)-1))

#define KILOBYTES(num) ((num)*1024)
#define MEGABYTES(num) KILOBYTES(((num)*1024))
#define GIGABYTES(num) MEGABYTES(((num)*1024))

#define ARRAY_COUNT(a) (sizeof(a) / sizeof((a)[0]))

#define UNUSED(x) ((void)(x))

#define MCONCAT(x, y) x##y
#define MCONCAT2(x, y, z) x##y##z

#define _MJOIN2(x, y) x##y
#define MJOIN2(x, y) _MJOIN2(x, y)
#define _MJOIN3(x, y, z) x##y##z
#define MJOIN3(x, y, z) _MJOIN3(x, y, z)

#define _MSTR(x) #x
#define MSTR(x) _MSTR(x)
#define MLITSTR(x) LIT(#x)
#define OFFSET_OF(type, element) ((size_t) & (((type*)0)->element))

#define CONTAINER_OF(ptr, type, member) \
    (type*)(((umm)ptr) - ((u64)OFFSET_OF(type, member)))

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
Target* as_proc(RetVal (T::*member_proc)(Args...))
{
    union {
        RetVal (T::*pf)(Args...);
        Target* p;
    };
    pf = member_proc;
    return p;
}

#define IS_A(what, T) (dynamic_cast<T*>(what) != 0)

template <typename T>
static _inline T& as(void* ptr)
{
    return *((T*)ptr);
}
