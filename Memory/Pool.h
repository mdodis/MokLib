#pragma once
#include "Base.h"

/**
 * Memory Pool
 *
 * @todo: alignment
 */
struct Pool : IAllocator {

    static Pool create(IAllocator *base, uint64 block_size, uint32 num_blocks);

    virtual _inline PROC_MEMORY_RESERVE(reserve) override {
        if (block_size != size)
            return 0;
        return push();
    }

    virtual _inline PROC_MEMORY_RESIZE(resize) override {
        return 0;
    }

    virtual PROC_MEMORY_RELEASE(release) override;
    virtual PROC_MEMORY_RELEASE_BASE(release_base) override;

    umm push();
    void erase();

    IAllocator to_alloc();

    struct FreeNode {
        FreeNode *next;
    };

    IAllocator *base;
    umm memory;
    uint64 capacity, block_size;
    uint32 num_blocks;

    uint64 chunk_size = 0;
    FreeNode *head = 0;
};