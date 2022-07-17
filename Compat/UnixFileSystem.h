#pragma once
#include "Base.h"
#include "FileSystem/FileSystem.h"
#include <unistd.h>
#include <sys/stat.h>

static _inline EFileAttributes unix_file_attribs_to_file_attribs(mode_t mode)
{
    EFileAttributes result = 0;

    if (mode & S_IFDIR) {
        return FileAttributes::Directory;
    } else {
        return FileAttributes::File;
    }

    return result;
}
