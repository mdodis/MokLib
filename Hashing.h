#pragma once
#include "Base.h"
#include "Str.h"

/**
 * MurmurHash2, by Austin Appleby
 */
uint32 murmur_hash2(const void *key, int32 len, uint32 seed);

static _inline uint32 hash_of(const Str &str, uint32 seed) {
    return murmur_hash2(str.data, str.len, seed);
}