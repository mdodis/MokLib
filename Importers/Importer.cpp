#include "Importer.h"
#include "Defer.h"
#include "FileSystem/FileSystem.h"

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
        return importer->load(fh, alloc, result);
    } else {
        return false;
    }
}
