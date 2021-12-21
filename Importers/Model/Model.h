#pragma once
#include "Base.h"
#include "Math/Base.h"
#include "Debugging/Base.h"

struct Model {
    umm ptr_vertices;
    umm ptr_indices;
    u32 num_vertices;
    u32 num_indices;
};