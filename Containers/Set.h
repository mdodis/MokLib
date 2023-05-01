#pragma once
#include "Base.h"
#include "IntrusiveList.h"
#include "Memory/Base.h"
#include "Memory/Extras.h"

template <typename KeyType, u32 InitialBinCount = 64>
struct Set {
    struct Bin {
        KeyType              key;
        intrusive_list::Node node;
    };

    Set() {}
    Set(Allocator& allocator) { init(allocator); }

    void init(Allocator& allocator)
    {
        this->allocator = &allocator;
        num_bins        = InitialBinCount;

        bins = alloc_array<Bin>(*allocator, num_bins);
        memset(bins, 0, sizeof(Bin) * num_bins);
    }

    bool contains(const KeyType& key) const
    {
        u64 hash = hash_of(key, Hash_Seed) % num_bins;
        return bin_has_entry(bins + hash);
    }

    void add(const KeyType& key) {}

    bool bin_has_entry(const Bin* bin)
    {
        return (bin->node.next == 0) && (bin->node.prev == 0);
    }

    Allocator*              allocator = 0;
    Bin*                    bins      = 0;
    u32                     num_bins  = 0;
    static constexpr uint32 Hash_Seed = 0x1337;
};