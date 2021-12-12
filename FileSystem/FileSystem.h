#pragma once
#include "Host.h"
#include "../Str.h"
#include "Memory/RawBuffer.h"
#include "Time/Time.h"
#include <stdio.h>

enum class SymLinkKind {
    File,
    Folder
};

namespace FileMode {
    enum Type : u16 {
        Read            = 1 << 0,
        Write           = 1 << 1,
        ShareRead       = 1 << 2,
        ShareWrite      = 1 << 3,
        CreateAlways    = 1 << 4,
        NoBuffering     = 1 << 5,
        Append          = 1 << 6,
        OpenAlways      = 1 << 7,
    };
}
typedef u16 EFileMode;

struct FileHandle {
#if OS_WINDOWS
    void *internal_handle;
#elif OS_LINUX
    int internal_handle;
#endif

};
#define IS_VALID_FILE(f) ((f).internal_handle != 0)

bool create_symlink(const Str &symlink_path, const Str &target_path, SymLinkKind kind);
FileHandle open_file(const Str &file_path,EFileMode mode);
uint32 get_file_size(const FileHandle &handle);
MTime::TimeSpec get_file_time(const Str &file_path);
int64 read_file(FileHandle &handle, void *destination, uint32 bytes_to_read, uint64 offset = 0);
// @todo: make this use an offset instead of a tape like interface
bool write_file(FileHandle &handle, const void *src, uint32 bytes_to_write, uint32 *bytes_written);

void flush_file_buffers(FileHandle &handle);
void close_file(const FileHandle &file);

bool create_dir(const Str &pathname);

template <typename T>
bool read_struct(FileHandle &handle, T *destination, uint64 offset = 0) {
    return read_file(handle, destination, sizeof(T), offset) == sizeof(T);
}

struct Tape {
    FileHandle file;
    uint64 current_offset;

    int64 read(void *destination, uint32 amount) {
        int64 num_read = read_file(file, destination, amount, current_offset);
        current_offset += num_read;
        return num_read;
    }

    template <typename T>
    bool read_struct(T *destination) {
        return read(destination, sizeof(T)) == sizeof(T);
    }

    int64 read_raw(Raw &destination) {
        return read(destination.buffer, destination.size);
    }
};