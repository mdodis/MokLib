#pragma once
#include "Base.h"
#include "Str.h"

static bool is_whitespace(char c) {
    return (c == ' ')
        || (c == '\t')
        || (c == '\n')
        || (c == '\r');
}

static bool is_digit(char c) {
    return (c >= '0' && c <= '9');
}

static bool is_valid_cid_begin(char c) {
    return (c >= 'A' && c <= 'Z')
        || (c >= 'a' && c <= 'z')
        || (c == '_');
}

static bool is_valid_cid(char c) {
    return is_valid_cid_begin(c)
        || (c >= '0' && c <= '9');
}

static int32 eat_whitespace(const Str &s, int32 i) {
    while (i < s.len && is_whitespace(s[i])) {
        i++;
    }
    return i;
}

int32 parse_cid(const Str &s, int32 i, Str &out);