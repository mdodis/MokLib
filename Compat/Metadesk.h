#pragma once
#if !defined(MD_H) && !defined(MD_VERSION_MAJ)
#error "Metadesk header must be included before this!"
#endif

#include "Str.h"

static _inline Str mds8_to_str(const MD_String8 &s8) {
    return Str((char*)s8.str, s8.size);
}

static _inline MD_String8 str_to_mds8(const Str &s) {
    MD_String8 result;
    result.str = (umm)s.data;
    result.size = s.len;
    return result;
}