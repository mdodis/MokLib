#pragma once
#include <stdlib.h>

#include "../Base.h"
#include "../Config.h"

namespace AllocatorMode {
    enum Type
    {
        Reserve,
        Resize,
        Release,
    };
}
typedef AllocatorMode::Type EAllocatorMode;

#define PROC_ALLOCATOR_ALLOCATE(name) \
    void* name(                       \
        void*          ptr,           \
        u64            prev_size,     \
        u64            new_size,      \
        EAllocatorMode mode,          \
        void*          usr)
typedef PROC_ALLOCATOR_ALLOCATE(ProcAllocatorAllocate);

struct MOKLIB_API Allocator {
    Allocator(ProcAllocatorAllocate* proc, void* usr) : proc(proc), usr(usr) {}

    ProcAllocatorAllocate* proc;
    void*                  usr;

    _inline void* reserve(u64 size)
    {
        return proc(nullptr, 0ull, size, AllocatorMode::Reserve, usr);
    }

    _inline void* resize(void* ptr, u64 prev_size, u64 new_size)
    {
        return proc(ptr, prev_size, new_size, AllocatorMode::Resize, usr);
    }

    _inline void release(void* ptr)
    {
        proc(ptr, 0ull, 0ull, AllocatorMode::Release, usr);
    }
};

struct MOKLIB_API SystemAllocator : public Allocator {
    SystemAllocator() : Allocator(allocate_proc, (void*)this) {}

    static PROC_ALLOCATOR_ALLOCATE(allocate_proc)
    {
        switch (mode) {
            case AllocatorMode::Reserve: {
                return malloc(new_size);
            } break;

            case AllocatorMode::Resize: {
                return realloc(ptr, new_size);
            } break;

            case AllocatorMode::Release: {
                free(ptr);
                return nullptr;
            } break;
        }

        return nullptr;
    }
};

struct MOKLIB_API NullAllocator : public Allocator {
    NullAllocator() : Allocator(allocate_proc, (void*)this) {}

    static PROC_ALLOCATOR_ALLOCATE(allocate_proc)
    {
        return nullptr;
    }
};

extern SystemAllocator System_Allocator;