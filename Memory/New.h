#include "Base.h"
#include "Memory/Base.h"
#include <new.h>

#define NEW(alloc, T, ...) new (new_memory_object(alloc, sizeof(T))) T (__VA_ARGS__)

static _inline void *new_memory_object(IAllocator &alloc, uint32 sz) {
    void *memory = alloc.reserve(alloc.context, sz);
    return memory;
}