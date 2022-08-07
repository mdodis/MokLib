#pragma once

/**
 * Operating System
 */
#if defined(WIN32) || defined(_WIN32)
    #define OS_MSWINDOWS 1
#elif defined(__linux__)
    #define OS_LINUX 1
#endif

#ifndef OS_MSWINDOWS
    #define OS_MSWINDOWS 0
#endif

#ifndef OS_LINUX
    #define OS_LINUX 0
#endif

/**
 * Host Compiler
 */

#if defined(_MSC_VER) || defined(_MSC_FULL_VER)
    #define COMPILER_MSVC 1
#elif defined(__clang__)
    #define COMPILER_CLANG 1
#elif defined(__GNUC__) || defined(__GNUG__)
    #define COMPILER_GCC 1
#endif

#ifndef COMPILER_MSVC
    #define COMPILER_MSVC 0
#endif

#ifndef COMPILER_CLANG
    #define COMPILER_CLANG 0
#endif

#ifndef COMPILER_GCC
    #define COMPILER_GCC 0
#endif

#if COMPILER_MSVC
    #ifdef MAKE_DLL
        #define DLLEXPORT __declspec(dllexport)
    #else
        #define DLLEXPORT __declspec(dllimport)
    #endif
#endif

/**
 * Inline
 */
#if COMPILER_MSVC
    #define _inline __forceinline
#elif COMPILER_GCC || COMPILER_CLANG
    #define _inline inline __attribute__((always_inline))
#else
    #error "Unsupported compiler type for _inline!"
#endif

/**
 * Restrict
 */
#if COMPILER_MSVC
    #define _restrict __declspec(restrict)
#else
    #define _restrict
#endif

/**
 * Allocator
 */
#if COMPILER_MSVC
    #define _allocator __declspec(allocator)
#else
    #define _allocator
#endif
