#include "Dll.h"
#include "FileSystem/FileSystem.h"
#include "Debugging/Assertions.h"

#if OS_MSWINDOWS
#include "WinInc.h"
#include "Compat/Win32Internal.inc"
#include <psapi.h>

TEnum<IOError> load_dll(Str filename, Dll &result) {
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
        mod.handle = (Win32::HModule)library_handle;
        result = mod;
        return IOError::None;

    } else {
        return to_io_error(GetLastError());
    }
}

void unload_dll(Dll &dll) {
    BOOL result = FreeLibrary((HMODULE)dll.handle);
    ASSERT(result);
}

void *Dll::get_proc_address(const char *name) {
    return (void*)GetProcAddress((HMODULE)handle, name);
}

Str Dll::get_filename(IAllocator &alloc, Win32::Handle _process) {

    Win32::Handle process = _process;

    if (process == 0) {
        process = (Win32::Handle)GetCurrentProcess();
    }

    umm ptr = alloc.reserve(MAX_PATH);

    DWORD count = GetModuleBaseNameA(
        (HANDLE)process,
        (HMODULE)handle,
        (LPSTR)ptr,
        MAX_PATH);

    if (count == 0)
        return Str::NullStr;

    return Str((char*)ptr, count);
}

bool get_current_process_dlls(IAllocator& alloc, TArray<Dll>& result) {
    HANDLE hprocess = GetCurrentProcess();
    DWORD count;
    if (!EnumProcessModules(hprocess, 0, 0, &count)) {
        return false;
    }

    result = TArray<Dll>(&alloc);
    result.init(count / sizeof(Dll));
    if (EnumProcessModules(hprocess, (HMODULE*)result.data, count, &count)) {
        result.size = count / sizeof(Dll);
        return true;
    }

    return false;
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