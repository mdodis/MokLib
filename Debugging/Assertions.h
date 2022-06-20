#pragma once
#include "Base.h"

#if MOK_ENABLE_ASSERT
    #if OS_MSWINDOWS
        #include <assert.h>  // Use Visual Studio assert window
        #define ASSERT(expr) assert(expr)
    #else
        #define ASSERT(expr) do { \
                if (!(expr)) { \
                    DEBUG_PRINTF("Assertion failed: %s at %s:%d\n", #expr, __FILE__, __LINE__); \
                    DEBUG_BREAK(); \
                } \
            } while (0)
    #endif
#else
    #define ASSERT(expr)
#endif
