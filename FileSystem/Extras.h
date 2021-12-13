#pragma once
#include "FileSystem.h"
#include "StringBuilder.h"

static _inline bool write_file(FileHandle &handle, TList<Str> what) {
    for (const Str &str : what) {

        u32 bytes_written;
        if (!write_file(handle, str.data, str.len, &bytes_written) || (bytes_written != str.len)) {
            return false;
        }
    }
    return true;
}

static _inline bool write_file(FileHandle &handle, StringBuilder builder) {
    return write_file(handle, builder.to_list());
}

static _inline bool write_file(FileHandle &handle, const Str &str) {
    u32 bytes_written;
    bool result = write_file(handle, str.data, str.len, &bytes_written);

    return (!result) || (bytes_written != str.len);
}