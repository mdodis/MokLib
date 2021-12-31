#pragma once
#include "Str.h"
#include "Host.h"

struct FileData {
    Str filename;
};

struct DirectoryIterator {
    Str directory;
    bool next_file(FileData *result);
    void *handle;
#if OS_MSWINDOWS
    bool has_first_file = false;
#endif
};

DirectoryIterator open_dir(Str filename);