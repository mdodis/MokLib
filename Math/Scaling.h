#pragma once

_inline Vec2 scale_to_retain_aspect(Vec2 basis, Vec2 size) {

    Vec2 result;
    float aspect = basis.x / basis.y;

    if (size.x > size.y) {
        result.y = size.y;
        result.x = (aspect) * size.y;
    } else {
        result.x = size.x;
        result.y = (1.f / aspect) * size.x;
    }

    return result;
}