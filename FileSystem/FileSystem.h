#pragma once
#include "../Config.h"
#include "Host.h"
#include "Str.h"
#include "Memory/RawBuffer.h"
#include "Memory/Arena.h"
#include "Time/Time.h"
#include "Tape.h"
#include "Console.h"
#include "Error.h"

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
        /** Opens the file if it exists, otherwise creates it */
        OpenAlways      = 1 << 7,
    };
}
typedef u16 EFileMode;

namespace FileAttributes {
    enum Type : u32 {
        Directory,
        File
    };
};
typedef u32 EFileAttributes;

struct MOKLIB_API FileHandle {
#if OS_MSWINDOWS
    void *internal_handle;
#elif OS_LINUX
    int internal_handle;
#endif

};
#define IS_VALID_FILE(f) ((f).internal_handle != 0)

MOKLIB_API TEnum<IOError> copy_file(Str source, Str destination);
MOKLIB_API bool create_symlink(const Str &symlink_path, const Str &target_path, SymLinkKind kind);
MOKLIB_API TimeSpec get_file_time(const Str &file_path);

MOKLIB_API FileHandle open_file(const Str &file_path,EFileMode mode);
MOKLIB_API void close_file(const FileHandle &file);
MOKLIB_API u64 get_file_size(const FileHandle &handle);

MOKLIB_API u64 read_file(FileHandle &handle, void *destination, u64 bytes_to_read, uint64 offset = 0);
// @todo: make this use an offset instead of a tape like interface
MOKLIB_API bool write_file(FileHandle &handle, const void *src, u64 bytes_to_write, u64 *bytes_written, u64 offset = 0);

MOKLIB_API void flush_file_buffers(FileHandle &handle);

MOKLIB_API bool create_dir(const Str &pathname);

/** Get current working directory */
MOKLIB_API Str get_cwd(IAllocator &alloc);

MOKLIB_API bool is_dir(const Str &path);

/** Returns the path of the currently executing process */
MOKLIB_API Str get_base_path(IAllocator &alloc);

/** Resolves path to absolute */
MOKLIB_API Str to_absolute_path(Str relative, IAllocator &alloc);

MOKLIB_API Str directory_of(Str file_path);

static _inline FileHandle open_file_write(Str path) {
    return open_file(path, FileMode::Write | FileMode::CreateAlways);
}

static _inline FileHandle open_file_read(Str path) {
    return open_file(path, FileMode::Read | FileMode::OpenAlways);
}

template <typename T>
bool read_struct(FileHandle &handle, T *destination, uint64 offset = 0) {
    return read_file(handle, destination, sizeof(T), offset) == sizeof(T);
}

struct MOKLIB_API StreamTape : public Tape {
    FileHandle stream_file;

    StreamTape() : stream_file({0}) {}
    StreamTape(FileHandle file) : stream_file(file) {}

    virtual u64  read(void *destination, u64 amount) override;
    virtual bool write(const void *src, u64 amount) override;
    virtual bool end() override;
    virtual void move(i64 offset) override;
};

template <bool AutoClose>
struct TFileTape : public SizedTape {
    FileHandle file;

    TFileTape() {}
    TFileTape(FileHandle file) : file(file) {
        current_offset = 0;
        size = get_file_size(file);
    }

    ~TFileTape() {
        if constexpr (AutoClose) {
            close_file(file);
        }
    }

    u64 read(void *destination, u64 amount) override {
        int64 num_read = read_file(file, destination, amount, current_offset);
        current_offset += num_read;
        return num_read;
    }

    bool write(const void *src, u64 amount) override {
        u64 bytes_written;
        bool success = write_file(file, src, amount, &bytes_written, current_offset);
        current_offset += success ? bytes_written : 0;
        return success;
    }
};

typedef TFileTape<false> FileTape;

MOKLIB_API StreamTape get_stream(Console::Handle kind);

static _inline TFileTape<true> open_write_tape(const Str &path) {
    return TFileTape<true>(open_file_write(path));
}

static _inline TFileTape<true> open_read_tape(const Str &path) {
    return TFileTape<true>(open_file_read(path));
}
