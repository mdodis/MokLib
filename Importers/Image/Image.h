#pragma once
#include "Base.h"
#include "Math/Base.h"
#include "Debugging/Base.h"

namespace PixelFormat {
    enum Type {
        RGB8X = 0,  // [R8 G8 B8 X8]  Packed 32bit components
        RGBA8,      // [R8 G8 B8 A8]  Packed 32bit components
        BGRA8,      // [B8 G8 R8 A8]  Packed 32bit components
        BGR8,       // [B8 G8 R8]     Packed 24bit components
        B5G6R5,     // [B5 G6 R5]     Packed 16bit components
        Count,
    };

    static _inline uint32 mask_of(PixelFormat::Type format, uint32 component) {

        static uint32 table[PixelFormat::Count * 4] = {
            0, 0, 0, 0,     // RGB8X
            0, 0, 0, 0,     // RGBA8
            0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000, // BGRA8
            0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000, // BGR8
            0x0000f800, 0X000007e0, 0x0000001f, 0x00000000, // B5G6R5
        };

        ASSERT(component >= 0 && component <= 3);
        return table[format * 4 + component];
    }

    static _inline bool is_widely_supported(PixelFormat::Type format) {

        static bool table[PixelFormat::Count] = {
            true,   // RGB8X
            true,   // RGBA8
            true,   // BGRA8
            false,  // BGR8
            true,   // B5G6R5
        };

        return table[format];
    }

    static _inline uint32 buffer_size(PixelFormat::Type format, Vec2i dimensions) {
        static uint32 bytes_per_element[PixelFormat::Count] = {
            4,   // RGB8X
            4,   // RGBA8
            4,   // BGRA8
            3,   // BGR8
            2,   // B5G6R5
        };

        return dimensions.x * dimensions.y * bytes_per_element[format];
    }
};

struct Image {

    union {
        Vec2i dimensions;
        struct { int32 width, height; };
    };
    PixelFormat::Type format;   // The format of the image
    uint8 bpp;                  // Bits per pixel
    uint32 pitch;               // Pitch
    bool is_flipped;            // True if the image is flipped vertically

    _inline uint32 offset_at(Vec2i pos) {
        return
            pitch * (is_flipped ? height - 1 - pos.y : pos.y) +
            (bpp / 8) * pos.x;
    }
};
