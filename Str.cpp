#include "Str.h"
#include "Memory/Extras.h"
#include <string.h>

const Str Str::New_Line("\n", 1);
char Str::Null = '\0';
const Str Str::NullStr(0, 0);

Str::Str() {
    data = 0;
    len = 0;
}

Str::Str(const char *cstr) {
    data = (const uint8*)cstr;
    len = (int32)strlen(cstr) + 1; // This struct is not meant to be used for huge strings
}

bool Str::split(int32 at, Str *left, Str *right) const {
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

int32 Str::last_of(char c) {
    for (int i = len - 1; i >= 0; --i) {
        if (data[i] == c) {
            return i;
        }
    }

    return -1;
}

int32 Str::first_of(char c, int32 start) {
    for (int i = start; i < len; ++i) {
        if (data[i] == c) {
            return i;
        }
    }
    return -1;
}

int32 Str::last_of(const Str &s, int32 start) {
    if (s.len <= 0) return -1;

    if (start == -1) start = len - 1;

    int sindex = s.len - 1;
    int i;
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

    return -1;
}

int32 Str::first_of(const Str &s, int32 start) {
    if (s.len <= 0) return -1;
    
    int sindex = 0;
    int i;
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

    return -1;
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
