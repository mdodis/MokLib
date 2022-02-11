#pragma once
#include "Math/Base.h"

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
        None = 0,
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

    uint32 size;    // Number of bytes for this structure
    int32  width;   // Image width in pixels
    int32  height;  // Image height in pixels
    uint16 planes;  // Must be 1
    uint16 bpp;     // What else could this mean?
    uint32 compression;
    uint32 raw_size;
    int32  hres;
    int32  vres;
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

struct BMPAttribs {
    uint32 num_colors;      // 0 or the actual count of the different colors in the palette
    uint32 compression;
    uint32 bpp;
    Vec2i dimensions;
    uint32 offset;          // Offset into the image data
    uint32 table_offset;    // Offset into the palette data
};

#pragma pack(pop)