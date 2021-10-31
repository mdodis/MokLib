#pragma once
#include "Base.h"

/**
 * Memory Pool
 *
 * @todo: alignment
 */
struct Pool {

    static Pool create(IAllocator base, uint64 block_size, uint32 num_blocks);

    umm push();
    void release(umm ptr);
    void erase();
    void release_base();

    IAllocator to_alloc();

    struct FreeNode {
        FreeNode *next;
    };

    IAllocator base;
    umm memory;
    uint64 capacity, block_size;
    uint32 num_blocks;

    uint64 chunk_size = 0;
    FreeNode *head = 0;
};