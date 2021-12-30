#include "Log.h"
#include "Math/Base.h"
#include "Math/Types.h"
#include "Memory/Base.h"
#include "Str.h"
#include "Importers/Importer.h"
#include "Importers/Import.h"
#include "Converters/ImageConverter.h"
#include <stdio.h>

extern ImporterRegistry Importer_Registry;

void output_image_ppm(Import &import);
void print_vertices_model(Import &import);

int main(int argc, char *argv[]) {
    if (argc < 2) {
        PRINTLN("Usage: Importers [File to open]");
        PRINTLN("Not enough arguments.");
        return -1;
    }

    Str filepath = argv[1];

    Import import;
    bool result = Importer_Registry.load_file(filepath, get_system_allocator(), &import);
    if (!result) {
        PRINTLN("Failed to load file: " + filepath);
        return -1;
    }

    switch (import.kind) {

        case ImportKind::Image: {
            output_image_ppm(import);
        } break;

        case ImportKind::Model: {
            print_vertices_model(import);
        } break;

        default: {
            return 0;
        } break;
    }

    return 0;
}

void output_image_ppm(Import &import) {

    auto alloc = get_system_allocator();

    ImageConverter::Desc desc;
    desc.image = &import.image;
    desc.alloc = alloc;
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
}

#pragma pack(push, 1)
struct Vertex {
    Vec3 p;
    Vec3 n;
    Vec2 t;
};
#pragma pack(pop)

void print_vertices_model(Import &import) {

    PRINTLN("o MokModel.MokModel_001");
    for (u32 i = 0; i < import.model.num_vertices; ++i) {
        Vertex *v = &((Vertex*)import.model.vertices)[i];
        PRINTLN("v " + v->p.x + " " + v->p.y + " " + v->p.z);
    }

    for (u32 i = 0; i < import.model.num_vertices; ++i) {
        Vertex *v = &((Vertex*)import.model.vertices)[i];
        PRINTLN("vt " + v->t.x + " " + v->t.y);
    }

    for (u32 i = 0; i < import.model.num_vertices; ++i) {
        Vertex *v = &((Vertex*)import.model.vertices)[i];
        PRINTLN("vn " + v->n.x + " " + v->n.y + " " + v->n.z);
    }

    for (u32 i = 0; i < import.model.num_indices; i += 3) {
        const u32 index0 = 1 + ((u32*)import.model.indices)[i];
        const u32 index1 = 1 + ((u32*)import.model.indices)[i + 1];
        const u32 index2 = 1 + ((u32*)import.model.indices)[i + 2];

        PRINTLN("f "
            + index0 + "/" + index0 + "/" + index0 + " "
            + index1 + "/" + index1 + "/" + index1 + " "
            + index2 + "/" + index2 + "/" + index2);
    }
}


#include "Compile.inc"
#include "Importers/Importers.inc"
#include "Converters/Converters.inc"