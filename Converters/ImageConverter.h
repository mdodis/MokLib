#pragma once
#include "../Config.h"
#include "Memory/Base.h"
#include "Memory/RawBuffer.h"
#include "Tape.h"

struct MOKLIB_API ImageConverter {
    struct Desc {
        struct Image* image;
        void*         data;
        Allocator*    alloc;
    };

    static Raw  to_truecolor_rgba32(Desc* desc);
    static bool to_truecolor_rgba32(Desc* desc, Tape* output);
};