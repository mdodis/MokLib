#include "BMP.h"
#include "Importers/Import.h"
#include "Debugging.h"
#include <math.h>

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
 * BITMAPINFOHEADER (Windows NT, 3.1x or later)
 */
struct BMPInfoHeader40 {
    uint32_t info_size;
    int32_t  width;
    int32_t  height;
    uint16_t num_planes;
    uint16_t bpp;
    uint32_t compression;
    uint32_t raw_size;
    int32_t  horizontal_resolution;
    int32_t  vertical_resolution;
    uint32_t num_colors;
    uint32_t num_important_colors;
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

        default: {
            ASSERT(false);
        } break;
    }

    return format;
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

    BMPInfoHeader40 info;
    if (read_file(file_handle, &info, sizeof(info), sizeof(file_header)) != sizeof(info)) {
        return false;
    }

    result->kind = ImportKind::Image;
    result->image.dimensions = Vec2i {info.width, abs(info.height)};
    result->image.format = bmp_get_pixel_format(info.compression, info.bpp);

    uint32 pitch = ceilf((info.bpp * result->image.dimensions.x) / 32.f) * 4;

    result->image.pitch = pitch;
    result->image.bpp = info.bpp;
    result->image.is_flipped = info.height > 0;
    uint32 image_size = pitch * result->image.dimensions.y;

    result->data.size = image_size;
    result->data.buffer = alloc.reserve(alloc.context, image_size);
    int64 num_read = read_file(file_handle, result->data.buffer, image_size, file_header.offset);

    return (num_read == image_size);
}
