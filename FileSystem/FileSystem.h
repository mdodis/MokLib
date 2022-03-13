#pragma once
#include "../Config.h"
#include "Host.h"
#include "Str.h"
#include "Memory/RawBuffer.h"
#include "Memory/Arena.h"
#include "Time/Time.h"
#include "Tape.h"
#include "Console.h"
#include "StringBuilder.h"

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

struct MOKLIB_API FileHandle {
#if OS_MSWINDOWS
    void *internal_handle;
#elif OS_LINUX
    int internal_handle;
#endif

};
#define IS_VALID_FILE(f) ((f).internal_handle != 0)

MOKLIB_API bool create_symlink(const Str &symlink_path, const Str &target_path, SymLinkKind kind);
MOKLIB_API FileHandle open_file(const Str &file_path,EFileMode mode);
MOKLIB_API u64 get_file_size(const FileHandle &handle);
MOKLIB_API TimeSpec get_file_time(const Str &file_path);
MOKLIB_API u64 read_file(FileHandle &handle, void *destination, u64 bytes_to_read, uint64 offset = 0);
// @todo: make this use an offset instead of a tape like interface
MOKLIB_API bool write_file(FileHandle &handle, const void *src, u64 bytes_to_write, u64 *bytes_written, u64 offset = 0);

MOKLIB_API void flush_file_buffers(FileHandle &handle);
MOKLIB_API void close_file(const FileHandle &file);

MOKLIB_API bool create_dir(const Str &pathname);

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

struct FileTape : public SizedTape {
    FileHandle file;

    FileTape(FileHandle file) : file(file) {
        current_offset = 0;
        size = get_file_size(file);
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


MOKLIB_API StreamTape get_stream(Console::Handle kind);
MOKLIB_API extern Arena Print_Arena;

#define PRINT(what) do { \
        SAVE_ARENA(&Print_Arena); \
        _print(Console::Output, (StringBuilder(&Print_Arena) + what).to_list()); \
    } while (0)

#define PRINTLN(what) PRINT(what + "\n")

static _inline void _print(Console::Handle to, const TList<Str> &list) {
    auto stream = get_stream(to);

    for (const Str &str : list) {
        stream.write_str(str);
    }
}