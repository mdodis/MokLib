#include "Types.h"
#include "Parsing.h"
#include <stdlib.h>

// uint32
PROC_STRINGIFY(stringify_uint32) {
    uint32 num = *((uint32*)ptr);

    if (num == 0) {
        output.append(STATIC_STR("0"));
        return;
    }

    char *buffer = (char*)allocator.reserve(allocator.context, 10);
    // memset(buffer, 0, 10);
    uint8 i = 9;

    while (num != 0) {
        uint8 digit = num % 10;

        buffer[i] = '0' + digit;

        num = num / 10;
        --i;
    }

    output.append(Str(buffer + i + 1, 9 - i));
}

PROC_DESTRINGIFY(destringify_uint32) {
    uint32 result = 0;

    int32 i = 0;
    while ((i < input.len) && is_digit(input[i])) {
        result *= 10;
        result += input[i] - '0';
        i++;
    }

    if (i == 0)
        return 0;

    *((uint32*)ptr) = result;
    return i;
}

// int32
PROC_STRINGIFY(stringify_int32) {
    int32 num = *((int32*)ptr);

    if (num == 0) {
        output.append(STATIC_STR("0"));
        return;
    }

    if (num < 0) {
        output.append(STATIC_STR("-"));
        num = -num;
    }

    uint32 unum = num;
    stringify_uint32((umm)&unum, output, allocator);
}

// float
PROC_STRINGIFY(stringify_float) {
    float f = *((float*)ptr);
    char *buffer = (char*)allocator.reserve(allocator.context, 16);

    snprintf(buffer, 16, "%f", f);

    output.append(Str(buffer));
}

PROC_DESTRINGIFY(destringify_float) {

    const char *start = (const char *)input.data;
    char *end = 0;
    float result = strtof(start, &end);

    *((float*)ptr) = result;

    return (int32)(end - start);

}
