#pragma once
#include "Host.h"
#include "Base.h"

#if OS_MSWINDOWS
#include "WinInc.h"
#include <intrin.h>

#define MEMORY_BARRIER() do { _ReadBarrier(); _WriteBarrier(); MemoryBarrier(); } while(0)

_inline int32 compare_and_swap32(volatile int32 *ptr, int32 new_value, int32 expected) {
    return InterlockedCompareExchange((volatile LONG *)ptr, new_value, expected);
}

_inline uint32 compare_and_swapu32(volatile uint32 *ptr, uint32 new_value, uint32 expected) {
    return InterlockedCompareExchange((volatile LONG *)ptr, new_value, expected);
}

_inline int64 compare_and_swap64(volatile int64 *ptr, int64 new_value, int64 expected) {
    return InterlockedCompareExchange64(ptr, new_value, expected);
}

_inline void *compare_and_swap_ptr(void *volatile *ptr, void *new_value, void *expected) {
    return InterlockedCompareExchangePointer(ptr, new_value, expected);
}

_inline int32 inc_and_fetch(volatile int32 *addend) {
    return InterlockedIncrement((volatile LONG *)addend);
}

_inline u32 inc_and_fetch(volatile u32 *addend) {
    return InterlockedIncrement((volatile LONG *)addend);
}

_inline int32 add_and_fetch(volatile int32 *addend, int32 value) {
    return InterlockedAdd((volatile LONG *)addend, value);
}

#elif OS_LINUX

#define MEMORY_BARRIER() do {__sync_synchronize(); asm volatile("mfence": : :"memory"); } while(0)

_inline int32 compare_and_swap32(volatile int32 *ptr, int32 new_value, int32 expected) {
    return __sync_val_compare_and_swap(ptr, expected, new_value);
}

_inline uint32 compare_and_swapu32(volatile uint32 *ptr, uint32 new_value, uint32 expected) {
    return __sync_val_compare_and_swap(ptr, expected, new_value);
}

_inline int64 compare_and_swap64(volatile int64 *ptr, int64 new_value, int64 expected) {
    return __sync_val_compare_and_swap(ptr, expected, new_value);
}

_inline void *compare_and_swap_ptr(void *volatile *ptr, void *new_value, void *expected) {
    return __sync_val_compare_and_swap(ptr, expected, new_value);
}

_inline int32 inc_and_fetch(volatile int32 *addend) {
    return __sync_add_and_fetch(addend, 1);
}

_inline u32 inc_and_fetch(volatile u32 *addend) {
    return __sync_add_and_fetch(addend, 1);
}

_inline int32 add_and_fetch(volatile int32 *addend, int32 value) {
    return __sync_add_and_fetch(addend, value);
}

#else
    #error "No Atomics support for host OS"
#endif