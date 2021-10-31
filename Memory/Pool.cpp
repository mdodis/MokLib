#include "Pool.h"
#include "../Debugging.h"

Pool Pool::create(IAllocator base, uint64 block_size, uint32 num_blocks) {
    Pool result = {};
    result.base = base;
    result.block_size = block_size;
    result.num_blocks = num_blocks;
    result.chunk_size = sizeof(Pool::FreeNode) + block_size;
    result.memory = base.reserve(base.context, result.chunk_size * num_blocks);
    result.capacity = result.chunk_size * num_blocks;
    result.erase();

    return result;
}

umm Pool::push() {
    Pool::FreeNode *node = head;

    if (node == 0)
        return 0;

    head = head->next;

    umm result_block = ((umm)node) + sizeof(Pool::FreeNode);
    return result_block;
}

void Pool::release(umm ptr) {

    const umm start = memory;
    const umm end   = memory + capacity;

    umm node_ptr = ptr - sizeof(Pool::FreeNode);
    if ((node_ptr < start) || (node_ptr > end)) {
        ASSERT(false);
    }

    Pool::FreeNode *free_node = (Pool::FreeNode*)node_ptr;

    free_node->next = head;
    head = free_node;
}

void Pool::erase() {
    for (uint32 i = 0; i < num_blocks; ++i) {
        umm chunk_ptr = &memory[i * chunk_size];

        Pool::FreeNode *free_node = (Pool::FreeNode*)chunk_ptr;
        free_node->next = head;
        head = free_node;
    }
}

void Pool::release_base() {
    base.release(base.context, memory);
}

PROC_MEMORY_RESERVE(pool_reserve) {
    Pool *pool = (Pool*)context;
    if (pool->block_size != size)
        return 0;
    return pool->push();
}

PROC_MEMORY_RESIZE(pool_resize) {
    return 0;
}

PROC_MEMORY_RELEASE(pool_release) {
    Pool *pool = (Pool*)context;
    pool->release(ptr);
}

PROC_MEMORY_RELEASE_BASE(pool_release_base) {
    Pool *pool = (Pool*)context;
    pool->release_base();
}

IAllocator Pool::to_alloc() {
    return IAllocator {
        pool_reserve,
        pool_resize,
        pool_release,
        pool_release_base,
        (void*)this
    };
}
