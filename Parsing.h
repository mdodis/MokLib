#pragma once
#include "Base.h"
#include "Containers/Array.h"
#include "Memory/Base.h"
#include "Str.h"
#include "Tape.h"
#include <stdlib.h>

static bool is_whitespace(char c) {
    return (c == ' ')
        || (c == '\t')
        || (c == '\n')
        || (c == '\r');
}

static _inline bool is_newline(char c) {
    return (c == '\n')
        || (c == '\r');
}

static _inline bool is_space(char c) {
    return (c == ' ')
        || (c == '\t');
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

static u64 eat_whitespace(const Str &s, u64 i) {
    while (i < s.len && is_whitespace(s[i])) {
        i++;
    }
    return i;
}

static u64 eat_line(const Str &s, u64 i = 0) {
    while (i < s.len && !is_newline(s[i])) {
        i++;
    }

    return i + 1;
}

static u32 match_strings(Str to, Str *strings, u32 num_strings) {
    for (u32 i = 0; i < num_strings; ++i) {
        if (to == strings[i]) {
            return i;
        }
    }

    return num_strings;
}


u64 parse_cid(const Str &s, u64 i, Str &out);

/**
 * Tape parsing
 */
static void eat_whitespace(struct Tape *tape) {
    char c = tape->read_char();
    while ((c != EOF) && (is_whitespace(c))) {
        c = tape->read_char();
    }

    if (c != EOF)
        tape->move(-1);
}

static void eat_line(struct Tape *tape) {
    char c = tape->read_char();

    while ((c != EOF) && (!is_newline(c))) {
        c = tape->read_char();
    }

    if (c != EOF)
        tape->move(-1);
}

static void eat_space(struct Tape *tape) {
    char c = tape->read_char();
    while ((c != EOF) && (is_space(c))) {
        c = tape->read_char();
    }

    if (c != EOF)
        tape->move(-1);
}

static bool match_string(struct Tape *tape, const Str &string) {
    char c;
    int32 i = 0;

    if (string.len == 0) {
        return false;
    }

    while ((c = tape->read_char()) != EOF) {
        if (string[i++] != c) {
            break;
        }

        if (i == string.len)
            return true;
    }

    tape->move(-i);
    return false;
}

static u32 match_strings(struct Tape *tape, Str *strings, u32 num_strings) {
    for (u32 i = 0; i < num_strings; ++i) {
        if (match_string(tape, strings[i])) {
            return i;
        }
    }

    return num_strings;
}

static Str parse_string(struct Tape *tape, IAllocator &alloc) {
    int32 i = 0;
    char c;

    TArray<char> result(alloc);
    while ((c = tape->read_char()) != EOF) {
        if (!is_whitespace(c)) {
            result.add(c);
        } else {
            break;
        }
    }

    tape->move(-1);
    return Str(result.data, result.size);
}

static bool parse_num(struct Tape *tape, float &result) {
    char c;
    char buf[16];
    i32 i = 0;

    while ((c = tape->read_char()) != EOF) {

        if (is_digit(c) || (c == '.') || (c == '-')) {
            if (i < 15) {
                buf[i++] = c;
            }
        } else {
            break;
        }
    }

    tape->move(-1);

    if (i == 0) {
        return false;
    }

    buf[i] = 0;

    result = strtof(buf, 0);
    return true;
}

static bool parse_num(struct Tape *tape, u32 &result) {
    char c;
    char buf[16];
    i32 i = 0;

    while ((c = tape->read_char()) != EOF) {

        if (is_digit(c)) {
            if (i < 15) {
                buf[i++] = c;
            }
        } else {
            break;
        }
    }

    tape->move(-1);

    if (i == 0) {
        return false;
    }

    buf[i] = 0;
    result = strtol(buf, 0, 10);

    return true;
}

Str parse_cid(struct Tape *tape, IAllocator &alloc);