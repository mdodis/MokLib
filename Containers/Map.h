#pragma once
#include <string.h>

#include "../Base.h"
#include "Containers/Array.h"
#include "Containers/List.h"
#include "Hashing.h"
#include "Memory/Base.h"

/**
 * A Simple Fixed-allocation Hashtable implementation, using Coalesced Chaining
 */
template <typename TKey, typename TValue>
struct TMap {
    struct Bin {
        TKey   key;
        TValue val;
        Bin*   next;
        bool   used;
    };

    _inline TMap() : alloc(System_Allocator) {}

    _inline TMap(IAllocator& alloc, uint32 init_size = Num_Total_Bins)
    {
        init(alloc, init_size);
    }

    void init(IAllocator& allocator, u32 init_size = Num_Total_Bins)
    {
        alloc  = allocator;
        values = (Bin*)alloc.reserve(init_size * sizeof(Bin));
        memset(values, 0, init_size * sizeof(Bin));

        num_bins       = uint32(float(init_size) * Bucket_Cellar_Factor);
        num_total_bins = init_size;

        ASSERT(values);
    }

    Bin* get_bin(const TKey& key)
    {
        uint32 h = hash_of(key, Hash_Seed) % num_bins;

        if (!values[h].used) return 0;

        Bin* next = &values[h];
        while ((next != 0) && !(next->key == key)) {
            next = values[h].next;
        }

        return next;
    }

    bool contains(const TKey& key)
    {
        Bin* bin = get_bin(key);
        return bin != 0;
    }

    void add(const TKey& key, const TValue& value)
    {
        uint32 h = hash_of(key, Hash_Seed) % num_bins;

        if ((!values[h].used) || values[h].key == key) {
            values[h].key  = key;
            values[h].val  = value;
            values[h].used = true;
        } else {
            uint32 cursor = num_total_bins - 1;
            while ((cursor >= 0) && (values[cursor].used) &&
                   (values[cursor].key != key))
            {
                --cursor;
            }

            if (cursor == -1) {
                return;
            }

            values[cursor].key  = key;
            values[cursor].val  = value;
            values[cursor].used = true;

            Bin* next = &values[h];
            while (values[h].next != 0) {
                next = values[h].next;
            }

            next->next = &values[cursor];
        }
    }

    TValue& at(const TKey& key)
    {
        Bin* bin = get_bin(key);
        ASSERT(bin);

        return bin->val;
    }

    TValue& operator[](const TKey& key) { return at(key); }

    const TValue& operator[](const TKey& key) const { return at(key); }

    Bin*   values;
    uint32 num_bins;
    uint32 num_total_bins;

    IAllocator&             alloc;
    static constexpr uint32 Hash_Seed            = 0x1337;
    static constexpr uint32 Num_Total_Bins       = 256;
    static constexpr float  Bucket_Cellar_Factor = 0.86f;
};