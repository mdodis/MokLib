#include "Parsing.h"

u64 parse_cid(const Str &s, u64 i, Str &out) {
    const char *begin = &s[i++];

    while (i < s.len && is_valid_cid(s[i])) {
        i++;
    }

    const char *end = &s[i];

    out = Str(begin, u64(end - begin));
    return i;
}

Str parse_cid(Tape *tape, IAllocator &alloc) {
    TArray<char> result(alloc);

    char c;
    while ((c = tape->read_char()) != EOF) {

        if (is_valid_cid(c)) {
            result.add(c);
        } else {
            break;
        }
    }

    tape->move(-1);

    if (result.size == 0) {
        return Str::NullStr;
    } else {
        return Str(result.data, result.size);
    }
}
