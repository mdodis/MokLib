#pragma once
#include "Base.h"
#include "FileSystem/FileSystem.h"
#include "Memory/Base.h"

#define PROC_IMPORTER_LOAD(name) bool name(FileHandle file_handle, IAllocator alloc, struct Import *result)
typedef PROC_IMPORTER_LOAD(ProcImporterLoad);

struct IImporter {
    Str friendly_name;
    Str extension;
    ProcImporterLoad *load;
};

struct ImporterRegistry {
    static constexpr uint32 Max_Importers = 16;

    IImporter importers[Max_Importers];
    uint32 num_importers;

    bool load_file(Str filename, IAllocator alloc, struct Import *result);
};
