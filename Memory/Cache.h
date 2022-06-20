#pragma once
#include "../Config.h"
#include "Base.h"
#include "Traits.h"
#include "Containers/List.h"

struct CacheBlock {
    u64 handle;
    u64 size;

    ListNode node;
    ListNode free_node;

    void init(u64 handle, u64 size);
};

struct MOKLIB_API Cache {
    Cache(IAllocator &alloc)
        : alloc(alloc)
        , ptr(0)
        , base_size(0)
        , first({})
        , free_first({})
        {}

    void init();
    umm get(u64 handle, u64 size);
    CacheBlock *find_block(u64 handle, u64 size);
    CacheBlock *create_contiguous_block(u64 size);

    umm ptr;
    u64 base_size;
    ListNode first;
    ListNode free_first;
    static constexpr u64 invalid_handle = NumProps<u64>::max;
    IAllocator &alloc;
};