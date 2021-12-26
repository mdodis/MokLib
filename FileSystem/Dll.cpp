#include "Dll.h"
#include "FileSystem/FileSystem.h"

#if OS_WINDOWS
#include "WinInc.h"

Dll load_dll(Str filename) {
    // Convert filename to wide string
    wchar_t filenamew[1024];
    int32 num_written = MultiByteToWideChar(
        CP_UTF8,
        MB_ERR_INVALID_CHARS,
        (char*)filename.data,
        (u32)filename.len,
        filenamew,
        ARRAY_COUNT(filenamew));
    ASSERT(num_written == filename.len);
    filenamew[num_written] = 0;

    HMODULE library_handle = LoadLibraryW(filenamew);

    if (library_handle != 0) {

        Dll mod;
        mod.handle = (HModule)library_handle;
        return mod;

    } else {
        return Dll {0};
    }
}

void unload_dll(Dll &dll) {
    BOOL result = FreeLibrary((HMODULE)dll.handle);
    ASSERT(result);
}

void *Dll::get_proc_address(const char *name) {
    return (void*)GetProcAddress((HMODULE)handle, name);
}

#elif OS_LINUX
#include <dlfcn.h>
#include <string.h>

Dll load_dll(Str filename) {

    static char filename_nullterm[1024];
    const char *filepath = (const char *)filename.data;

    if (filename[filename.len - 1] != 0) {
        memcpy(filename_nullterm, filename.data, filename.len);
        filename_nullterm[filename.len] = 0;
        filepath = filename_nullterm;
    }

    void *handle = dlopen(filepath, RTLD_LAZY);

    if (!handle) {
        return Dll {0};
    }

    Dll result;
    result.handle = handle;

    return result;
}

void unload_dll(Dll &dll) {
    dlclose(dll.handle);
}

void *Dll::get_proc_address(const char *name) {
    return dlsym(handle, name);
}

#else
#error "No dll support for given platform!"
#endif