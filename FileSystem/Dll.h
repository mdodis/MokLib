#pragma once
#include "Config.h"
#include "Base.h"
#include "Str.h"
#include "Time/Time.h"
#include "Containers/Array.h"
#include "Error.h"
#define NAMED_PROC(Name, ProcType) TNamedProc(Name, (ProcType*)0)

template <typename ProcType>
struct TNamedProc {
    const char *name;
    ProcType *native;

    TNamedProc()
        : name(0)
        {}

    TNamedProc(const char *name)
        : name(name)
        {}

    ProcType *operator*() const { return native; }
};

#if OS_MSWINDOWS
#include "Compat/Win32.h"

#endif

struct MOKLIB_API Dll {
#if OS_MSWINDOWS
    Win32::HModule handle;
#elif OS_LINUX
    void *handle;
#else
    #error "No Dll support for this platform!"
#endif

    void *get_proc_address(const char *name);

    template <typename TProc>
    _inline bool get_proc_address(TNamedProc<TProc> &proc) {
        void *result = get_proc_address(proc.name);
        if (result) {
            proc.native = (TProc*)result;
        }

        return result != 0;
    }

#if OS_MSWINDOWS
    Str get_filename(IAllocator &alloc, Win32::Handle _process = 0);
#else
    #warning "No Dll support for this platform!"
#endif
};

#if OS_MSWINDOWS
static_assert(sizeof(Win32::HModule) == sizeof(Dll));
#endif

/** Loads a Dll by file path */
MOKLIB_API TEnum<IOError> load_dll(Str filename, Dll &result);

/**
 * Unloads a Dll. Note that this will invalidate any objects
 * that are used through the Dll.
 */
MOKLIB_API void unload_dll(Dll &dll);

/** Enumerates all the loaded modules inside this process */
MOKLIB_API bool get_current_process_dlls(
    IAllocator &alloc,
    TArray<Dll> &result);
