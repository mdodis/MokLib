#pragma once
#include "Base.h"

#ifndef MOK_LIB_INTERNAL
    #define MOK_LIB_INTERNAL 0
#endif

#ifndef MOK_LIB_STATIC
    #define MOK_LIB_STATIC 0
#endif

#if MOK_LIB_STATIC
    #define MOKLIB_API
#else
    #if MOK_LIB_INTERNAL
        #define MOKLIB_API DLL_EXPORT
    #else
        #define MOKLIB_API DLL_IMPORT
    #endif
#endif