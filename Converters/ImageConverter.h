#pragma once
#include "Memory/Base.h"
#include "Memory/RawBuffer.h"

struct ImageConverter {
    struct Desc {
        struct Image *image;
        void *data;
        IAllocator *alloc;
    };

    static Raw to_truecolor_rgba32(Desc *desc);
    static Raw to_truecolor_norm32(Desc *desc);
};