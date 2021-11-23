#pragma once
#include "Base.h"
#include "Math/Base.h"
#include "Memory/RawBuffer.h"
#include "Debugging.h"

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

namespace PixelFormat {
    enum Type {
        RGB8 = 0,   // [R8 G8 B8]     Packed 32bit components
        RGBA8,      // [R8 G8 B8 A8]  Packed 32bit components
        BGRA8,      // [B8 G8 R8 A8]  Packed 32bit components
        BGR8,       // [B8 G8 R8]     Packed 32bit components
        B5G6R5,     // [B5 G6 R5]     Packed 16bit components
        Count,
    };

    static uint32 Mask_Table[PixelFormat::Count * 4] = {
        0, 0, 0, 0,     // RGB8
        0, 0, 0, 0,     // RGBA8
        0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000, // BGRA8
        0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000, // BGR8
        0x0000f800, 0X000007e0, 0x0000001f, 0x00000000, // B5G6R5
    };

    static _inline uint32 mask_of(PixelFormat::Type format, uint32 component) {
        ASSERT(component >= 0 && component <= 3);
        return Mask_Table[format * 4 + component];
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
};

struct Import {
    Raw data;

    ImportKind::Type kind;
    union {
        Image image;
    };
};
