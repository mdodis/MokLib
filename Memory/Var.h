#pragma once
#include "Base.h"
#include "Memory/Base.h"

struct Var : IAllocator {

    Var(IAllocator& base, u64 size = Default_Size);

    virtual PROC_MEMORY_RESERVE(reserve) override;
    virtual PROC_MEMORY_RESIZE(resize) override;
    virtual PROC_MEMORY_RELEASE(release) override;
    virtual PROC_MEMORY_RELEASE_BASE(release_base) override;



    IAllocator *base;
    u64 total_size;
    struct FreeBlockInfo *first_free_block;
    umm memory;
    static constexpr const u64 Default_Size = MEGABYTES(1);
};