#include "Importer.h"
#include "Base.h"
#include "Defer.h"
#include "FileSystem/FileSystem.h"
#include "Importers/Import.h"
#include "Memory/Arena.h"

bool match_against_extensions(const Str &match, const Str &extensions) {

    u64 last_pos = 0;
    u64 pos = extensions.first_of('|');

    // if we don't have a string with multiple encoded extensions => strcmp
    if (pos == extensions.len) {
        return match == extensions;
    }

    while (last_pos != extensions.len) {

        Str extension = extensions.chop_middle(last_pos, pos);

        if (extension == match) {
            return true;
        }

        last_pos = pos + 1;
        pos = extensions.first_of('|', last_pos);
    }

    return false;
}

bool ImporterRegistry::load_file(Str filename, IAllocator *alloc, struct Import *result) {

    FileHandle fh = open_file(filename, FileMode::Read);
    DEFER({
        close_file(fh);
    });

    Str extension = filename.chop_left(filename.last_of('.') - 1);

    IImporter *importer = 0;

    for (uint32 i = 0; i < num_importers; ++i) {

        if (match_against_extensions(extension, importers[i].extension)) {
            importer = &importers[i];
            break;
        }
    }

    if (importer) {

        Arena arena = Arena(alloc, MEGABYTES(1));
        bool successful = importer->load(fh, &arena, result);
        result->data.buffer = arena.get_block_data();
        result->data.size   = arena.used;

        return successful;

    } else {
        return false;
    }
}
