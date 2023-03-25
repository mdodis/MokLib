#pragma once
#include "Config.h"
#include "Console.h"
#include "Error.h"
#include "Host.h"
#include "Memory/Arena.h"
#include "Memory/RawBuffer.h"
#include "Str.h"
#include "Tape.h"
#include "Time/Time.h"

enum class SymLinkKind
{
    File,
    Folder
};

/**
 * New Attributes
 * 0: Read
 * 1: Write
 * 2: ShareReads
 * 3: ShareWrites
 * 4: Create / Open (i.e. Truncate / Open)
 */
namespace FileMode {
    enum Type : u16
    {
        Read       = 1 << 0,
        Write      = 1 << 1,
        ShareRead  = 1 << 2,
        ShareWrite = 1 << 3,
        /** Truncates the file if it exists */
        Truncate   = 1 << 4,
    };
}
typedef u16 EFileMode;

namespace FileAttributes {
    enum Type : u32
    {
        Directory,
        File
    };
};
typedef u32 EFileAttributes;

struct MOKLIB_API FileHandle {
#if OS_MSWINDOWS
    void* internal_handle;
#elif OS_LINUX
    int internal_handle;
#endif
};

#define IS_VALID_FILE(f) ((f).internal_handle != 0)

MOKLIB_API TEnum<IOError> copy_file(Str source, Str destination);
MOKLIB_API bool           create_symlink(
              const Str& symlink_path, const Str& target_path, SymLinkKind kind);

MOKLIB_API TimeSpec get_file_time(const Str& file_path);

MOKLIB_API FileHandle open_file(const Str& file_path, EFileMode mode);
MOKLIB_API void       close_file(const FileHandle& file);
MOKLIB_API u64        get_file_size(const FileHandle& handle);

MOKLIB_API u64 read_file(
    FileHandle& handle,
    void*       destination,
    u64         bytes_to_read,
    uint64      offset = 0);
// @todo: make this use an offset instead of a tape like interface
MOKLIB_API bool write_file(
    FileHandle& handle,
    const void* src,
    u64         bytes_to_write,
    u64*        bytes_written,
    u64         offset = 0);

MOKLIB_API void flush_file_buffers(FileHandle& handle);

MOKLIB_API bool create_dir(const Str& pathname);

/** Get current working directory */
MOKLIB_API Str get_cwd(IAllocator& alloc);

/**
 * Checks whether the file designated by @path is a directory
 * @param  path The path. Must be null terminated
 * @return      True if the file is valid and a directory
 */
MOKLIB_API bool is_dir(const Str& path);

/** Returns the path of the currently executing process */
MOKLIB_API Str get_base_path(IAllocator& alloc);

/**
 * Converts a relative path to an absolute, canonicalized path. Resolves
 * symbolic links
 * @param  relative The relative path. Must be null terminated
 * @param  alloc    Allocator for the absolute path
 * @return          The absolute path, owned by @alloc.
 */
MOKLIB_API Str to_absolute_path(Str relative, IAllocator& alloc);

MOKLIB_API Str directory_of(Str file_path);

static _inline FileHandle open_file_write(Str path)
{
    return open_file(path, FileMode::Write | FileMode::Truncate);
}

static _inline FileHandle open_file_read(Str path)
{
    return open_file(path, FileMode::Read);
}

template <typename T>
bool read_struct(FileHandle& handle, T* destination, uint64 offset = 0)
{
    return read_file(handle, destination, sizeof(T), offset) == sizeof(T);
}

struct MOKLIB_API StreamReadTape : public ReadTape {
    StreamReadTape() : stream_file({0}), ReadTape(read_proc, (void*)this) {}
    StreamReadTape(FileHandle file)
        : stream_file(file), ReadTape(read_proc, (void*)this)
    {}

    static PROC_READ_TAPE_READ(read_proc);

    FileHandle stream_file;
};

struct MOKLIB_API StreamWriteTape : public WriteTape {
    StreamWriteTape() : stream_file({0}), WriteTape(write_proc, (void*)this) {}
    StreamWriteTape(FileHandle file)
        : stream_file(file), WriteTape(write_proc, (void*)this)
    {}

    static PROC_WRITE_TAPE_WRITE(write_proc);

    FileHandle stream_file;
};

struct MOKLIB_API StreamTape : public Tape {
    FileHandle stream_file;

    StreamTape() : stream_file({0}) {}
    StreamTape(FileHandle file) : stream_file(file) {}

    virtual u64  read(void* destination, u64 amount) override;
    virtual bool write(const void* src, u64 amount) override;
    virtual bool end() override;
    virtual void move(i64 offset) override;
};

template <bool AutoClose>
struct FileReadTape : public ReadTape {
    FileReadTape() : ReadTape(read_proc, (void*)this) {}
    FileReadTape(FileHandle file) : ReadTape(read_proc, (void*)this)
    {
        set_file(file);
    }

    FileReadTape(ProcReadTapeRead* proc, void* usr) : ReadTape(proc, usr) {}
    FileReadTape(ProcReadTapeRead* proc, void* usr, FileHandle file)
        : FileReadTape(proc, usr)
    {
        set_file(file);
    }

    ~FileReadTape()
    {
        if constexpr (AutoClose) {
            close_file(file);
        }
    }

    void set_file(FileHandle file)
    {
        this->file = file;
        size       = get_file_size(file);
        offset     = 0;
    }

    i64 _read(void* dst, i64 size)
    {
        if (size <= 0) return -1;

        u64 sizeu64 = (u64)size;

        i64 num_read = read_file(file, dst, size, offset);
        if (num_read < 0) {
            return 0;
        }

        offset += num_read;
        return num_read;
    }

    i64 _seek(i64 off)
    {
        if (off < 0) {
            u64 sizeu64 = (u64)(-off);

            if (sizeu64 > offset) sizeu64 = offset;

            offset -= sizeu64;

            return -((i64)sizeu64);
        } else {
            u64 sizeu64 = (u64)off;

            if ((sizeu64 + offset) > size) sizeu64 = size - offset;

            offset += sizeu64;

            return (i64)sizeu64;
        }
    }

    bool _end() { return offset == size; }

    static PROC_READ_TAPE_READ(read_proc)
    {
        FileReadTape* self = (FileReadTape*)usr;
        switch (mode) {
            case ReadTapeMode::Read: {
                return self->_read(dst, size);
            } break;

            case ReadTapeMode::Seek: {
                return self->_seek(size);
            } break;

            case ReadTapeMode::End: {
                return self->_end();
            } break;

            default: {
                return 0;
            } break;
        }
    }

    u64        offset = 0;
    u64        size;
    FileHandle file;
};

template <bool AutoClose>
struct FileWriteTape : public WriteTape {
    FileWriteTape() : WriteTape(write_proc, (void*)this) {}
    FileWriteTape(FileHandle file) : WriteTape(write_proc, (void*)this)
    {
        set_file(file);
    }

    FileWriteTape(ProcWriteTapeWrite* proc, void* usr) : WriteTape(proc, usr) {}
    FileWriteTape(ProcWriteTapeWrite* proc, void* usr, FileHandle file)
        : FileWriteTape(proc, usr)
    {
        set_file(file);
    }

    ~FileWriteTape()
    {
        if constexpr (AutoClose) {
            close_file(file);
        }
    }

    void set_file(FileHandle file)
    {
        this->file = file;
        offset     = 0;
    }

    i64 _write(void* src, i64 size)
    {
        if (size <= 0) return -1;
        u64 sizeu64 = (u64)size;

        u64  bytes_written;
        bool success = write_file(file, src, size, &bytes_written, offset);
        offset += success ? bytes_written : 0;
        return (i64)sizeu64;
    }

    i64 _seek(i64 off)
    {
        if (off < 0) {
            u64 sizeu64 = (u64)(-off);

            if (sizeu64 > offset) sizeu64 = offset;

            offset -= sizeu64;

            return -((i64)sizeu64);
        } else {
            u64 sizeu64 = (u64)off;

            offset += sizeu64;

            return (i64)sizeu64;
        }
    }

    static PROC_WRITE_TAPE_WRITE(write_proc)
    {
        FileWriteTape* self = (FileWriteTape*)usr;

        switch (mode) {
            case WriteTapeMode::Write: {
                return self->_write(src, size);
            } break;

            case WriteTapeMode::Seek: {
                return self->_seek(size);
            } break;

            case WriteTapeMode::End: {
                return 0;
            } break;

            default: {
                return 0;
            } break;
        }
    }

    u64        offset = 0;
    FileHandle file;
};

template <bool AutoClose>
struct BufferedReadTape : public FileReadTape<AutoClose> {
    using Super = FileReadTape<AutoClose>;

    BufferedReadTape(
        FileHandle file, IAllocator& allocator, u64 size = KILOBYTES(1))
        : Super(read_proc, (void*)this, file), size(size), allocator(allocator)
    {
        buffer = allocator.reserve(size);
    }

    ~BufferedReadTape() { allocator.release(buffer); }

    void flush(void* dst, u64 count)
    {
        memcpy(dst, buffer, count);
        accumulated -= count;
    }

    static PROC_READ_TAPE_READ(read_proc)
    {
        BufferedReadTape* self = (BufferedReadTape*)usr;

        switch (mode) {
            case ReadTapeMode::Read: {
                if (size <= 0) return 0;

                u64 sizeu64 = (u64)size;

                u64 num_read = 0;
                while (num_read != sizeu64) {
                    if (self->accumulated == 0) {
                        self->accumulated =
                            self->_read(self->buffer, self->size);

                        if (self->accumulated == 0) return num_read;
                    }

                    // Finish any pending reads
                    u64 max_bytes_to_read = sizeu64 - num_read;
                    u64 min_bytes_to_read = self->accumulated;

                    u64 bytes_to_read = max_bytes_to_read > min_bytes_to_read
                                          ? min_bytes_to_read
                                          : max_bytes_to_read;

                    if (bytes_to_read > 0) {
                        self->flush(umm(dst) + num_read, bytes_to_read);
                    }

                    num_read += bytes_to_read;
                }

                return num_read;
            } break;

            case ReadTapeMode::Seek: {
                self->accumulated = 0;
                return self->_seek(size);
            } break;

            case ReadTapeMode::End: {
                return self->_end();
            } break;

            default: {
                return 0;
            } break;
        }
    }

    umm         buffer      = 0;
    u64         size        = 0;
    u64         accumulated = 0;
    IAllocator& allocator;
};

template <bool AutoClose>
struct BufferedWriteTape : public FileWriteTape<AutoClose> {
    using Super = FileWriteTape<AutoClose>;

    BufferedWriteTape(
        FileHandle file, IAllocator& allocator, u64 size = KILOBYTES(1))
        : Super(write_proc, (void*)this, file), size(size), allocator(allocator)
    {
        buffer = allocator.reserve(size);
    }

    ~BufferedWriteTape()
    {
        flush();
        allocator.release(buffer);
    }

    static PROC_WRITE_TAPE_WRITE(write_proc)
    {
        BufferedWriteTape* self = (BufferedWriteTape*)usr;

        switch (mode) {
            case WriteTapeMode::Write: {
                i64 num_written = 0;
                while (num_written != size) {
                    if (self->accumulated == self->size) {
                        if (!self->flush()) return 0;
                    }

                    u64 remaining      = self->size - self->accumulated;
                    u64 bytes_to_write = remaining < (size - num_written)
                                           ? remaining
                                           : (size - num_written);

                    memcpy(
                        self->buffer + self->accumulated,
                        umm(src) + num_written,
                        bytes_to_write);

                    self->accumulated += bytes_to_write;
                    num_written += bytes_to_write;
                }

                if (self->accumulated == self->size) {
                    if (!self->flush()) return 0;
                }

                return (i64)num_written;
            } break;

            case WriteTapeMode::Seek: {
                self->flush();
                return self->_seek(size);
            } break;

            case WriteTapeMode::End: {
                return 0;
            } break;

            default: {
                return 0;
            } break;
        }
    }

    bool flush()
    {
        if (Super::_write(buffer, accumulated) != accumulated) {
            return false;
        }
        accumulated = 0;
        return true;
    }

    umm         buffer      = 0;
    u64         size        = 0;
    u64         accumulated = 0;
    IAllocator& allocator;
};

template <bool AutoClose>
struct TFileTape : public SizedTape {
    FileHandle file;

    TFileTape() {}
    TFileTape(FileHandle file) : file(file)
    {
        current_offset = 0;
        size           = get_file_size(file);
    }

    ~TFileTape()
    {
        if constexpr (AutoClose) {
            close_file(file);
        }
    }

    void move_to_end() { move(get_file_size(file)); }

    u64 read(void* destination, u64 amount) override
    {
        // @todo: u64 reads
        int64 num_read = read_file(file, destination, amount, current_offset);
        if (num_read < 0) {
            return 0;
        }
        current_offset += num_read;
        return num_read;
    }

    bool write(const void* src, u64 amount) override
    {
        u64  bytes_written;
        bool success =
            write_file(file, src, amount, &bytes_written, current_offset);
        current_offset += success ? bytes_written : 0;
        return success;
    }
};

template <bool AutoClose>
struct TBufferedFileTape : TFileTape<AutoClose> {
    using Super = TFileTape<AutoClose>;
    umm         buffer;
    u64         size;
    u64         accumulated;
    IAllocator& allocator;
    TBufferedFileTape(
        FileHandle file_handle,
        u64 size = KILOBYTES(1),  // 1-8Kb buffer size shows good perf readings
        IAllocator& allocator = System_Allocator)
        : Super(file_handle), size(size), accumulated(0), allocator(allocator)
    {
        buffer = allocator.reserve(size);
    }

    ~TBufferedFileTape()
    {
        flush();
        allocator.release(buffer);
    }

    bool flush()
    {
        if (!Super::write(buffer, accumulated)) {
            return false;
        }
        accumulated = 0;
        return true;
    }

    bool write(const void* src, u64 amount) override
    {
        u64 num_written = 0;
        while (num_written != amount) {
            if (accumulated == size) {
                if (!flush()) return false;
            }

            u64 remaining      = size - accumulated;
            u64 bytes_to_write = min(remaining, amount - num_written);

            memcpy(
                buffer + accumulated,
                umm(src) + num_written,
                bytes_to_write);

            accumulated += bytes_to_write;
            num_written += bytes_to_write;
        }

        if (accumulated == size) {
            if (!flush()) return false;
        }

        return true;
    }
};

typedef TFileTape<false> FileTape;

MOKLIB_API StreamTape      get_stream(Console::Handle kind);
MOKLIB_API StreamReadTape  get_read_stream(Console::Handle kind);
MOKLIB_API StreamWriteTape get_write_stream(Console::Handle kind);

static _inline TFileTape<true> open_write_tape(const Str& path)
{
    return TFileTape<true>(open_file_write(path));
}

static _inline TFileTape<true> open_read_tape(const Str& path)
{
    return TFileTape<true>(open_file_read(path));
}
