#include "ImageConverter.h"
#include "Bit.h"
#include "Importers/Import.h"
#include <string.h>

Raw ImageConverter::to_truecolor_rgba32(Desc *desc) {

    const int32 width = desc->image->width;
    const int32 height = desc->image->height;
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

            *(((uint32*)destination) + width * y + x) =
                (((buffer & red_mask)   >> red_shift)   << 24) |
                (((buffer & green_mask) >> green_shift) << 16) |
                (((buffer & blue_mask)  >> blue_shift)  <<  8) |
                (((buffer & alpha_mask) >> alpha_shift));

            column += column_size;
        }

    }

    return Raw {
        destination,
        final_image_size
    };
}
