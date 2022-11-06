#pragma once
#include "../../Base.h"
#include "../../Math/CG.h"
#include "../../Debugging/Assertions.h"
#include "../../Bit.h"

#define MOK_PIXEL_FORMATS                                                       \
    X(R8G8B8G4u,          "4 byte aligned 8bit components (RGB)")               \
    X(R8G8B8A8u,          "Unaligned 8bit components (RGB)")                    \
    X(B8G8R8A8u,          "Unaligned 8bit components (RGB)")                    \
    X(B5G6R5u,            "Unaligned 5 bit components, except for green (RGB)") \
    X(R8u,                "Unaligned 8 bit components (R)")                     \
    X(A8u,                "Unaligned 8 bit components (A)")                     \
    X(R32G32B32f,         "Unaligned [0-1] f32 components (RGB)")               \

namespace PixelFormat {
    enum Type {
        RGB8X = 0,  // [R8 G8 B8 X8]  Padded 8bit components
        RGBA8,      // [R8 G8 B8 A8]  Packed 8bit components
        BGRA8,      // [B8 G8 R8 A8]  Packed 8bit components
        BGR8,       // [B8 G8 R8]     Packed 8bit components
        B5G6R5,     // [B5 G6 R5]     Packed 5bit components except for green (6 bits)
        Mono8,      // [L8]           One value representing all 3 channels (Opaque)
        Alpha8,     // [A8]           One alpha value, everything else is white
        R32G32B32F, // [R32 G32 B32] Packed 32bit normalized components
        Count,
    };

    static _inline uint32 mask_of(PixelFormat::Type format, uint32 component) {

        static uint32 table[PixelFormat::Count * 4] = {
            0xff000000, 0x00ff0000, 0x0000ff00, 0x00000000, // RGB8X
            0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff, // RGBA8
            0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000, // BGRA8
            0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000, // BGR8
            0x0000f800, 0x000007e0, 0x0000001f, 0x00000000, // B5G6R5
            0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff, // Mono8
            0x00000000, 0x00000000, 0x00000000, 0x000000ff
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
            true,   // Mono8
            true,   // Alpha8
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

    Vec2i dimensions;
    PixelFormat::Type format;   // The format of the image
    uint8 bpp;                  // Bits per pixel
    uint32 pitch;               // Pitch
    bool is_flipped;            // True if the image is flipped vertically

    constexpr i32 &width()  { return dimensions.x; }
    constexpr i32 &height() { return dimensions.y; }

    _inline uint32 offset_at(Vec2i pos) {
        return
            pitch * (is_flipped ? height() - 1 - pos.y : pos.y) +
            (bpp / 8) * pos.x;
    }


    struct Detail {
        Image *image;
        u32 mask[4];
        i32 shift_co[4];
        u32 count[4];

        _inline void normalized(Vec2i coords, void *dataptr, float *output) {
            const u32 offset = image->offset_at(coords);
            umm data = ((umm)dataptr) + offset;
            const u32 column_size = image->bpp / 8;

            u32 buffer = 0;
            memcpy(&buffer, data, column_size);

            u32 red_value   = ((buffer & mask[0]) >> shift_co[0]);
            u32 green_value = ((buffer & mask[1]) >> shift_co[1]);
            u32 blue_value  = ((buffer & mask[2]) >> shift_co[2]);
            u32 alpha_value = ((buffer & mask[3]) >> shift_co[3]);

            f32 nred   = (f32(red_value)   / f32(ipow(2, count[0]) - 1));
            f32 ngreen = (f32(green_value) / f32(ipow(2, count[1]) - 1));
            f32 nblue  = (f32(blue_value)  / f32(ipow(2, count[2]) - 1));
            f32 nalpha = (f32(alpha_value) / f32(ipow(2, count[3]) - 1));

            output[0] = nred;
            output[1] = ngreen;
            output[2] = nblue;
            output[3] = nalpha;
        }
    };

    _inline void fill_detail(Detail &detail) {
        detail.image = this;
        detail.mask[0] = PixelFormat::mask_of(format, 0);
        detail.mask[1] = PixelFormat::mask_of(format, 1);
        detail.mask[2] = PixelFormat::mask_of(format, 2);
        detail.mask[3] = PixelFormat::mask_of(format, 3);

        detail.shift_co[0] = bit_scan(detail.mask[0]);
        detail.shift_co[1] = bit_scan(detail.mask[1]);
        detail.shift_co[2] = bit_scan(detail.mask[2]);
        detail.shift_co[3] = bit_scan(detail.mask[3]);

        detail.count[0] = bit_count(detail.mask[0]);
        detail.count[1] = bit_count(detail.mask[1]);
        detail.count[2] = bit_count(detail.mask[2]);
        detail.count[3] = bit_count(detail.mask[3]);
    }

};
