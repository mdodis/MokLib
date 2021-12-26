#pragma once
#include "Base.h"
#include "Types.h"
#include "Parsing.h"

static PROC_STRINGIFY(stringify_vec2);
static PROC_DESTRINGIFY(destringify_vec2);
static constexpr _inline IType type_of(const Vec2 &v) {
    return IType {
        "Vec2",
        stringify_vec2,
        destringify_vec2,
    };
}

static PROC_STRINGIFY(stringify_vec2) {
    Vec2 *v = (Vec2*)ptr;

    output.append(STATIC_STR("["));

    stringify_float((umm)&v->x, output, allocator);
    output.append(STATIC_STR(","));
    stringify_float((umm)&v->y, output, allocator);

    output.append(STATIC_STR("]"));
}

static PROC_DESTRINGIFY(destringify_vec2) {
    Vec2 result;
    u64 i = 0;

    if (input[i] != '[')
        return 0;
    i++;

    i += destringify_float((umm)&result.x, input.chop_left(i - 1));

    i = eat_whitespace(input, i);

    if (input[i] != ',')
        return 0;
    i++;

    i += destringify_float((umm)&result.y, input.chop_left(i - 1));

    i = eat_whitespace(input, i);

    if (input[i] != ']')
        return 0;
    i++;

    *((Vec2*)ptr) = result;

    return i;
}

// Vec3
static PROC_STRINGIFY(stringify_vec3);
static constexpr _inline IType type_of(const Vec3 &v) {
    return IType {
        "Vec3",
        stringify_vec3,
        0
    };
}

static PROC_STRINGIFY(stringify_vec3) {
    Vec3 *v = (Vec3*)ptr;

    output.append(STATIC_STR("["));

    stringify_float((umm)&v->x, output, allocator);
    output.append(STATIC_STR(","));
    stringify_float((umm)&v->y, output, allocator);
    output.append(STATIC_STR(","));
    stringify_float((umm)&v->z, output, allocator);

    output.append(STATIC_STR("]"));
}