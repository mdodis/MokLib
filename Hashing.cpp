#include "Hashing.h"

u64 murmur_hash2(const void *key, u64 len, uint32 seed) {
    // 'm' and 'r' are mixing constants generated offline.
    // They're not really 'magic', they just happen to work well.

    const u64 m = 0x5bd1e995;
    const u64 r = 24;

    // Initialize the hash to a 'random' value

    u64 h = seed ^ len;

    // Mix 4 bytes at a time into the hash

    const uint8 *data = (const uint8*)key;

    while (len >= 4) {
        uint32 k = *(uint32*)data;

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^= k;

        data += 4;
        len -= 4;
    }

    // Handle the last few bytes of the input array

    switch(len) {
        case 3: h ^= data[2] << 16;
        case 2: h ^= data[1] << 8;
        case 1: h ^= data[0];
                h *= m;
    };

    // Do a few final mixes of the hash to ensure the last few
    // bytes are well-incorporated.

    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    return h;
}
