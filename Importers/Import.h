#pragma once
#include "Base.h"
#include "Math/Base.h"
#include "Memory/RawBuffer.h"
#include "Debugging/Base.h"
#include "Image/Image.h"

namespace ImportKind {
    enum Type {
        Image,      // Any truecolor image
        DataObject, // JSon, YAML
        TreeObject  // XML
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
    };
};
