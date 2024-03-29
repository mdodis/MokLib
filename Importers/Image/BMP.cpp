#include "BMP.h"

#include "BMPInternal.h"
#include "Debugging/Base.h"
#include "FileSystem/FileSystem.h"
#include "Importers/Import.h"
#include "Importers/Importer.h"
#include "Math/Base.h"

#define PROC_IMPORTER_BMP_EXTRACT(name)   \
    bool name(                            \
        FileHandle           file_handle, \
        Allocator&           alloc,       \
        struct Import*       result,      \
        const BMPFileHeader& file_header)

static PROC_IMPORTER_BMP_EXTRACT(extract_bmp_40);
static PROC_IMPORTER_BMP_EXTRACT(extract_bmp_124);

static bool extract_bmp(
    FileHandle     fh,
    Allocator&     alloc,
    BMPAttribs*    attribs,
    struct Import* result);
static bool read_bmp_indexed(
    FileHandle     file_handle,
    Allocator&     alloc,
    BMPAttribs*    attribs,
    struct Import* result);

static bool read_bmp_raw(
    FileHandle     file_handle,
    Allocator&     alloc,
    struct Import* result,
    uint32         offset)
{
    result->data.buffer = alloc.reserve(result->data.size);
    int64 num_read =
        read_file(file_handle, result->data.buffer, result->data.size, offset);
    return (num_read == result->data.size);
}

PROC_IMPORTER_LOAD(import_bmp_load)
{
    BMPFileHeader file_header;
    if (read_file(file_handle, &file_header, sizeof(file_header)) !=
        sizeof(file_header))
    {
        return false;
    }

    if (file_header.hdr != BMPInfo::BM) {
        // unsupported info header variant
        return false;
    }

    uint32 header_size;
    if (read_file(
            file_handle,
            &header_size,
            sizeof(header_size),
            sizeof(file_header)) != sizeof(header_size))
    {
        return false;
    }

    if (header_size == sizeof(BMPInfoHeader40)) {
        return extract_bmp_40(file_handle, *alloc, result, file_header);
    } else if (header_size == sizeof(BMPInfoHeader124)) {
        return extract_bmp_124(file_handle, *alloc, result, file_header);
    } else {
        return false;
    }
}

static PROC_IMPORTER_BMP_EXTRACT(extract_bmp_40)
{
    BMPInfoHeader40 info;
    if (read_file(file_handle, &info, sizeof(info), sizeof(file_header)) !=
        sizeof(info))
    {
        return false;
    }

    BMPAttribs attribs;
    attribs.dimensions.x = info.width;
    attribs.dimensions.y = info.height;
    attribs.bpp          = info.bpp;
    attribs.compression  = info.compression;
    attribs.offset       = file_header.offset;
    attribs.num_colors   = info.num_colors;
    attribs.table_offset = sizeof(BMPFileHeader) + sizeof(info);

    return extract_bmp(file_handle, alloc, &attribs, result);
}

static PROC_IMPORTER_BMP_EXTRACT(extract_bmp_124)
{
    BMPInfoHeader124 info;
    if (read_file(file_handle, &info, sizeof(info), sizeof(file_header)) !=
        sizeof(info))
    {
        return false;
    }

    BMPAttribs attribs;
    attribs.dimensions.x = info.width;
    attribs.dimensions.y = info.height;
    attribs.bpp          = info.bpp;
    attribs.compression  = info.compression;
    attribs.offset       = file_header.offset;
    attribs.num_colors   = info.num_colors;
    attribs.table_offset = sizeof(BMPFileHeader) + sizeof(info);

    return extract_bmp(file_handle, alloc, &attribs, result);
}

static bool extract_bmp(
    FileHandle fh, Allocator& alloc, BMPAttribs* attribs, struct Import* result)
{
    result->kind = ImportKind::Image;
    result->image.dimensions =
        Vec2i{attribs->dimensions.x, absolute(attribs->dimensions.y)};
    result->image.is_flipped = attribs->dimensions.y > 0;

    // Taken from https://en.wikipedia.org/wiki/BMP_file_format#Color_table
    uint32 pitch =
        (uint32)ceilf((attribs->bpp * result->image.width()) / 32.f) * 4;
    result->image.pitch = pitch;

    uint32 image_size = pitch * result->image.dimensions.y;
    result->data.size = image_size;

    switch (attribs->bpp) {
        case 32: {
            result->image.format = PixelFormat::BGRA8;
            result->image.bpp    = uint8(attribs->bpp);
            return read_bmp_raw(fh, alloc, result, attribs->offset);
        } break;

        case 24: {
            result->image.format = PixelFormat::BGR8;
            result->image.bpp    = uint8(attribs->bpp);
            return read_bmp_raw(fh, alloc, result, attribs->offset);
        } break;

        case 16: {
            result->image.format = PixelFormat::B5G6R5;
            result->image.bpp    = uint8(attribs->bpp);
            return read_bmp_raw(fh, alloc, result, attribs->offset);
        } break;

        case 04: {
            return read_bmp_indexed(fh, alloc, attribs, result);
        } break;

        default: {
            ASSERT(false);
            return false;
        } break;
    }
}

static bool read_bmp_indexed(
    FileHandle     file_handle,
    Allocator&     alloc,
    BMPAttribs*    attribs,
    struct Import* result)
{
    BMPCompression::Type compression =
        BMPCompression::Type(attribs->compression);

    uint32 color_table_count =
        attribs->num_colors == 0 ? ipow(2, attribs->bpp) : attribs->num_colors;

    const uint32 converted_size =
        attribs->dimensions.y * attribs->dimensions.x * sizeof(uint32);
    const uint32 pitch       = result->image.pitch;
    const bool   is_flipped  = result->image.is_flipped;
    result->image.format     = PixelFormat::BGRA8;
    result->image.bpp        = 32;
    result->image.pitch      = result->image.width() * sizeof(uint32);
    result->image.is_flipped = false;

    // Output Data
    uint32* output      = (uint32*)alloc.reserve(converted_size);
    result->data.buffer = output;
    result->data.size   = converted_size;

    // Color Table
    const uint32 color_table_size = color_table_count * sizeof(uint32);
    uint32*      color_table      = (uint32*)alloc.reserve(color_table_size);
    ASSERT(color_table);

    read_file(
        file_handle,
        color_table,
        color_table_size,
        attribs->table_offset);

    uint32 curr_offset = attribs->offset;
    for (int32 c = 0; c < attribs->dimensions.y; ++c) {
        int32 y = is_flipped ? attribs->dimensions.y - 1 - c : c;

        u32 curr_row = curr_offset;
        u32 row_end  = curr_offset + pitch;
        i32 x        = 0;

        // Parse each row depending on compression field
        while (curr_row < row_end) {
            switch (compression) {
                case BMPCompression::None: {
                    if (x == result->image.width()) {
                        curr_row = row_end;
                        break;
                    }

                    uint8 code;
                    read_struct(file_handle, &code, curr_row);

                    const u8 left_pixel  = code >> 4;
                    const u8 right_pixel = code & 0x0f;

                    // Assign color data for left pixel & right pixel (if
                    // available)
                    const uint32 offset_at =
                        result->image.offset_at(Vec2i{x++, y});
                    uint32* out_pixels = (uint32*)(umm(output) + offset_at);
                    out_pixels[0]      = color_table[left_pixel];

                    if (x < result->image.width()) {
                        out_pixels[1] = color_table[right_pixel];
                        x++;
                    }

                    curr_row += 1;

                } break;

                case BMPCompression::RLE4: {
                    uint8 rle;
                    read_struct<uint8>(file_handle, &rle, curr_row);

                    const uint8 repeat = rle >> 4;
                    const uint8 rcount = rle & 0x0f;

                    curr_row += 1;
                    if (repeat == 0) {
                        for (uint8 i = 0; i < rcount; ++i) {
                            if (x == result->image.width()) {
                                curr_row = row_end;
                                break;
                            }

                            uint8 code;
                            read_struct<uint8>(file_handle, &code, curr_row);

                            const u8 left_pixel  = code >> 4;
                            const u8 right_pixel = code & 0x0f;

                            // Assign color data for left pixel & right pixel
                            // (if available)
                            const uint32 offset_at =
                                result->image.offset_at(Vec2i{x++, y});
                            uint32* out_pixels =
                                (uint32*)(umm(output) + offset_at);
                            out_pixels[0] = color_table[left_pixel];

                            if (x < result->image.width()) {
                                out_pixels[1] = color_table[right_pixel];
                                x++;
                            }
                        }

                    } else {
                        // @todo: Need to find case were repeat != 0
                        ASSERT(false);
                    }

                } break;

                default: {
                } break;
            }
        }

        curr_offset += pitch;
    }

    return true;
}
