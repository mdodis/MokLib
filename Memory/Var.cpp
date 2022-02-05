#include "Var.h"
#include "Debugging/Base.h"

struct FreeBlockInfo {
    FreeBlockInfo *next, *prev;
    u64 size;
};

Var::Var(IAllocator& base, u64 size) : base(&base) {
    ASSERT(size > sizeof(FreeBlockInfo));

    total_size = size;
    memory = base.reserve(size);
    ASSERT(memory);

    FreeBlockInfo *free_block = (FreeBlockInfo*)memory;
    free_block->next = free_block->prev = 0;
    free_block->size = total_size - sizeof(FreeBlockInfo);
    first_free_block = free_block;
}

PROC_MEMORY_RESERVE(Var::reserve) {

    FreeBlockInfo *chosen_block = 0;
    for (FreeBlockInfo *it = first_free_block; it != 0; it = it->next) {
        if (it->size >= size) {
            chosen_block = it;
        }
    }

    if (!chosen_block)
        return 0;


    chosen_block->prev->next = chosen_block->next;
    chosen_block->next->prev = chosen_block->prev;



}

PROC_MEMORY_RESIZE(Var::resize) {
    return 0;
}

PROC_MEMORY_RELEASE(Var::release) {
}

PROC_MEMORY_RELEASE_BASE(Var::release_base) {
}
