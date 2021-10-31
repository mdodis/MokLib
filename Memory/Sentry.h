#pragma once
#include "Base.h"
#include "../Containers/Array.h"

/**
 * A debuggable allocator that protects separate allocations
 * through guard pages
 */
struct Sentry {

    struct Allocation {
        umm base_ptr;
        uint64 base_size;
        umm ptr;
        uint64 size;
    };

    enum class Mode {
        None,
        Overflow,
        Underflow
    };

    Sentry(IAllocator base, Mode mode) :mode(mode) {
        allocations = TArray<Allocation>(base);
    }

    umm push(uint64 size);
    umm resize(umm ptr, uint64 new_size);
    void pop(umm ptr);
    Allocation *find_allocation(umm ptr, int32 *out_index = 0);

    IAllocator to_alloc();

    Mode mode;
    TArray<Allocation> allocations;
};