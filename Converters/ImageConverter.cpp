#include "ImageConverter.h"
#include "Bit.h"
#include "Importers/Import.h"
#include "Numeric.h"
#include <string.h>

struct _ConversionConstants {
    u32 masks[4];
    i32 shifts[4];
    u32 counts[4];
};

static void get_conversion_constants(ImageConverter::Desc *desc, _ConversionConstants *result);

Raw ImageConverter::to_truecolor_rgba32(Desc *desc) {
    const uint32 width  = (uint32)desc->image->width;
    const uint32 height = (uint32)desc->image->height;
    uint32 final_image_size = width * height * sizeof(uint32);
    umm destination = desc->alloc->reserve(desc->alloc->context, final_image_size);

    umm row = (umm)desc->data;
    const uint32 column_size = desc->image->bpp / 8;

    _ConversionConstants constants;
    get_conversion_constants(desc, &constants);

    for (uint32 y = 0; y < height; ++y) {

        const umm row = umm(desc->data) + desc->image->pitch * (desc->image->is_flipped ? height - 1 - y : y);
        umm column = row;
        for (uint32 x = 0; x < width; ++x) {

            uint32 buffer = 0; // assuming highest size for component would be a word
            memcpy(&buffer, column, column_size);

            uint32 red_value   = ((buffer & constants.masks[0]) >> constants.shifts[0]);
            uint32 green_value = ((buffer & constants.masks[1]) >> constants.shifts[1]);
            uint32 blue_value  = ((buffer & constants.masks[2]) >> constants.shifts[2]);
            uint32 alpha_value = ((buffer & constants.masks[3]) >> constants.shifts[3]);

            float nred   = (float(red_value)   / float(ipow(2, constants.counts[0]) - 1));
            float ngreen = (float(green_value) / float(ipow(2, constants.counts[1]) - 1));
            float nblue  = (float(blue_value)  / float(ipow(2, constants.counts[2]) - 1));
            float nalpha = (float(alpha_value) / float(ipow(2, constants.counts[3]) - 1));

            red_value   = (uint32) (nred   * 255.f);
            green_value = (uint32) (ngreen * 255.f);
            blue_value  = (uint32) (nblue  * 255.f);
            alpha_value = (uint32) (nalpha * 255.f);

            *(((uint32*)destination) + width * y + x) =
                red_value   << 24 |
                green_value << 16 |
                blue_value  <<  8 |
                alpha_value;

            column += column_size;
        }

    }

    return Raw {
        destination,
        final_image_size
    };
}

Raw ImageConverter::to_truecolor_norm32(Desc *desc) {
    const uint32 width  = (uint32)desc->image->width;
    const uint32 height = (uint32)desc->image->height;
    uint32 final_image_size = width * height * sizeof(uint32);
    umm destination = desc->alloc->reserve(desc->alloc->context, final_image_size);

    umm row = (umm)desc->data;
    const uint32 column_size = desc->image->bpp / 8;

    _ConversionConstants constants;
    get_conversion_constants(desc, &constants);

    for (uint32 y = 0; y < height; ++y) {

        const umm row = umm(desc->data) + desc->image->pitch * (desc->image->is_flipped ? height - 1 - y : y);
        umm column = row;
        for (uint32 x = 0; x < width; ++x) {

            uint32 buffer = 0; // assuming highest size for component would be a word
            memcpy(&buffer, column, column_size);

            const u32 red_value   = ((buffer & constants.masks[0]) >> constants.shifts[0]);
            const u32 green_value = ((buffer & constants.masks[1]) >> constants.shifts[1]);
            const u32 blue_value  = ((buffer & constants.masks[2]) >> constants.shifts[2]);
            const u32 alpha_value = ((buffer & constants.masks[3]) >> constants.shifts[3]);

            const float nred   = (float(red_value)   / float(ipow(2, constants.counts[0]) - 1));
            const float ngreen = (float(green_value) / float(ipow(2, constants.counts[1]) - 1));
            const float nblue  = (float(blue_value)  / float(ipow(2, constants.counts[2]) - 1));
            const float nalpha = (float(alpha_value) / float(ipow(2, constants.counts[3]) - 1));

            const u8 r8 = re_f32_u8(nred);
            const u8 g8 = re_f32_u8(ngreen);
            const u8 b8 = re_f32_u8(nblue);
            const u8 a8 = re_f32_u8(nalpha);

            u32 pixel =
                (r8 << 24) |
                (g8 << 16) |
                (b8 <<  8) |
                (a8 <<  0);

            *(((u32*)destination) + width * y + x) = pixel;
        }
    }

    return Raw {
        destination,
        final_image_size
    };
}

static void get_conversion_constants(ImageConverter::Desc *desc, _ConversionConstants *result) {
    result->masks[0] = PixelFormat::mask_of(desc->image->format, 0);
    result->masks[1] = PixelFormat::mask_of(desc->image->format, 1);
    result->masks[2] = PixelFormat::mask_of(desc->image->format, 2);
    result->masks[3] = PixelFormat::mask_of(desc->image->format, 3);

    result->shifts[0] = bit_scan(result->masks[0]);
    result->shifts[1] = bit_scan(result->masks[1]);
    result->shifts[2] = bit_scan(result->masks[2]);
    result->shifts[3] = bit_scan(result->masks[3]);

    result->counts[0] = bit_count(result->masks[0]);
    result->counts[1] = bit_count(result->masks[1]);
    result->counts[2] = bit_count(result->masks[2]);
    result->counts[3] = bit_count(result->masks[3]);
}

