#include "FileSystem.h"

#include <sys/types.h>

#include "../Host.h"
#include "Base.h"
#include "Console.h"
#include "Containers/Array.h"
#include "Debugging/Assertions.h"
#include "Math/Base.h"
#include "Time/Time.h"
#include "Traits.h"

Str directory_of(Str file_path)
{
    u64 last_separator = file_path.last_of('/');
    if (last_separator == file_path.len) {
        last_separator = file_path.last_of('\\');
    }

    if (last_separator == file_path.len) return Str::NullStr;

    return file_path.chop_right(
        last_separator == 0 ? last_separator : last_separator - 1);
}

Str get_file_name(Str directory_path)
{
    auto is_sep = [](char c) { return c == '/' || c == '\\'; };
    u64  start  = directory_path.len - 1;
    u64  i      = start;

    // Find the last '/' or '\\'
    while (i != 0) {
        if (!is_sep(directory_path[i])) {
            break;
        }
        i--;
    }

    u64 r_end = i + 1;

    // Find the next '/' or '\\'
    while (i != 0) {
        if (is_sep(directory_path[i])) {
            break;
        }
        i--;
    }

    u64 r_start = i + 1;

    return directory_path.chop_middle(r_start, r_end);
}

#if OS_MSWINDOWS
#include "Compat/Win32Internal.inc"

TEnum<IOError> copy_file(Str source, Str destination)
{
    Allocator& allocator = System_Allocator;

    u32      wsource_len;
    wchar_t* wsource = multibyte_to_wstr(source, &wsource_len, allocator);
    DEFER(allocator.release(wsource));

    u32      wdest_len;
    wchar_t* wdest = multibyte_to_wstr(destination, &wdest_len, allocator);
    DEFER(allocator.release(wdest));

    if (!CopyFileW(wsource, wdest, FALSE)) {
        return to_io_error(GetLastError());
    }

    return IOError::None;
}

bool create_symlink(
    const Str& symlink_path, const Str& target_path, SymLinkKind kind)
{
    static wchar_t Sym_Link_PathW[1024];
    static wchar_t Target_PathW[1024];

    int link_lenw = MultiByteToWideChar(
        CP_UTF8,
        MB_ERR_INVALID_CHARS,
        (const char*)symlink_path.data,
        (i32)symlink_path.len,
        Sym_Link_PathW,
        ARRAY_COUNT(Sym_Link_PathW));

    Sym_Link_PathW[link_lenw] = 0;

    int target_lenw = MultiByteToWideChar(
        CP_UTF8,
        MB_ERR_INVALID_CHARS,
        (const char*)target_path.data,
        (u32)target_path.len,
        Target_PathW,
        ARRAY_COUNT(Target_PathW));

    Target_PathW[target_lenw] = 0;

    DWORD flags;

    switch (kind) {
        case SymLinkKind::File:
            flags = 0;
            break;

        case SymLinkKind::Folder:
            flags = SYMBOLIC_LINK_FLAG_DIRECTORY;
            break;
    }

    return CreateSymbolicLinkW(Sym_Link_PathW, Target_PathW, flags);
}

FileHandle open_file(const Str& file_path, EFileMode mode)
{
    static wchar_t file_pathw[1024];

    int file_path_lenw = MultiByteToWideChar(
        CP_UTF8,
        MB_ERR_INVALID_CHARS,
        (const char*)file_path.data,
        (u32)file_path.len,
        file_pathw,
        ARRAY_COUNT(file_pathw));

    file_pathw[file_path_lenw] = 0;

    DWORD access_flags = 0;
    if (mode & FileMode::Read) access_flags |= GENERIC_READ;
    if (mode & FileMode::Write) access_flags |= GENERIC_WRITE;

    DWORD share_mode = 0;
    if (mode & FileMode::ShareRead) share_mode |= FILE_SHARE_READ;
    if (mode & FileMode::ShareWrite) share_mode |= FILE_SHARE_WRITE;

    DWORD creation = 0;
    if (mode & FileMode::Truncate)
        creation = CREATE_ALWAYS;
    else
        creation = OPEN_EXISTING;

    DWORD attribs = FILE_ATTRIBUTE_NORMAL;
    // if (mode & FileMode::NoBuffering) attribs |= FILE_FLAG_NO_BUFFERING |
    // FILE_FLAG_WRITE_THROUGH;

    HANDLE result = CreateFileW(
        file_pathw,
        access_flags,
        share_mode,
        0,
        creation,
        attribs,
        0);

    if (result == INVALID_HANDLE_VALUE) {
        DWORD last_error = GetLastError();

        return FileHandle{0};
    }

    return FileHandle{(void*)result};
}

u64 get_file_size(const FileHandle& handle)
{
    return GetFileSize((HANDLE)handle.internal_handle, 0);
}

TimeSpec get_file_time(const Str& file_path)
{
    ASSERT(file_path.has_null_term);

    HANDLE file_handle = CreateFileA(
        (char*)file_path.data,
        0,
        FILE_SHARE_WRITE,
        0,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        0);

    FILETIME time;
    GetFileTime(file_handle, 0, 0, &time);

    CloseHandle(file_handle);

    ULARGE_INTEGER time_int;
    time_int.HighPart = time.dwHighDateTime;
    time_int.LowPart  = time.dwLowDateTime;
    return TimeSpec{time_int.QuadPart};
}

u64 read_file(
    FileHandle& handle, void* destination, u64 bytes_to_read, u64 offset)
{
    LONG low_distance  = LONG(offset & ((~0ull) >> 32));
    LONG high_distance = LONG(offset >> 32);

    SetFilePointer(
        handle.internal_handle,
        low_distance,
        &high_distance,
        FILE_BEGIN);

    // @todo: actually read u64
    DWORD bytes_read;
    BOOL  success = ReadFile(
        (HANDLE)handle.internal_handle,
        destination,
        (u32)bytes_to_read,
        &bytes_read,
        0);

    if (success) {
        return bytes_read;
    } else {
        return -1;
    }
}

bool write_file(
    FileHandle& handle,
    const void* src,
    u64         bytes_to_write,
    u64*        bytes_written,
    u64         offset)
{
    LONG low_distance  = LONG(offset & ((~0ull) >> 32));
    LONG high_distance = LONG(offset >> 32);

    SetFilePointer(
        (HANDLE)handle.internal_handle,
        low_distance,
        &high_distance,
        FILE_BEGIN);

    DWORD bytes_written32;
    BOOL  success = WriteFile(
        (HANDLE)handle.internal_handle,
        src,
        (u32)bytes_to_write,
        &bytes_written32,
        0);

    if (success && bytes_written) *bytes_written = bytes_written32;

    return success;
}

void flush_file_buffers(FileHandle& handle)
{
    FlushFileBuffers((HANDLE)handle.internal_handle);
}

void close_file(const FileHandle& file)
{
    CloseHandle((HANDLE)file.internal_handle);
}

bool create_dir(const Str& pathname)
{
    char*       dirname;
    static char buf[260];
    if (pathname.has_null_term) {
        dirname = (char*)pathname.data;
    } else {
        memcpy(buf, pathname.data, pathname.len);
        buf[pathname.len] = 0;
        dirname           = buf;
    }

    if (!CreateDirectoryA(dirname, 0)) {
        DWORD h = GetLastError();
        DEBUG_PRINTF("CreateDirectoryA failed with %d", h);
        return false;
    }
    return true;
}

Str get_cwd(Allocator& alloc)
{
    static thread_local wchar_t buf[MAX_PATH];
    DWORD                       num_chars = GetCurrentDirectoryW(MAX_PATH, buf);

    return wstr_to_multibyte(buf, num_chars, alloc);
}

bool is_dir(const Str& path)
{
    Allocator& alloc = System_Allocator;

    wchar_t* wpath = multibyte_to_wstr(path, 0, alloc);
    DEFER(alloc.release((umm)wpath););

    DWORD attribs = GetFileAttributesW(wpath);

    return (attribs != INVALID_FILE_ATTRIBUTES) &&
           (attribs & FILE_ATTRIBUTE_DIRECTORY);
}

// @todo: account for this horrible API allocate wchar_t using TArray since path
// names can be larger than MAX_PATH
Str get_base_path(Allocator& alloc)
{
    wchar_t* ptr = (wchar_t*)alloc.reserve(MAX_PATH * sizeof(wchar_t));
    DEFER(alloc.release((umm)ptr));

    DWORD num_chars = GetModuleFileNameW(0, ptr, MAX_PATH);

    Str result = wstr_to_multibyte(ptr, num_chars, alloc);

    return result.chop_right_last_of(LIT("\\"));
}

Str to_absolute_path(Str relative, Allocator& alloc)
{
    u32      wrelative_len;
    wchar_t* wrelative = multibyte_to_wstr(relative, &wrelative_len, alloc);
    DEFER(alloc.release(wrelative));

    u32 wabsolute_len = GetFullPathNameW(wrelative, 0, 0, 0);

    wchar_t* wptr = (wchar_t*)alloc.reserve(wabsolute_len * sizeof(wchar_t));
    DEFER(alloc.release(wptr));

    u32 wwritten = GetFullPathNameW(wrelative, wabsolute_len, wptr, 0);

    return wstr_to_multibyte(wptr, wwritten, alloc);
}

PROC_READ_TAPE_READ(StreamReadTape::read_proc)
{
    StreamReadTape* self = (StreamReadTape*)usr;

    switch (mode) {
        case ReadTapeMode::Read: {
            DWORD bytes_read;
            BOOL  success = ReadFile(
                (HANDLE)self->stream_file.internal_handle,
                dst,
                (i32)size,
                &bytes_read,
                0);

            if (!success) return 0;

            return (i64)bytes_read;
        } break;

        case ReadTapeMode::Seek: {
            LONG low_distance  = LONG(size & ((~0ull) >> 32));
            LONG high_distance = LONG(size >> 32);

            DWORD result = SetFilePointer(
                (HANDLE)self->stream_file.internal_handle,
                low_distance,
                &high_distance,
                FILE_CURRENT);

            if (result == INVALID_SET_FILE_POINTER) return 0;

            return i64(result) & (i64(high_distance) << 32);
        } break;

        case ReadTapeMode::End: {
            DWORD bytes_read;
            char  dest;
            BOOL  success = ReadFile(
                (HANDLE)self->stream_file.internal_handle,
                &dest,
                1,
                &bytes_read,
                0);

            return success && (bytes_read != 0);
        } break;

        default: {
            return 0;
        } break;
    }
}

PROC_WRITE_TAPE_WRITE(StreamWriteTape::write_proc)
{
    StreamReadTape* self = (StreamReadTape*)usr;
    switch (mode) {
        case WriteTapeMode::Write: {
            // @todo: i64 writes
            DWORD bytes_written;
            BOOL  success = WriteFile(
                (HANDLE)self->stream_file.internal_handle,
                src,
                (u32)size,
                &bytes_written,
                0);

            return bytes_written;
        } break;

        case WriteTapeMode::Seek: {
            LONG low_distance  = LONG(size & ((~0ull) >> 32));
            LONG high_distance = LONG(size >> 32);

            DWORD result = SetFilePointer(
                (HANDLE)self->stream_file.internal_handle,
                low_distance,
                &high_distance,
                FILE_CURRENT);

            if (result == INVALID_SET_FILE_POINTER) return 0;

            return i64(result) & (i64(high_distance) << 32);
        } break;

        case WriteTapeMode::End: {
            DWORD bytes_read;
            char  dest;
            BOOL  success = ReadFile(
                (HANDLE)self->stream_file.internal_handle,
                &dest,
                1,
                &bytes_read,
                0);

            return success && (bytes_read != 0);
        } break;

        default: {
            return 0;
        } break;
    }
}

u64 StreamTape::read(void* destination, u64 amount)
{
    DWORD bytes_read;
    BOOL  success = ReadFile(
        (HANDLE)stream_file.internal_handle,
        destination,
        (i32)amount,
        &bytes_read,
        0);

    if (success && (bytes_read == 0)) {
        return NumProps<u64>::max;
    } else if (!success) {
        return NumProps<u64>::max;
    } else {
        return bytes_read;
    }
}

bool StreamTape::write(const void* src, u64 amount)
{
    DWORD bytes_written;
    BOOL  success = WriteFile(
        (HANDLE)stream_file.internal_handle,
        src,
        (u32)amount,
        &bytes_written,
        0);
    return success;
}

bool StreamTape::end()
{
    DWORD bytes_read;
    char  dest;
    BOOL  success =
        ReadFile((HANDLE)stream_file.internal_handle, &dest, 1, &bytes_read, 0);

    return success && (bytes_read != 0);
}

StreamReadTape get_read_stream(Console::Handle kind)
{
    HANDLE handle = GetStdHandle(kind);
    return StreamReadTape(FileHandle{(void*)handle});
}

StreamWriteTape get_write_stream(Console::Handle kind)
{
    HANDLE handle = GetStdHandle(kind);
    return StreamWriteTape(FileHandle{(void*)handle});
}

StreamTape get_stream(Console::Handle kind)
{
    HANDLE handle = GetStdHandle(kind);
    return StreamTape(FileHandle{(void*)handle});
}

void StreamTape::move(i64 offset)
{
    LONG low_distance  = LONG(offset & ((~0ull) >> 32));
    LONG high_distance = LONG(offset >> 32);

    SetFilePointer(
        (HANDLE)stream_file.internal_handle,
        low_distance,
        &high_distance,
        FILE_CURRENT);
}

#elif OS_LINUX
#include <fcntl.h>
#include <limits.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "Compat/UnixInternal.inc"

FileHandle open_file(const Str& file_path, EFileMode mode)
{
    static char path[1024];
    memcpy(path, file_path.data, file_path.len);
    path[file_path.len] = 0;

    int access_flags = 0;

    if ((mode & FileMode::Write) && (mode & FileMode::Read)) {
        access_flags |= O_RDWR;
    } else if (mode & FileMode::Write) {
        access_flags |= O_WRONLY;
    } else if (mode & FileMode::Read) {
        access_flags |= O_RDONLY;
    }

    if (mode & FileMode::Truncate) {
        truncate(path, 0);
        access_flags |= O_CREAT;
    }

    int  fd = open(path, access_flags, 0644);
    auto e  = errno;

    if (fd == -1) {
        return FileHandle{0};
    }

    FileHandle result;
    result.internal_handle = fd;

    return result;
}

static void seek_file(int fd, u64 off)
{
    lseek(fd, 0, SEEK_SET);
    while (off != 0) {
        off_t soff = min((off_t)off, NumProps<off_t>::max);
        lseek(fd, soff, SEEK_CUR);

        off -= soff;
    }
}

u64 read_file(
    FileHandle& handle, void* destination, u64 bytes_to_read, u64 offset)
{
    int fd = handle.internal_handle;
    seek_file(fd, offset);
    return read(fd, destination, bytes_to_read);
}

bool write_file(
    FileHandle& handle,
    const void* src,
    u64         bytes_to_write,
    u64*        bytes_written,
    u64         offset)
{
    // @todo off_t is signed integer, use better offsets
    int fd = handle.internal_handle;

    // Seek to start
    seek_file(fd, offset);

    i64 result = write(fd, src, bytes_to_write);

    if (result < 0) {
        *bytes_written = 0;
        return false;
    } else {
        *bytes_written = result;
        return true;
    }
}

u64 get_file_size(const FileHandle& handle)
{
    int         fd = handle.internal_handle;
    struct stat statinfo;
    fstat(fd, &statinfo);

    return (u64)statinfo.st_size;
}

TimeSpec get_file_time(const Str& file_path)
{
    // @todo: need to find a better way to temp convert
    // to null term strings
    ASSERT(file_path.has_null_term);

    struct stat stat_result;
    stat((char*)file_path.data, &stat_result);

    return TimeSpec{stat_result.st_mtim};
}

void close_file(const FileHandle& file)
{
    int fd = file.internal_handle;
    close(fd);
}

bool create_dir(const Str& pathname)
{
    char*       dirname;
    static char buf[260];
    if (pathname.has_null_term) {
        dirname = (char*)pathname.data;
    } else {
        memcpy(buf, pathname.data, pathname.len);
        buf[pathname.len] = 0;
        dirname           = buf;
    }

    return mkdir(dirname, 0755) == 0;
}

Str get_base_path(Allocator& alloc)
{
    umm     ptr = alloc.reserve(PATH_MAX);
    ssize_t len = readlink("/proc/self/exe", (char*)ptr, PATH_MAX);
    Str     result((char*)ptr, len);

    return result.chop_right(result.last_of(LIT("/")));
}

Str get_cwd(Allocator& alloc)
{
    // @note: And here I thought that just winapi had some terrible parts
    TArray<char> result(&alloc);
    char*        ret;

    result.init(256);

    while (getcwd(result.data, result.capacity) != result.data) {
        if (!result.stretch()) {
            result.release();
            return Str::NullStr;
        }
    }

    return Str((char*)result.data);
}

StreamTape get_stream(Console::Handle kind)
{
    return StreamTape(FileHandle{(int)kind});
}

PROC_READ_TAPE_READ(StreamReadTape::read_proc)
{
    StreamReadTape* self = (StreamReadTape*)usr;
    switch (mode) {
        case ReadTapeMode::Read: {
            int fd = self->stream_file.internal_handle;
            return ::read(fd, dst, size);
        } break;

        case ReadTapeMode::Seek: {
            int fd = self->stream_file.internal_handle;
            lseek(fd, SEEK_CUR, size);
        } break;

        case ReadTapeMode::End: {
            int  fd = self->stream_file.internal_handle;
            char c;
            return ::read(fd, &c, 1) == -1;
        } break;

        default: {
            return 0;
        } break;
    }
}

PROC_WRITE_TAPE_WRITE(StreamWriteTape::write_proc)
{
    StreamWriteTape* self = (StreamWriteTape*)usr;
    switch (mode) {
        case WriteTapeMode::Write: {
            int     fd      = self->stream_file.internal_handle;
            ssize_t written = ::write(fd, src, size);
            return (written == size);
        } break;

        case WriteTapeMode::Seek: {
            int fd = self->stream_file.internal_handle;
            lseek(fd, SEEK_CUR, size);
        } break;

        case WriteTapeMode::End: {
            int  fd = self->stream_file.internal_handle;
            char c;
            return ::read(fd, &c, 1) == -1;
        } break;

        default: {
            return 0;
        } break;
    }
}

u64 StreamTape::read(void* destination, u64 amount)
{
    int fd = stream_file.internal_handle;
    return ::read(fd, destination, amount);
}

bool StreamTape::write(const void* src, u64 amount)
{
    int     fd      = stream_file.internal_handle;
    ssize_t written = ::write(fd, src, amount);
    return (written == amount);
}

bool StreamTape::end()
{
    int  fd = stream_file.internal_handle;
    char c;
    return ::read(fd, &c, 1) == -1;
}

void StreamTape::move(i64 offset)
{
    int fd = stream_file.internal_handle;
    lseek(fd, SEEK_CUR, offset);
}

bool is_dir(const Str& path)
{
    ASSERT(path.has_null_term);

    struct stat statbuf;
    stat(path.data, &statbuf);

    return (statbuf.st_mode & S_IFDIR);
}

Str to_absolute_path(Str relative, Allocator& alloc)
{
    ASSERT(relative.has_null_term);

    char* result = realpath(relative.data, NULL);

    auto e = errno;

    if (!result) {
        return Str::NullStr;
    }

    DEFER(free(result));

    return Str(result).clone(alloc);
}

TEnum<IOError> copy_file(Str source, Str destination)
{
    ASSERT(source.has_null_term);
    ASSERT(destination.has_null_term);
    int from_fd, to_fd;
    from_fd = open((char*)source.data, O_RDONLY);
    if (from_fd == -1) {
        return to_io_error(errno);
    }

    to_fd = open(
        (char*)destination.data,
        O_WRONLY | O_EXCL | O_CREAT | O_TRUNC,
        0666);
    if (to_fd == -1) {
        return to_io_error(errno);
    }

    constexpr u64 buf_size = 1024;
    char          buf[buf_size];

    size_t nread, nwritten;

    while (nread = read(from_fd, buf, buf_size), nread > 0) {
        char* out_buf = buf;

        do {
            nwritten = write(to_fd, out_buf, nread);

            if (nwritten >= 0) {
                nread -= nwritten;
                out_buf += nwritten;
            } else {
                return IOError::Unrecognized;
            }
        } while (nread > 0);
    }

    close(from_fd);
    close(to_fd);

    return IOError::None;
}

#else

#error "Unsupported platform for FileSystem"

#endif
