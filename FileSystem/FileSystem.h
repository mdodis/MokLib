#pragma once
#include "Host.h"
#include "../Str.h"
#include "Memory/RawBuffer.h"

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
int64 read_file(FileHandle &handle, void *destination, uint32 bytes_to_read);
void close_file(const FileHandle &file);