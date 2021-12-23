#include "DirectoryIterator.h"

#if OS_WINDOWS
#include "WinInc.h"

static wchar_t Cached_Directory_Entry[1024];

DirectoryIterator open_dir(Str filename) {
    DirectoryIterator result;
    result.directory = filename;


    return result;
}

static char Filename_Cache[260];

bool DirectoryIterator::next_file(FileData *result) {
    WIN32_FIND_DATAW dataw;
    bool was_found = false;

    if (!has_first_file) {
        int lenw = MultiByteToWideChar(
            CP_UTF8,
            MB_ERR_INVALID_CHARS,
            (const char*)directory.data,
            directory.len,
            Cached_Directory_Entry,
            ARRAY_COUNT(Cached_Directory_Entry));
        Cached_Directory_Entry[lenw++] = L'*';
        Cached_Directory_Entry[lenw] = 0;

        handle = FindFirstFileW(Cached_Directory_Entry, &dataw);

        was_found = handle != INVALID_HANDLE_VALUE;
        has_first_file = true;
    } else {
        was_found = FindNextFileW(handle, &dataw);
    }

    if (!was_found)
        return false;

    int num_bytes = WideCharToMultiByte(
        CP_UTF8,
        WC_ERR_INVALID_CHARS,
        dataw.cFileName,
        -1,
        Filename_Cache,
        sizeof(Filename_Cache),
        0, 0);

    result->filename = Str((char*)Filename_Cache, num_bytes - 1, true);
    return true;
}

#elif OS_LINUX
#include <sys/types.h>
#include <dirent.h>

DirectoryIterator open_dir(Str filename) {
    ASSERT(filename.has_null_term);

    DIR *dir_handle = opendir((char*)filename.data);

    return DirectoryIterator {
        filename,
        dir_handle
    };
}

bool DirectoryIterator::next_file(FileData *result) {
    DIR *dir_handle = (DIR*)handle;

    struct dirent *read_result = readdir(dir_handle);
    if (!read_result) {
        return false;
    }

    result->filename = Str(read_result->d_name);

    return true;
}

#else

#error "Directory Iterator not supported for this platform!"

#endif