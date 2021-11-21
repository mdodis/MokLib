#include "Importer.h"
#include "FileSystem/FileSystem.h"

bool ImporterRegistry::load_file(Str filename, IAllocator alloc, struct Import *result) {

    FileHandle fh = open_file(filename, FileMode::Read);

    bool success = importers[0].load(fh, alloc, result);
    close_file(fh);
    return success;
}
