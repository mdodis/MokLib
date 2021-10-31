#pragma once

#include "Base.h"

#define RGBA(r, g, b, a) Vec4{(r), (g), (b), (a)}

namespace Color {
    static Vec4 Black = RGBA(0, 0, 0, 1);
}
