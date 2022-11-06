#pragma once
#include "Base.h"
#include "Backtrace.h"
#include "DebugOutput.h"
// @todo: If we include this here, for some reason it doesnt compile
// #include "FileSystem/FileSystem.h"
// FileSystem depends on Str which depends on Assertions, cyclical incl dep

#ifndef MOK_ENABLE_ASSERT
    #define MOK_ENABLE_ASSERT 1
#endif

#if MOK_ENABLE_ASSERT

    #if OS_MSWINDOWS
        // #include <assert.h>  // Use Visual Studio assert window
        #define ASSERT(expr) \
            do { \
                if (!(expr)) {     \
                    DEBUG_PRINTF("Assertion failed: %s at %s:%d\n", #expr, __FILE__, __LINE__); \
                    print_backtrace(get_debug_tape()); \
                    DEBUG_BREAK(); \
                }                  \
            } while (0)
    #else
        #define ASSERT(expr)       \
            do {                   \
                if (!(expr)) {     \
                    DEBUG_PRINTF("Assertion failed: %s at %s:%d\n", #expr, __FILE__, __LINE__); \
                    print_backtrace(get_debug_tape()); \
                    DEBUG_BREAK(); \
                }                  \
            } while (0)

    #endif
#else
    #define ASSERT(expr)
#endif

#define UNIMPLEMENTED() ASSERT(false)