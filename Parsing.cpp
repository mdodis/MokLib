#include "Parsing.h"

int32 parse_cid(const Str &s, int32 i, Str &out) {
    const char *begin = &s[i++];

    while (i < s.len && is_valid_cid(s[i])) {
        i++;
    }

    const char *end = &s[i];

    out = Str(begin, (int32)(end - begin));
    return i;
}