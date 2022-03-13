#include "ImageConverter.h"
#include "Bit.h"
#include "Importers/Import.h"
#include "Numeric.h"
#include <string.h>

struct _ConversionConstants {
    u32 m[4];   // Masks
    i32 s[4];   // Shifts
    u32 c[4];   // Counts
};

static void get_conversion_constants(
    ImageConverter::Desc *desc,
    _ConversionConstants *result);

bool ImageConverter::to_truecolor_rgba32(Desc *desc, Tape *output) {
    const uint32 width  = (uint32)desc->image->width;
    const uint32 height = (uint32)desc->image->height;
    uint32 final_image_size = width * height * sizeof(uint32);


    umm row = (umm)desc->data;
    const uint32 column_size = desc->image->bpp / 8;

    _ConversionConstants constants;
    get_conversion_constants(desc, &constants);

    for (uint32 y = 0; y < height; ++y) {

        const umm row = umm(desc->data) +
            desc->image->pitch *
            (desc->image->is_flipped ? height - 1 - y : y);
        umm column = row;
        for (uint32 x = 0; x < width; ++x) {

            // assuming highest size for component would be a word
            uint32 buffer = 0;
            memcpy(&buffer, column, column_size);

            uint32 red_value   = ((buffer & constants.m[0]) >> constants.s[0]);
            uint32 green_value = ((buffer & constants.m[1]) >> constants.s[1]);
            uint32 blue_value  = ((buffer & constants.m[2]) >> constants.s[2]);
            uint32 alpha_value = ((buffer & constants.m[3]) >> constants.s[3]);

            f32 nred   = (f32(red_value)   / f32(ipow(2, constants.c[0]) - 1));
            f32 ngreen = (f32(green_value) / f32(ipow(2, constants.c[1]) - 1));
            f32 nblue  = (f32(blue_value)  / f32(ipow(2, constants.c[2]) - 1));
            f32 nalpha = (f32(alpha_value) / f32(ipow(2, constants.c[3]) - 1));

            red_value   = (uint32) (nred   * 255.f);
            green_value = (uint32) (ngreen * 255.f);
            blue_value  = (uint32) (nblue  * 255.f);
            alpha_value = (uint32) (nalpha * 255.f);

            u32 offset = (width * y + x) * sizeof(u32);

            u32 value =
                red_value   <<  0 |
                green_value <<  8 |
                blue_value  << 16 |
                alpha_value << 24;

            // *(((uint32*)destination) + width * y + x) =
            //     value;

            output->write(&value, sizeof(value));

            column += column_size;
        }

    }

    return true;
}

static void get_conversion_constants(
    ImageConverter::Desc *desc,
    _ConversionConstants *result)
{
    result->m[0] = PixelFormat::mask_of(desc->image->format, 0);
    result->m[1] = PixelFormat::mask_of(desc->image->format, 1);
    result->m[2] = PixelFormat::mask_of(desc->image->format, 2);
    result->m[3] = PixelFormat::mask_of(desc->image->format, 3);

    result->s[0] = bit_scan(result->m[0]);
    result->s[1] = bit_scan(result->m[1]);
    result->s[2] = bit_scan(result->m[2]);
    result->s[3] = bit_scan(result->m[3]);

    result->c[0] = bit_count(result->m[0]);
    result->c[1] = bit_count(result->m[1]);
    result->c[2] = bit_count(result->m[2]);
    result->c[3] = bit_count(result->m[3]);
}

Raw ImageConverter::to_truecolor_rgba32(Desc *desc) {
    const uint32 width  = (uint32)desc->image->width;
    const uint32 height = (uint32)desc->image->height;
    uint32 final_image_size = width * height * sizeof(uint32);
    umm destination = desc->alloc->reserve(final_image_size);

    RawTape tape(Raw {
        destination,
        final_image_size
    });

    ImageConverter::to_truecolor_rgba32(desc, &tape);

    return tape.raw;
}
