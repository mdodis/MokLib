#pragma once
#include "Host.h"

/**
 * Basic
 */
#ifndef MOK_DEBUG
	#define MOK_DEBUG 1
#endif


#ifndef MOK_DEBUG_PRINTF
	#include <stdio.h>
	#include <stdarg.h>

	#if OS_MSWINDOWS
		extern "C" __declspec(dllimport) void OutputDebugStringA(const char *lpOutputString);
		#define DEBUG_PRINTF(fmt, ...) _dbg_printf(fmt, __VA_ARGS__)

		static void _dbg_printf(const char *fmt, ...) {
			char buffer[2048];
			va_list args;
			va_start(args, fmt);
			vsnprintf(buffer, sizeof(buffer), fmt, args);
			OutputDebugStringA(buffer);
			OutputDebugStringA("\n");
			va_end(args);
		}
	#else
		#define DEBUG_PRINTF(fmt, ...) printf(fmt, __VA_ARGS__)
	#endif
#endif

#ifndef MOK_DEBUG_BREAK
	#define MOK_DEBUG_BREAK 1
#endif

#if MOK_DEBUG_BREAK
	#if OS_MSWINDOWS
		#define DEBUG_BREAK() __debugbreak()
	#elif OS_LINUX
		#include <signal.h>
		#define DEBUG_BREAK() raise(SIGTRAP);
	#endif
#else
	#define DEBUG_BREAK()
#endif