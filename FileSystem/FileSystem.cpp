#include "FileSystem.h"
#include "../Host.h"
#include "Time/Time.h"
#include <sys/types.h>

#if OS_WINDOWS
#include "../WinInc.h"

bool create_symlink(const Str &symlink_path, const Str &target_path, SymLinkKind kind) {

    static wchar_t Sym_Link_PathW[1024];
    static wchar_t Target_PathW[1024];

    int link_lenw = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, (const char*)symlink_path.data, symlink_path.len, Sym_Link_PathW, ARRAY_COUNT(Sym_Link_PathW));

    Sym_Link_PathW[link_lenw] = 0;

    int target_lenw = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, (const char*)target_path.data, target_path.len, Target_PathW, ARRAY_COUNT(Target_PathW));

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

FileHandle open_file(const Str &file_path, EFileMode mode) {
    static wchar_t file_pathw[1024];

    int file_path_lenw = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, (const char*)file_path.data, file_path.len, file_pathw, ARRAY_COUNT(file_pathw));

    file_pathw[file_path_lenw] = 0;

    DWORD access_flags = 0;
    if (mode & FileMode::Read)      access_flags |= GENERIC_READ;
    if (mode & FileMode::Write)     access_flags |= GENERIC_WRITE;
    if (mode & FileMode::Append)    access_flags |= FILE_APPEND_DATA;

    DWORD share_mode = 0;
    if (mode & FileMode::ShareRead)  share_mode |= FILE_SHARE_READ;
    if (mode & FileMode::ShareWrite) share_mode |= FILE_SHARE_WRITE;

    DWORD creation = 0;
    if      (mode & FileMode::CreateAlways) creation |= CREATE_ALWAYS;
    else if (mode & FileMode::OpenAlways)   creation |= OPEN_ALWAYS;
    else creation = OPEN_EXISTING;

    DWORD attribs = FILE_ATTRIBUTE_NORMAL;
    if (mode & FileMode::NoBuffering) attribs |= FILE_FLAG_NO_BUFFERING | FILE_FLAG_WRITE_THROUGH;

    HANDLE result = CreateFileW(file_pathw,
        access_flags,
        share_mode,
        0,
        creation,
        attribs,
        0);

    if (result == INVALID_HANDLE_VALUE) {
        DWORD last_error = GetLastError();

        return FileHandle {0};
    }

    return FileHandle {
        (void*)result
    };
}

u64 get_file_size(const FileHandle &handle) {
    return GetFileSize((HANDLE)handle.internal_handle, 0);
}

MTime::TimeSpec get_file_time(const Str &file_path) {
    ASSERT(file_path.has_null_term);

    HANDLE file_handle = CreateFileA((char*)file_path.data,  0, FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    FILETIME time;
    GetFileTime(file_handle, 0, 0, &time);


    CloseHandle(file_handle);

    ULARGE_INTEGER time_int;
    time_int.HighPart = time.dwHighDateTime;
    time_int.LowPart = time.dwLowDateTime;
    return MTime::TimeSpec{ time_int.QuadPart };
}

int64 read_file(FileHandle &handle, void *destination, u32 bytes_to_read, uint64 offset) {
    LONG low_distance  = LONG(offset & ((~0ull) >> 32));
    LONG high_distance = LONG(offset >> 32);

    SetFilePointer(
        handle.internal_handle,
        low_distance,
        &high_distance,
        FILE_BEGIN);

    DWORD bytes_read;
    BOOL success = ReadFile((HANDLE)handle.internal_handle, destination, bytes_to_read, &bytes_read, 0);

    if (success) {
        return bytes_read;
    } else {
        return -1;
    }
}

bool write_file(FileHandle &handle, const void *src, u32 bytes_to_write, u64 *bytes_written) {
    return WriteFile((HANDLE)handle.internal_handle, src, bytes_to_write, (LPDWORD)bytes_written, 0);
}


void flush_file_buffers(FileHandle &handle) {
    FlushFileBuffers((HANDLE)handle.internal_handle);
}

void close_file(const FileHandle &file) {
    CloseHandle((HANDLE)file.internal_handle);
}

bool create_dir(const Str &pathname) {

    char *dirname;
    static char buf[260];
    if (pathname.has_null_term) {
        dirname = (char*)pathname.data;
    } else {
        memcpy(buf, pathname.data, pathname.len);
        buf[pathname.len] = 0;
        dirname = buf;
    }

    return CreateDirectoryA(dirname, 0);
}


#elif OS_LINUX
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

FileHandle open_file(const Str &file_path, EFileMode mode) {
    static char path[1024];
    memcpy(path, file_path.data, file_path.len);
    path[file_path.len] = 0;

    int access_flags = 0;

    if (mode & FileMode::Read)      access_flags |= S_IRUSR;
    if (mode & FileMode::Write)     access_flags |= S_IWUSR;

    int fd = open(path, access_flags);
    if (fd == -1) {
        return FileHandle { 0 };
    }

    FileHandle result;
    result.internal_handle = fd;

    return result;
}

int64 read_file(FileHandle &handle, void *destination, u32 bytes_to_read, uint64 offset) {
    int fd = handle.internal_handle;
    return pread(fd, destination, bytes_to_read, offset);
}

bool write_file(FileHandle &handle, const void *src, u32 bytes_to_write, u64 *bytes_written) {
    int fd = handle.internal_handle;
    i64 result = pwrite(fd, src, bytes_to_write, 0);
    if (result < 0) {
        *bytes_written = 0;
        return false;
    } else {
        *bytes_written = result;
        return true;
    }
}

u64 get_file_size(const FileHandle &handle) {
    int fd = handle.internal_handle;
    struct stat statinfo;
    fstat(fd, &statinfo);

    return (u64)statinfo.st_size;
}

MTime::TimeSpec get_file_time(const Str &file_path) {
    // @todo: need to find a better way to temp convert
    // to null term strings
    ASSERT(file_path.has_null_term);

    struct stat stat_result;
    stat((char*)file_path.data, &stat_result);

    return MTime::TimeSpec{stat_result.st_mtim};
}


void close_file(const FileHandle &file) {
    int fd = file.internal_handle;
    close(fd);
}

#else

#error "Unsupported platform for FileSystem"

#endif
