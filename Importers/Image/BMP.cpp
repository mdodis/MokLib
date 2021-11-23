#include "BMP.h"
#include "Importers/Import.h"
#include "Debugging.h"
#include "Importers/Importer.h"
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

static PixelFormat::Type bmp_get_pixel_format(uint32 compression, uint16 bpp) {
    PixelFormat::Type format;

    switch (bpp) {
        case 32: {
            format = PixelFormat::BGRA8;
        } break;

        case 24: {
            format = PixelFormat::BGR8;
        } break;

        case 16: {
            format = PixelFormat::B5G6R5;
        } break;

        default: {
            ASSERT(false);
        } break;
    }

    return format;
}

#define PROC_IMPORTER_BMP_EXTRACT(name) bool name(FileHandle file_handle, IAllocator alloc, struct Import *result, const BMPFileHeader &file_header)

static PROC_IMPORTER_BMP_EXTRACT(extract_bmp_40);
static PROC_IMPORTER_BMP_EXTRACT(extract_bmp_124);

static void extract_bmp_set_default(int32 w, int32 h, uint16 bpp, uint32 compression, struct Import *result);

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

    extract_bmp_set_default(info.width, info.height, info.bpp, info.compression, result);
    result->data.buffer = alloc.reserve(alloc.context, result->data.size);
    int64 num_read = read_file(file_handle, result->data.buffer, result->data.size, file_header.offset);

    return (num_read == result->data.size);
}

static PROC_IMPORTER_BMP_EXTRACT(extract_bmp_124) {
    BMPInfoHeader124 info;
    if (read_file(file_handle, &info, sizeof(info), sizeof(file_header)) != sizeof(info)) {
        return false;
    }

    extract_bmp_set_default(info.width, info.height, info.bpp, info.compression, result);
    result->data.buffer = alloc.reserve(alloc.context, result->data.size);
    int64 num_read = read_file(file_handle, result->data.buffer, result->data.size, file_header.offset);

    return (num_read == result->data.size);
}

static void extract_bmp_set_default(int32 w, int32 h, uint16 bpp, uint32 compression, struct Import *result) {
    result->kind = ImportKind::Image;
    result->image.dimensions = Vec2i {w, abs(h)};
    result->image.bpp = uint8(bpp);
    result->image.is_flipped = h > 0;

    uint32 pitch = (uint32)ceilf((bpp * result->image.width) / 32.f) * 4;
    result->image.pitch = pitch;

    uint32 image_size = pitch * result->image.dimensions.y;
    result->data.size = image_size;

    result->image.format = bmp_get_pixel_format(compression, bpp);
}
