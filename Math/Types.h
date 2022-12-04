#pragma once
#include "Base.h"
#include "CG.h"
#include "../Types.h"
#include "Parsing.h"
#include "StringFormat.h"

PROC_FMT_INL(Vec2i) { format(tape, LIT("[{},{}]"),type.x, type.y); }
PROC_FMT_INL(Vec2)  { format(tape, LIT("[{},{}]"),type.x, type.y); }
PROC_FMT_INL(Vec3)  { format(tape, LIT("[{},{},{}]"),type.x, type.y, type.z); }
PROC_PARSE_INL(Vec2) {

    i64 total_move_back = 0;

    if (!tape->peek_char('['))
        goto PARSE_VEC2_ERROR;

    total_move_back++;

    if (!parse<f32>(tape, result.x))
        goto PARSE_VEC2_ERROR;

    total_move_back += eat_whitespace(tape);

    if (!tape->peek_char(','))
        goto PARSE_VEC2_ERROR;

    total_move_back += eat_whitespace(tape);

    if (!parse<f32>(tape, result.y))
        goto PARSE_VEC2_ERROR;

    if (!tape->peek_char(']'))
        goto PARSE_VEC2_ERROR;

    return true;

PARSE_VEC2_ERROR:
    tape->move(-total_move_back);
    return false;
}