#pragma once
#include "Base.h"

static uint32 Utf8_Last_Code_Point_Offset[] = {
    0x00000000UL, 0x00003080UL, 0x000E2080UL,
    0x03C82080UL, 0xFA082080UL, 0x82082080UL
};

#define isstart8(c) (((c) & 0xC0) != 0x80)

_inline uint32 next8(const char *s, uint32 *const i) {
    uint32 ch = 0;
    uint32 sz = 0u;
    do {
        ch <<= 6;
        ch += (unsigned char)s[(*i)++];
        sz++;
    } while (s[*i] && !isstart8(s[*i]));

    ch -= Utf8_Last_Code_Point_Offset[sz - 1];
    return ch;
}

_inline uint32 length8(const char *str) {
    uint32 i = 0u, sz = 0u;
    while (next8(str, &i)) sz++;
    return sz;
}

_inline uint32 inc8(const char *s) {
    uint32 i = 0u;
    return next8(s, &i);
}