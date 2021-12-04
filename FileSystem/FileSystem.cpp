#include "FileSystem.h"
#include "../Host.h"
#include "Time/Time.h"

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

FileHandle open_file(const Str &file_path, FileMode::Type mode) {
    static wchar_t file_pathw[1024];

    int file_path_lenw = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, (const char*)file_path.data, file_path.len, file_pathw, ARRAY_COUNT(file_pathw));

    file_pathw[file_path_lenw] = 0;

    DWORD access_flags = 0;

    if (mode & FileMode::Read)      access_flags |= GENERIC_READ;
    if (mode & FileMode::Write)     access_flags |= GENERIC_WRITE;

    DWORD share_mode = 0;
    if (mode & FileMode::ShareRead)  share_mode |= FILE_SHARE_READ;
    if (mode & FileMode::ShareWrite) share_mode |= FILE_SHARE_WRITE;

    HANDLE result = CreateFileW(file_pathw, access_flags, share_mode, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if (result == INVALID_HANDLE_VALUE) {
        return FileHandle {0};
    }

    return FileHandle {
        (void*)result
    };
}

uint32 get_file_size(const FileHandle &handle) {
    return GetFileSize((HANDLE)handle.internal_handle, 0);
}

int64 read_file(FileHandle &handle, void *destination, uint32 bytes_to_read, uint64 offset) {
    SetFilePointer(handle.internal_handle, (uint32)offset, 0, FILE_BEGIN);

    DWORD bytes_read;
    BOOL success = ReadFile((HANDLE)handle.internal_handle, destination, bytes_to_read, &bytes_read, 0);

    if (success) {
        return bytes_read;
    } else {
        return -1;
    }
}

void close_file(const FileHandle &file) {
    CloseHandle((HANDLE)file.internal_handle);
}

#elif OS_LINUX
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

FileHandle open_file(const Str &file_path, FileMode::Type mode) {
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

int64 read_file(FileHandle &handle, void *destination, uint32 bytes_to_read, uint64 offset) {
    int fd = handle.internal_handle;
    return pread(fd, destination, bytes_to_read, offset);
}

uint32 get_file_size(const FileHandle &handle) {
    int fd = handle.internal_handle;
    struct stat statinfo;
    fstat(fd, &statinfo);

    return (uint32)statinfo.st_size;
}

MTime::TimeSpec get_file_time(const Str &file_path) {
    // @todo: need to find a better way to temp convert
    // to null term strings
    ASSERT(file_path[file_path.len - 1] == 0);

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
