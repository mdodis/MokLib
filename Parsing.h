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

static int32 eat_whitespace(const Str &s, int32 i) {
    while (i < s.len && is_whitespace(s[i])) {
        i++;
    }
    return i;
}

static int32 eat_line(const Str &s, int32 i = 0) {
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


int32 parse_cid(const Str &s, int32 i, Str &out);

/**
 * Tape parsing
 */

static void eat_whitespace(Tape *tape) {
    char c = tape->read_char();
    while ((c != EOF) && (is_whitespace(c))) {
        c = tape->read_char();
    }

    tape->move(-1);
}

static void eat_line(Tape *tape) {
    char c = tape->read_char();

    while ((c != EOF) && (!is_newline(c))) {
        c = tape->read_char();
    }
}

static void eat_space(Tape *tape) {
    char c = tape->read_char();
    while ((c != EOF) && (is_space(c))) {
        c = tape->read_char();
    }
    tape->move(-1);
}

static bool match_string(Tape *tape, const Str &string) {
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

static u32 match_strings(Tape *tape, Str *strings, u32 num_strings) {
    for (u32 i = 0; i < num_strings; ++i) {
        if (match_string(tape, strings[i])) {
            return i;
        }
    }

    return num_strings;
}

static Str parse_string(Tape *tape, IAllocator &alloc) {
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

static bool parse_num(Tape *tape, float &result) {
    char c;
    char buf[16];
    i32 i = 0;

    while ((c = tape->read_char()) != EOF) {

        if (is_digit(c) || (c == '.') || (c == '-')) {
            buf[i++] = c;
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

static bool parse_num(Tape *tape, u32 &result) {
    char c;
    char buf[16];
    i32 i = 0;

    while ((c = tape->read_char()) != EOF) {

        if (is_digit(c)) {
            buf[i++] = c;
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