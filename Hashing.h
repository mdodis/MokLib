#pragma once
#include "Base.h"
#include "Str.h"

/**
 * MurmurHash2, by Austin Appleby
 */
u64 murmur_hash2(const void *key, u64 len, uint32 seed);

static _inline u64 hash_of(const Str &str, uint32 seed) {
    return murmur_hash2(str.data, str.len, seed);
}