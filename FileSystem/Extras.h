#pragma once
#include "Containers/List.h"
#include "FileSystem.h"
#include "Memory/Base.h"
#include "StringFormat.h"

static _inline bool write_file(FileHandle& handle, TList<Str> what)
{
    for (const Str& str : what) {
        u64 bytes_written;
        if (!write_file(handle, str.data, str.len, &bytes_written) ||
            (bytes_written != str.len))
        {
            return false;
        }
    }
    return true;
}

static _inline bool write_file(FileHandle& handle, const Str& str)
{
    u64  bytes_written;
    bool result = write_file(handle, str.data, str.len, &bytes_written);

    return (!result) || (bytes_written != str.len);
}

static _inline Raw dump_file(FileHandle& handle, Allocator& alloc)
{
    u64 file_size = get_file_size(handle);

    void* file_data = alloc.reserve(file_size);
    if (!file_data) return Raw{0};

    if (read_file(handle, file_data, file_size) != file_size) {
        alloc.release(file_data);
        return Raw{0};
    }

    return Raw{file_data, file_size};
}

static _inline Raw dump_file(
    Str filename, Allocator& allocator = System_Allocator)
{
    FileHandle handle = open_file(filename, FileMode::Read);
    Raw        result = dump_file(handle, allocator);
    close_file(handle);
    return result;
}

static _inline void print(Str fmt_str)
{
    auto tape = get_write_stream(Console::Output);
    tape.write_str(fmt_str);
}

template <typename First, typename... Rest>
static _inline void print(Str fmt_str, const First& first, const Rest&... rest)
{
    auto tape = get_write_stream(Console::Output);
    format(&tape, fmt_str, first, rest...);
}
