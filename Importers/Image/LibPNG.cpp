#include "LibPNG.h"
#include "Base.h"
#include "Debugging/Base.h"
#include "Importers/Image/Image.h"
#include "Importers/Import.h"
#include "FileSystem/FileSystem.h"
#include "Memory/Base.h"
#include "Memory/Arena.h"
#include "Tape.h"
#include "WinInc.h"

#define PROC_IMPORT_PNG_ERROR_HANDLER(name) void name(png_structp pngptr, png_const_charp error_msg)
typedef PROC_IMPORT_PNG_ERROR_HANDLER(ProcImportPNGErrorHandler);

PROC_IMPORT_PNG_ERROR_HANDLER(import_png_user_error) {
    OutputDebugStringA("PNG error: ");
    OutputDebugStringA(error_msg);
}

png_voidp import_png_malloc(png_structp ptr, png_alloc_size_t size) {
    IAllocator *alloc = (IAllocator*)png_get_mem_ptr(ptr);
    return alloc->reserve(alloc->context, size);
}

void import_png_free(png_structp png_ptr, png_voidp ptr) {
    IAllocator *alloc = (IAllocator*)png_get_mem_ptr(png_ptr);
    alloc->release(alloc->context, (umm)ptr);
}

void import_png_io_read(png_structp png_ptr, png_bytep data, png_size_t length) {
    Tape *tape = (Tape*)png_get_io_ptr(png_ptr);
    tape->read(data, length);
}

void import_png_io_write(png_structp png_ptr, png_bytep data, png_size_t length) {

}

void import_png_io_flush(png_structp png_ptr) {

}

enum {
    PngSignatureSize = 8,
};

PROC_IMPORTER_LOAD(import_png_load) {

    CREATE_SCOPED_ARENA(get_system_allocator(), temp, KILOBYTES(512));
    auto temp_alloc = temp.to_alloc();

    u8 signature[PngSignatureSize];
    if (read_file(file_handle, signature, PngSignatureSize, 0) != PngSignatureSize) {
        return false;
    }

    if (png_sig_cmp(signature, 0, PngSignatureSize) != 0) {
        return false;
    }

    // @bug: putting &alloc results in a crash inside libpng
    png_structp png_ptr = png_create_read_struct_2(
        PNG_LIBPNG_VER_STRING,
        import_png_user_error, import_png_user_error, import_png_user_error,
        (png_voidp)&temp_alloc, import_png_malloc, import_png_free);

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        return false;
    }

    FileTape read_tape(file_handle);

    png_set_read_fn(png_ptr, &read_tape, import_png_io_read);
    png_set_sig_bytes(png_ptr, 0);
    png_read_info(png_ptr, info_ptr);

    result->image.width  = png_get_image_width(png_ptr, info_ptr);
    result->image.height = png_get_image_height(png_ptr, info_ptr);

    u32 bits_per_channel = png_get_bit_depth(png_ptr, info_ptr);
    u32 num_channels     = png_get_channels(png_ptr, info_ptr);
    u32 color_type       = png_get_color_type(png_ptr, info_ptr);

    switch (color_type) {
        case PNG_COLOR_TYPE_PALETTE: {
            png_set_palette_to_rgb(png_ptr);
        } break;

        case PNG_COLOR_TYPE_GRAY: {
            png_set_expand_gray_1_2_4_to_8(png_ptr);
            bits_per_channel = 8;
        } break;
    }

    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
        png_set_tRNS_to_alpha(png_ptr);
        num_channels++;
    }

    png_read_update_info(png_ptr, info_ptr);

    color_type = png_get_color_type(png_ptr, info_ptr);

    if ((bits_per_channel == 8)
        && (num_channels == 3)
        && (color_type == PNG_COLOR_TYPE_RGB))
    {
        result->image.format = PixelFormat::RGB8X;
    } else if (
        (bits_per_channel == 8)
        && (num_channels == 4)
        && (color_type == PNG_COLOR_TYPE_RGB_ALPHA))
    {
        result->image.format = PixelFormat::RGBA8;
    }


    umm *row_ptrs = (umm*)temp.push(sizeof(umm) * result->image.height);
    ASSERT(row_ptrs);

    result->data.size = result->image.width
        * result->image.height
        * ((bits_per_channel * num_channels) / 8);

    result->data.buffer = alloc.reserve(alloc.context, result->data.size);
    result->image.pitch = result->image.width * ((bits_per_channel * num_channels) / 8);
    result->image.bpp = num_channels * bits_per_channel;

    for (i32 i = 0; i < result->image.height; ++i) {
        row_ptrs[i] = umm(result->data.buffer) + (result->image.height - i - 1 ) * result->image.pitch;
    }

    png_read_image(png_ptr, row_ptrs);

    png_read_end(png_ptr, info_ptr);

    result->kind = ImportKind::Image;

    png_destroy_read_struct(&png_ptr, &info_ptr, 0);

    return true;
}
