#pragma once
#include "Host.h"

// @todo: better debug print function translation (use OutputDebugString for vs...)

/**
 * Basic
 */
#ifndef MOK_DEBUG
	#define MOK_DEBUG 1
#endif

#ifndef MOK_ENABLE_ASSERT
	#define MOK_ENABLE_ASSERT 1
#endif

#ifndef MOK_DEBUG_PRINTF
	#include <stdio.h>
	#define DEBUG_PRINTF(fmt, ...) fprintf(stderr, fmt, __VA_ARGS__)
#endif

#ifndef MOK_DEBUG_BREAK
	#define MOK_DEBUG_BREAK 1
#endif

#if MOK_DEBUG_BREAK
	#define DEBUG_BREAK() __debugbreak()
#else
	#define DEBUG_BREAK()
#endif

#if MOK_ENABLE_ASSERT
	#if OS_WINDOWS
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

#define UNIMPLEMENTED() ASSERT(false)
