#include "Str.h"
#include "Memory/Extras.h"
#include <string.h>

const Str Str::New_Line("\n", 1);
char Str::Null = '\0';
const Str Str::NullStr(0, 0);

Str::Str(const char *cstr) {
    data = (char*)cstr;
    len = (u64)strlen(cstr); // This struct is not meant to be used for huge strings
    has_null_term = true;
}

bool Str::split(u64 at, Str *left, Str *right) const {
    if (len < at) {
        return false;
    }

    if (left != 0) {
        left->data = data;
        left->len = at + 1;
    }

    if (right != 0) {
        right->data = data + at + 1;
        right->len = len - (at + 1);
    }

    return true;
}

u64 Str::last_of(char c) const{
    for (u64 i = len - 1; i >= 0; --i) {
        if (data[i] == c) {
            return i;
        }
    }

    return -1;
}

u64 Str::first_of(char c, u64 start) const {
    for (u64 i = start; i < len; ++i) {
        if (data[i] == c) {
            return i;
        }
    }
    return -1;
}

u64 Str::last_of(const Str &s, u64 start) const {
    if (s.len == 0) return len;

    if (start == StartEnd) start = len - 1;

    u64 sindex = s.len - 1;
    u64 i;
    for (i = start; i >= 0; --i) {
        
        if (data[i] == s[sindex]) {
            sindex--;
        } else {
            sindex = s.len - 1;
        }

        if (sindex == -1) {
            return i;
        }
    }

    return len;
}

u64 Str::first_of(const Str &s, u64 start) const {
    if (s.len == 0) return -1;
    
    u64 sindex = 0;
    u64 i;
    for (i = start; i < len; ++i) {
        if (data[i] == s[sindex]) {
            sindex++;
        } else {
            sindex = 0;
        }

        if (sindex == s.len) {
            return i - s.len + 1;
        }
    }

    return len;
}

bool Str::starts_with(const Str &needle) const {
    u64 i = 0;
    while ((i < needle.len) && (i < len) && (data[i] == needle[i])) {
        i++;
    }

    return (i == needle.len);
}

bool Str::ends_with(const Str &needle) const {

#if MOK_STR_RANGE_CHECK
    if ((len == 0) || (needle.len == 0)) return false;
#endif

    u64 mi = len - 1;
    u64 ni = needle.len - 1;
    while ((mi >= 0) && (ni >= 0)) {
        if (data[mi] != needle[ni]) {
            return false;
        }
        mi--;
        ni--;
    }

    if (mi < 0) {
        return false;
    }

    return true;
}

bool operator==(const Str &left, const Str &right) {
    if (left.len != right.len) return false;

    for (int i = 0; i < left.len; ++i) {

        if (left[i] != right[i]) {
            return false;
        }

    }

    return true;
}
