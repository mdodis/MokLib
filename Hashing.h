#pragma once
#include "Base.h"
#include "Config.h"
#include "Str.h"

/**
 * MurmurHash2, by Austin Appleby
 */
u64 MOKLIB_API murmur_hash2(const void* key, u64 len, uint32 seed);

static _inline u64 hash_of(const Str& str, uint32 seed)
{
    return murmur_hash2(str.data, str.len, seed);
}

static _inline u64 hash_of(const i32& v, uint32 seed)
{
    char* as_str = (char*)&v;
    return murmur_hash2(as_str, 4, seed);
}

static _inline u64 hash_of(const u64& v, uint32 seed)
{
    char* as_str = (char*)&v;
    return murmur_hash2(as_str, 8, seed);
}