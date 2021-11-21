#include "Log.h"
#include "Memory/Base.h"
#include "Str.h"
#include "Importers/Importer.h"
#include "Importers/Import.h"
#include "Converters/ImageConverter.h"
#include <stdio.h>

extern ImporterRegistry Importer_Registry;

int main(int argc, char *argv[]) {
    if (argc < 2) {
        PRINT("Usage: Importers [File to open]");
        PRINT("Not enough arguments.");
        return -1;
    }

    Str filepath = argv[1];

    Import import;
    bool result = Importer_Registry.load_file(filepath, get_system_allocator(), &import);
    if (!result) {
        PRINT("Failed to load file: " + filepath);
        return -1;
    }

    auto alloc = get_system_allocator();

    ImageConverter::Desc desc;
    desc.image = &import.image;
    desc.alloc = &alloc;
    desc.data = import.data.buffer;
    Raw image32 = ImageConverter::to_truecolor_rgba32(&desc);

    uint32 *pixels = (uint32*)image32.buffer;

    FILE *f = fopen("converted.ppm", "w");
    fprintf(f, "P3\n%d %d\n255\n", import.image.width, import.image.height);

    for (int y = 0; y < import.image.height; ++y) {
        for (int x = 0; x < import.image.width; ++x) {

            uint32 p = pixels[y * import.image.width + x];

            int r, g, b;

            r = (p & 0xff000000) >> 24;
            g = (p & 0x00ff0000) >> 16;
            b = (p & 0x0000ff00) >> 8;

            fprintf(f, "%d %d %d\n", r, g, b);
        }
    }

    fclose(f);


    return 0;
}

#include "Compile.inc"
#include "Importers/Importers.inc"
#include "Converters/Converters.inc"