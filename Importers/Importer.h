#pragma once
#include "../Base.h"
#include "../Config.h"
#include "../FileSystem/FileSystem.h"
#include "../Memory/Base.h"

#define PROC_IMPORTER_LOAD(name) \
    bool name(FileHandle file_handle, Allocator* alloc, struct Import* result)
typedef PROC_IMPORTER_LOAD(ProcImporterLoad);

struct IImporter {
    Str               friendly_name;
    Str               extension;
    ProcImporterLoad* load;
};

struct IImporterRegistry {
    virtual bool load_file(
        Str filename, Allocator* alloc, struct Import* result) = 0;
};

struct MOKLIB_API ImporterRegistry : public IImporterRegistry {
    ImporterRegistry(IImporter* importers, u32 num_importers)
        : importers(importers), num_importers(num_importers)
    {}
    IImporter* importers;
    uint32     num_importers;

    virtual bool load_file(
        Str filename, Allocator* alloc, struct Import* result) override;
};
