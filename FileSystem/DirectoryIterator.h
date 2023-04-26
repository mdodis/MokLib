#pragma once
#include "Config.h"
#include "FileSystem.h"
#include "Host.h"
#include "Str.h"

struct FileData {
    Str             filename;
    EFileAttributes attributes;
};

struct MOKLIB_API DirectoryIterator {
    Str   directory;
    bool  next_file(FileData* result);
    void  close();
    void* handle;
#if OS_MSWINDOWS
    bool has_first_file = false;
#endif
};

MOKLIB_API DirectoryIterator open_dir(Str filename);
