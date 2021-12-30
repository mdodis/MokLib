#include "Pool.h"
#include "../Debugging/Base.h"

Pool Pool::create(IAllocator *base, uint64 block_size, uint32 num_blocks) {
    Pool result = {};
    result.base = base;
    result.block_size = block_size;
    result.num_blocks = num_blocks;
    result.chunk_size = sizeof(Pool::FreeNode) + block_size;
    result.memory = base->reserve(result.chunk_size * num_blocks);
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

PROC_MEMORY_RELEASE(Pool::release) {

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

PROC_MEMORY_RELEASE_BASE(Pool::release_base) {
    base->release(memory);
}