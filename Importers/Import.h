#pragma once
#include "Base.h"
#include "Math/Base.h"
#include "Memory/RawBuffer.h"
#include "Debugging/Base.h"
#include "Image/Image.h"
#include "Model/Model.h"

namespace ImportKind {
    enum Type {
        Image,      // Any truecolor image
        Model,      // Any Model kind
    };
};

/**
 * Image
 */

struct Import {
    Raw data;

    ImportKind::Type kind;
    union {
        Image image;
        Model model;
    };
};
