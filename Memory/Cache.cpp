#include "Cache.h"
#include <Containers/Array.h>
#include <Containers/List.h>
#include <Thread/ThreadContext.h>

void CacheBlock::init(u64 handle, u64 size) {
    this->handle = handle;
    this->size = size;
    make_list_node(&node);
    make_list_node(&free_node);
}

void Cache::init() {
    base_size = MEGABYTES(1);
    ptr = alloc.reserve(base_size);
    ASSERT(ptr);

    CacheBlock *first_block = (CacheBlock*)ptr;
    first_block->init(invalid_handle, base_size - sizeof(CacheBlock));

    make_list_node(&first);
    make_list_node(&free_first);
}

umm Cache::get(u64 handle, u64 size) {
    if (base_size == 0) {
        init();
    }

    if (CacheBlock *block = find_block(handle, size)) {
        return (umm)block + sizeof(CacheBlock);
    }

    CacheBlock *block = create_contiguous_block(size);

    return 0;
}

CacheBlock *Cache::create_contiguous_block(u64 size) {
    MAKE_TEMP_ARENA(temp, 100 * sizeof(umm));
    TArray<CacheBlock*> blocks(&temp);

    u64 total = 0;
    u32 num_blocks = 0;
    ListNode *next = free_first.next;

    // Iterate through free list
    while (total < size) {

        total = 0;
        blocks.empty();

        ListNode *c = next;
        CacheBlock *b = CONTAINER_OF(c, CacheBlock, free_node);
        ListNode *curr = &b->free_node;

        // Iterate through node list
        // Search for next block if it is free
        while (total < size) {

            CacheBlock *block = CONTAINER_OF(curr, CacheBlock, node);
            total += block->size;
            blocks.add(block);

            curr = curr->next;

            // if (curr->handle != invalid_handle)
            //     break;

            if (curr == next)
                break;
        }

        next = next->next;

        if (next == &free_first)
            break;
    }

    // @note: For now we expect to always free enough blocks to get something
    ASSERT(total >= size);

    CacheBlock *first_block = blocks[0];
    CacheBlock *last_block = *blocks.last();

    for (u64 i = 1; i < blocks.size; ++i) {
        CacheBlock *block = blocks[i];
        // list_node_remove(block->node);
        // list_node_remove(block->free_node);
    }

    return 0;
}

CacheBlock *Cache::find_block(u64 handle, u64 size) {
    // CacheBlock *result = first;
    // while (result->handle != handle) {
    //     result = result->next;
    // 
    //     if (result == first)
    //         break;
    // }
    // 
    // if (result->size != size)
    //     return 0;
    // return result;
    return 0;
}
