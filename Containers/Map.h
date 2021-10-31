#pragma once
#include "../Base.h"
#include "Containers/List.h"
#include "Containers/Array.h"
#include "Memory/Base.h"
#include "Hashing.h"

template <typename TKey, typename TValue>
struct Map {

    _inline Map() {

    }

    _inline Map(const IAllocator &alloc) :alloc(alloc) {
        values = alloc.reserve(alloc.context, Num_Initial_Buckets * sizeof(TList<TValue>));
        num_buckets = Num_Initial_Buckets;
        memset(values, 0, sizeof(TList<TValue>) * num_buckets);
        ASSERT(values);
    }

    bool contains(const TKey &key) {
        uint32 h = hash_value(key, Hash_Seed) % num_buckets;
        return false;
    }

    void add(const TKey &key, const TValue &value) {
        uint32 h = hash_value(key, Hash_Seed) % num_buckets;

        if (values[h] == 0) {
            init_bucket_list(h);
        }
    }

    void init_bucket_list(uint32 index) {
        values[index] = TList<TKey>(alloc);
    }

    TList<TValue> *values;
    uint32 num_buckets;

    IAllocator alloc;
    static constexpr uint32 Hash_Seed = 0x1337;
    static constexpr uint32 Num_Initial_Buckets = 128;
};