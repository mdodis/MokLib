#pragma once
#include "../Base.h"

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

IAllocator get_system_allocator(void);