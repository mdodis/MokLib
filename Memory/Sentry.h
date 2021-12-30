#pragma once
#include "Base.h"
#include "../Containers/Array.h"

/**
 * A debuggable allocator that protects separate allocations
 * through guard pages
 */
struct Sentry : IAllocator {

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

    Sentry(IAllocator *base, Mode mode) :mode(mode) {
        allocations = TArray<Allocation>(base);
    }

    virtual PROC_MEMORY_RESERVE(reserve) override;
    virtual PROC_MEMORY_RESIZE(resize)   override;
    virtual PROC_MEMORY_RELEASE(release) override;
    virtual _inline PROC_MEMORY_RELEASE_BASE(release_base) override {
        return;
    }

    Allocation *find_allocation(umm ptr, int32 *out_index = 0);

    Mode mode;
    TArray<Allocation> allocations;
};