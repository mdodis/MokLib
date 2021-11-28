#include "BMP.h"
#include "Importers/Import.h"
#include "Debugging.h"
#include "Importers/Importer.h"
#include "Memory/Base.h"
#include <math.h>
#include <windows.h>
#include <wingdi.h>

#pragma pack(push, 1)
struct BMPFileHeader {
    uint16 hdr;           // needs to have BM
    uint32 file_size;     // actual file size in bytes
    uint16 reserved[2];
    uint32 offset;
};

namespace BMPInfo {
    enum Type : uint16 {
        BM = 0x4d42,
    };
};

namespace BMPCompression {
    enum Type {
        None,
        RLE8,
        RLE4,
        Bitfields,
    };
}

/**
 * Core Header
 */
struct BMPCoreHeader {
    uint32 size;
    uint16 width;
    uint16 height;
    uint16 planes;
    uint16 bpp;
};

/**
 * BITMAPINFOHEADER (Windows NT, 3.1x or later)
 */
struct BMPInfoHeader40 {
    uint32 info_size;
    int32  width;
    int32  height;
    uint16 num_planes;
    uint16 bpp;
    uint32 compression;
    uint32 raw_size;
    int32  horizontal_resolution;
    int32  vertical_resolution;
    uint32 num_colors;
    uint32 num_important_colors;
};

/**
 * BITMAPV5HEADER (Windows NT 5.0, 98 or later)
 */
struct BMPInfoHeader124 {
    uint32 size;
    int32  width;
    int32  height;
    uint16 planes;
    uint16 bpp;
    uint32 compression;
    uint32 raw_size;
    int32  horizontal_resolution;
    int32  vertical_resolution;
    uint32 num_colors;
    uint32 num_important_colors;
    uint32 red_mask;
    uint32 green_mask;
    uint32 blue_mask;
    uint32 alpha_mask;
    uint32 color_space;
    int32  endpoint_red[3];
    int32  endpoint_green[3];
    int32  endpoint_blue[3];
    uint32 gamma_red;
    uint32 gamma_green;
    uint32 gamma_blue;
    uint32 intent;
    uint32 profile_data_offset;
    uint32 profile_data_size;
    uint32 reserved;
};

#pragma pack(pop)

#define PROC_IMPORTER_BMP_EXTRACT(name) bool name(FileHandle file_handle, IAllocator alloc, struct Import *result, const BMPFileHeader &file_header)

static PROC_IMPORTER_BMP_EXTRACT(extract_bmp_40);
static PROC_IMPORTER_BMP_EXTRACT(extract_bmp_124);

static bool extract_bmp(FileHandle fh, IAllocator &alloc, int32 w, int32 h, uint16 bpp, uint32 compression, uint32 offset, struct Import *result);
static bool read_bmp_compressed(FileHandle file_handle, IAllocator &alloc, struct Import *result, uint16 bpp, uint32 num_colors, uint32 offset, uint32 _compression);

static bool read_bmp_raw(FileHandle file_handle, IAllocator &alloc, struct Import *result, uint32 offset) {
    result->data.buffer = alloc.reserve(alloc.context, result->data.size);
    int64 num_read = read_file(file_handle, result->data.buffer, result->data.size, offset);
    return (num_read == result->data.size);
}

PROC_IMPORTER_LOAD(import_bmp_load) {

    BMPFileHeader file_header;
    if (read_file(file_handle, &file_header, sizeof(file_header)) != sizeof(file_header)) {
        return false;
    }

    if (file_header.hdr != BMPInfo::BM) {
        // unsupported info header variant
        return false;
    }

    uint32 header_size;
    if (read_file(file_handle, &header_size, sizeof(header_size), sizeof(file_header)) != sizeof(header_size)) {
        return false;
    }

    if (header_size == sizeof(BMPInfoHeader40)) {
        return extract_bmp_40(file_handle, alloc, result, file_header);
    } else if (header_size == sizeof(BMPInfoHeader124)) {
        return extract_bmp_124(file_handle, alloc, result, file_header);
    } else {
        return false;
    }
}

static PROC_IMPORTER_BMP_EXTRACT(extract_bmp_40) {

    BMPInfoHeader40 info;
    if (read_file(file_handle, &info, sizeof(info), sizeof(file_header)) != sizeof(info)) {
        return false;
    }

    return extract_bmp(
        file_handle,
        alloc,
        info.width,
        info.height,
        info.bpp,
        info.compression,
        file_header.offset,
        result);
}

static PROC_IMPORTER_BMP_EXTRACT(extract_bmp_124) {
    BMPInfoHeader124 info;
    if (read_file(file_handle, &info, sizeof(info), sizeof(file_header)) != sizeof(info)) {
        return false;
    }

    return extract_bmp(
        file_handle,
        alloc,
        info.width,
        info.height,
        info.bpp,
        info.compression,
        file_header.offset,
        result);
}

static bool extract_bmp(FileHandle fh, IAllocator &alloc, int32 w, int32 h, uint16 bpp, uint32 compression, uint32 offset, struct Import *result) {
    result->kind = ImportKind::Image;
    result->image.dimensions = Vec2i {w, abs(h)};
    result->image.is_flipped = h > 0;

    uint32 pitch = (uint32)ceilf((bpp * result->image.width) / 32.f) * 4;
    result->image.pitch = pitch;

    uint32 image_size = pitch * result->image.dimensions.y;
    result->data.size = image_size;

    switch (bpp) {
        case 32: {
            result->image.format = PixelFormat::BGRA8;
            result->image.bpp = uint8(bpp);
            return read_bmp_raw(fh, alloc, result, offset);
        } break;

        case 24: {
            result->image.format = PixelFormat::BGR8;
            result->image.bpp = uint8(bpp);
            return read_bmp_raw(fh, alloc, result, offset);
        } break;

        case 16: {
            result->image.format = PixelFormat::B5G6R5;
            result->image.bpp = uint8(bpp);
            return read_bmp_raw(fh, alloc, result, offset);
        } break;

        case 04: {

            if (compression == BMPCompression::RLE4) {
                return read_bmp_compressed(fh, alloc, result, offset, compression);
            } else {
                return false;
            }

        } break;

        default: {
            ASSERT(false);
        } break;
    }

}

static bool read_bmp_compressed(FileHandle file_handle, IAllocator &alloc, struct Import *result, uint16 bpp, uint32 num_colors, uint32 offset, uint32 _compression) {

    BMPCompression::Type compression = BMPCompression::Type(_compression);

    uint32 color_table_size = num_colors == 0
        ? ipow(2, bpp)
        : num_colors;

    switch (compression) {

        case BMPCompression::RLE4: {
            umm output = alloc.reserve(alloc.context, result->data.size);
            result->data.buffer = output;



            umm color_table = alloc.reserve(alloc.context, )
        } break;

        case BMPCompression::None:
        default: {
            ASSERT(false);
        } break;
    }

    return false;
}
