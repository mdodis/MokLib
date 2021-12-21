#include "Importer.h"
#include "Base.h"
#include "Defer.h"
#include "FileSystem/FileSystem.h"
#include "Importers/Import.h"
#include "Memory/Arena.h"

bool ImporterRegistry::load_file(Str filename, IAllocator alloc, struct Import *result) {

    FileHandle fh = open_file(filename, FileMode::Read);
    DEFER({
        close_file(fh);
    });

    Str extension = filename.chop_left(filename.last_of('.') - 1);

    IImporter *importer = 0;

    for (uint32 i = 0; i < num_importers; ++i) {
        if (importers[i].extension == extension) {
            importer = &importers[i];
            break;
        }
    }

    if (importer) {

        Arena arena = Arena::create(alloc, MEGABYTES(1));
        bool successful = importer->load(fh, arena.to_alloc(), result);
        // result->data.buffer = arena.memory;
        // result->data.size   = arena.used;

        return successful;

    } else {
        return false;
    }
}
