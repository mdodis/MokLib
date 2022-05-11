#pragma once
#include "../Base.h"
#include "../Config.h"
#include <stdlib.h>

#define PROC_MEMORY_RESERVE(name)       umm name(uint64 size)
#define PROC_MEMORY_RESIZE(name)        umm name(umm ptr, uint64 prev_size, uint64 new_size)
#define PROC_MEMORY_RELEASE(name)       void name(umm ptr)
#define PROC_MEMORY_RELEASE_BASE(name)  void name()

struct MOKLIB_API IAllocator {
    virtual PROC_MEMORY_RESERVE(reserve) = 0;
    virtual PROC_MEMORY_RESIZE(resize) = 0;
    virtual PROC_MEMORY_RELEASE(release) = 0;
    virtual PROC_MEMORY_RELEASE_BASE(release_base) = 0;

    template <typename T>
    _inline T *reserve(u64 count) {
        return (T*)reserve(count * sizeof(T));
    }

    _inline void release(void *ptr) { release((umm)ptr); }
};

struct SystemAllocator : IAllocator {
    virtual _inline PROC_MEMORY_RESERVE(reserve) override {
        return (umm)malloc(size);
    }

    virtual _inline PROC_MEMORY_RESIZE(resize) override {
        return (umm)realloc(ptr, new_size);
    }

    virtual _inline PROC_MEMORY_RELEASE(release) override {
        free(ptr);
    }

    virtual _inline PROC_MEMORY_RELEASE_BASE(release_base) override {
        return;
    }
};

struct NullAllocator : IAllocator {
    virtual _inline PROC_MEMORY_RESERVE(reserve) override {
        return 0;
    }

    virtual _inline PROC_MEMORY_RESIZE(resize) override {
        return 0;
    }

    virtual _inline PROC_MEMORY_RELEASE(release) override {
        return;
    }

    virtual _inline PROC_MEMORY_RELEASE_BASE(release_base) override {
        return;
    }
};

static SystemAllocator  System_Allocator;
static NullAllocator    Null_Allocator;

constexpr IAllocator *get_system_allocator(void) {
    return (IAllocator*)(&System_Allocator);
}
