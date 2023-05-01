#include "Exporters.h"

#include "../Base.h"
#include "../Converters/ImageConverter.h"
#include "../FileSystem/FileSystem.h"
#include "../Importers/Image/BMPInternal.h"
#include "../Memory/Arena.h"
#include "../Memory/Base.h"

void write_image_to_file(const Str& filepath, Image& image, umm data)
{
    FileHandle handle =
        open_file(filepath, FileMode::Write | FileMode::Truncate);
    FileTape tape(handle);

    BMPFileHeader   file_header = {};
    BMPInfoHeader40 info_header = {};

    const u32 output_size = image.width() * image.height() * 4;

    // File Header
    file_header.file_size =
        sizeof(file_header) + sizeof(info_header) + output_size;

    file_header.hdr         = BMPInfo::BM;
    file_header.reserved[0] = 0;
    file_header.reserved[1] = 0;
    file_header.offset      = sizeof(file_header) + sizeof(info_header);

    // Info Header
    info_header.info_size             = sizeof(info_header);
    info_header.width                 = image.width();
    info_header.height                = image.height();
    info_header.bpp                   = 32;
    info_header.compression           = BMPCompression::None;
    info_header.raw_size              = output_size;
    info_header.horizontal_resolution = 1;
    info_header.vertical_resolution   = 1;
    info_header.num_planes            = 1;
    info_header.num_colors            = 0;
    info_header.num_important_colors  = 0;

    tape.write(&file_header, sizeof(file_header));
    tape.write(&info_header, sizeof(info_header));

    CREATE_SCOPED_ARENA(System_Allocator, arena, MEGABYTES(1));
    ImageConverter::Desc desc = {};
    desc.image                = &image;
    desc.alloc                = &arena;
    desc.data                 = data;
    ImageConverter::to_truecolor_rgba32(&desc, &tape);
}