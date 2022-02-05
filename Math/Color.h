#pragma once

#include "Base.h"

#define RGBA(r, g, b, a) Vec4{(r), (g), (b), (a)}
#define RGB(r, g, b) Vec3{(r), (g), (b)}

namespace Color {
    static Vec4 Black = RGBA(0, 0, 0, 1);
    static Vec4 Red   = RGBA(1, 0 ,0, 1);
    static Vec4 Green = RGBA(0, 1 ,0, 1);
    static Vec4 Blue  = RGBA(0, 0 ,1, 1);
}
