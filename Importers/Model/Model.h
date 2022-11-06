#pragma once
#include "../../Base.h"
#include "../../Math/Base.h"
#include "../../Debugging/Base.h"

namespace IndexFormat {
    enum Type {
        UInt16,
        UInt32,
        Count
    };
};

struct Model {
    umm vertices;
    umm indices;
    u32 num_vertices;
    u32 num_indices;
};