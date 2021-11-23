#pragma once
#include "Host.h"
#include "../Str.h"
#include "Memory/RawBuffer.h"
#include <stdio.h>

enum class SymLinkKind {
    File,
    Folder
};

namespace FileMode {
    enum Type {
        Read        = 1 << 0,
        Write       = 1 << 1,
        ShareRead   = 1 << 2,
        ShareWrite  = 1 << 3,
    };
}

struct FileHandle {
#if OS_WINDOWS
    void *internal_handle;
#elif OS_LINUX
    int internal_handle;
#endif

};
#define IS_VALID_FILE(f) ((f).internal_handle != 0)

bool create_symlink(const Str &symlink_path, const Str &target_path, SymLinkKind kind);
FileHandle open_file(const Str &file_path, FileMode::Type mode);
uint32 get_file_size(const FileHandle &handle);
int64 read_file(FileHandle &handle, void *destination, uint32 bytes_to_read, uint64 offset = 0);
void close_file(const FileHandle &file);

template <typename T>
bool read_struct(FileHandle &handle, T *destination, uint64 offset = 0) {
    return read_file(handle, destination, sizeof(T)) == sizeof(T);
}

struct Tape {
    FileHandle file;
    uint64 current_offset;

    Tape(FileHandle file) :file(file) {
        current_offset = 0;
    }

    int64 read(void *destination, uint32 amount) {
        int64 num_read = read_file(file, destination, amount, current_offset);
        current_offset += num_read;
        return num_read;
    }

    char read_char() {
        char result = EOF;
        read(&result, sizeof(char));
        return result;
    }
};