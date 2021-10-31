#pragma once

/**
 * Operating System
 */
#if defined(WIN32) || defined(_WIN32)
    #define OS_WINDOWS 1
#elif defined(__linux__)
    #define OS_LINUX 1
#endif

#ifndef OS_WINDOWS
    #define OS_WINDOWS 0
#endif

#ifndef OS_LINUX
    #define OS_LINUX 0
#endif

/**
 * Host Compiler
 */

#if defined(_MSC_VER) || defined(_MSC_FULL_VER)
    #define COMPILER_MSVC 1
#endif

#ifndef COMPILER_MSVC
    #define COMPILER_MSVC 0
#endif

#if COMPILER_MSVC
    #ifdef MAKE_DLL
        #define DLLEXPORT __declspec(dllexport)
    #else
        #define DLLEXPORT __declspec(dllimport)
    #endif
#endif

// @note: Protect against minwindef.h defining min/max
#if OS_WINDOWS

    #ifdef min
        #undef min
    #endif

    #ifdef max
        #undef max
    #endif

    #define min min
    #define max max
#endif