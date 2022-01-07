#pragma once
#include "../Config.h"
#include "Memory/Base.h"
#include "Memory/RawBuffer.h"

struct MOKLIB_API ImageConverter {
    struct Desc {
        struct Image *image;
        void *data;
        IAllocator *alloc;
    };

    static Raw to_truecolor_rgba32(Desc *desc);
};