#pragma once
#include "../Base.h"
#include <stdlib.h>

#define PROC_MEMORY_RESERVE(name)       umm name(void *context, uint64 size)
#define PROC_MEMORY_RESIZE(name)        umm name(void *context, umm ptr, uint64 prev_size, uint64 new_size)
#define PROC_MEMORY_RELEASE(name)       void name(void *context, umm ptr)
#define PROC_MEMORY_RELEASE_BASE(name)  void name(void *context)

typedef PROC_MEMORY_RESERVE(ProcMemoryReserve);
typedef PROC_MEMORY_RESIZE(ProcMemoryResize);
typedef PROC_MEMORY_RELEASE(ProcMemoryRelease);
typedef PROC_MEMORY_RELEASE_BASE(ProcMemoryReleaseBase);

struct IAllocator {
    ProcMemoryReserve       *reserve;
    ProcMemoryResize        *resize;
    ProcMemoryRelease       *release;
    ProcMemoryReleaseBase   *release_base;
    void *context;
};

static _inline PROC_MEMORY_RESERVE(system_allocator_reserve) {
    return (umm)malloc(size);
}

static _inline PROC_MEMORY_RESIZE(system_allocator_resize) {
    return (umm)realloc(ptr, new_size);
}

static _inline PROC_MEMORY_RELEASE(system_allocator_release) {
    free(ptr);
}

static _inline PROC_MEMORY_RELEASE_BASE(system_allocator_release_base) {
    return;
}

static IAllocator system_allocator = {
    system_allocator_reserve,
    system_allocator_resize,
    system_allocator_release,
    system_allocator_release_base,
};

constexpr IAllocator get_system_allocator(void) {
    return system_allocator;
}
