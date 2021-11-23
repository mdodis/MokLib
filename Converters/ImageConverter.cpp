#include "ImageConverter.h"
#include "Bit.h"
#include "Importers/Import.h"
#include <string.h>

Raw ImageConverter::to_truecolor_rgba32(Desc *desc) {

    const uint32 width  = (uint32)desc->image->width;
    const uint32 height = (uint32)desc->image->height;
    uint32 final_image_size = width * height * sizeof(uint32);
    umm destination = desc->alloc->reserve(desc->alloc->context, final_image_size);

    umm row = (umm)desc->data;
    const uint32 column_size = desc->image->bpp / 8;

    for (uint32 y = 0; y < height; ++y) {

        const umm row = umm(desc->data) + desc->image->pitch * (desc->image->is_flipped ? height - 1 - y : y);
        umm column = row;
        for (uint32 x = 0; x < width; ++x) {

            uint32 buffer = 0; // assuming highest size for component would be a word
            memcpy(&buffer, column, column_size);

            // buffer = buffer >> (32 - desc->image->bpp);

            uint32 red_mask     = PixelFormat::mask_of(desc->image->format, 0);
            uint32 green_mask   = PixelFormat::mask_of(desc->image->format, 1);
            uint32 blue_mask    = PixelFormat::mask_of(desc->image->format, 2);
            uint32 alpha_mask   = PixelFormat::mask_of(desc->image->format, 3);

            int32 red_shift   = bit_scan(red_mask);
            int32 green_shift = bit_scan(green_mask);
            int32 blue_shift  = bit_scan(blue_mask);
            int32 alpha_shift = bit_scan(alpha_mask);

            uint32 red_count   = bit_count(red_mask);
            uint32 green_count = bit_count(green_mask);
            uint32 blue_count  = bit_count(blue_mask);
            uint32 alpha_count = bit_count(alpha_mask);

            uint32 red_value   = ((buffer & red_mask)   >> red_shift);
            uint32 green_value = ((buffer & green_mask) >> green_shift);
            uint32 blue_value  = ((buffer & blue_mask)  >> blue_shift);
            uint32 alpha_value = ((buffer & alpha_mask) >> alpha_shift);

            int t = ipow(2, blue_count);
            float nred   = (float(red_value)   / float(ipow(2, red_count)   - 1));
            float ngreen = (float(green_value) / float(ipow(2, green_count) - 1));
            float nblue  = (float(blue_value)  / float(ipow(2, blue_count)  - 1));
            float nalpha = (float(alpha_value) / float(ipow(2, alpha_count) - 1));

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
