#pragma once
#include "FileSystem.h"
#include "Memory/Base.h"
#include "Containers/List.h"

static _inline bool write_file(FileHandle &handle, TList<Str> what) {
    for (const Str &str : what) {

        u64 bytes_written;
        if (!write_file(handle, str.data, str.len, &bytes_written)
            || (bytes_written != str.len)) {
            return false;
        }
    }
    return true;
}

static _inline bool write_file(FileHandle &handle, const Str &str) {
    u64 bytes_written;
    bool result = write_file(handle, str.data, str.len, &bytes_written);

    return (!result) || (bytes_written != str.len);
}

static _inline Raw dump_file(FileHandle &handle, IAllocator &alloc) {
    u64 file_size = get_file_size(handle);

    umm file_data = alloc.reserve(file_size);
    if (!file_data)
        return Raw{0};

    if (read_file(handle, file_data, file_size) != file_size) {
        alloc.release(file_data);
        return Raw{0};
    }

    return Raw { file_data, file_size };
}

static _inline Raw dump_file(
    Str filename,
    IAllocator &alloc = *get_system_allocator())
{
    FileHandle handle = open_file(filename, FileMode::Read);
    return dump_file(handle, alloc);
}