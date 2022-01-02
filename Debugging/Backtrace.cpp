#include "Backtrace.h"
#include "Host.h"
#include "Memory/Base.h"
#include <stdlib.h>

#if OS_MSWINDOWS
#include "WinInc.h"

// DbgHelp.dll Structs
struct Win32SymbolInfo {
    ULONG   SizeOfStruct;
    ULONG   TypeIndex;
    ULONG64 Reserved[2];
    ULONG   Index;
    ULONG   Size;
    ULONG64 ModBase;
    ULONG   Flags;
    ULONG64 Value;
    ULONG64 Address;
    ULONG   Register;
    ULONG   Scope;
    ULONG   Tag;
    ULONG   NameLen;
    ULONG   MaxNameLen;
    CHAR    Name[1];
};

struct Win32ImageHlpLine {
    DWORD SizeOfStruct;
    PVOID Key;
    DWORD LineNumber;
    PCHAR FileName;
    DWORD Address;
};

// DbgHelp.dll Procedures
#define PROC_WIN32_SYMINIT(name) int name(HANDLE process, PCSTR user_search_path, BOOL invade_proc)
#define PROC_WIN32_SYMFROMADDR(name) int name(HANDLE process, DWORD64 addr, PDWORD64 disp, Win32SymbolInfo *sym)
#define PROC_WIN32_SYMGETLINEFROMADDR(name) int name(HANDLE process, DWORD64 addr, PDWORD disp, Win32ImageHlpLine *line)

typedef PROC_WIN32_SYMINIT(ProcWin32SymInit);
typedef PROC_WIN32_SYMFROMADDR(ProcWin32SymFromAddr);
typedef PROC_WIN32_SYMGETLINEFROMADDR(ProcWin32SymGetLineFromAddr);

static ProcWin32SymInit             *sym_initialize;
static ProcWin32SymFromAddr         *sym_from_addr;
static ProcWin32SymGetLineFromAddr  *sym_get_line_from_addr;
static void **stack;

void Backtrace::print(ILog *log) {
    stack = (void**)malloc(1024 * sizeof(*stack));

    // Load Dbghelp.dll
    HMODULE dbghelp = LoadLibraryA("Dbghelp.dll");
    {
        sym_initialize          = (ProcWin32SymInit*)            GetProcAddress(dbghelp, "SymInitialize");
        sym_from_addr           = (ProcWin32SymFromAddr*)        GetProcAddress(dbghelp, "SymFromAddr");
        sym_get_line_from_addr  = (ProcWin32SymGetLineFromAddr*) GetProcAddress(dbghelp, "SymGetLineFromAddr64");
    }

    HANDLE process = GetCurrentProcess();

    sym_initialize(process, 0, TRUE);
    WORD num_frames = RtlCaptureStackBackTrace(0, 1000, stack, 0);

    Win32SymbolInfo *symbol = (Win32SymbolInfo*)malloc(sizeof(Win32SymbolInfo) + 1024 - 1);
    symbol->MaxNameLen = 1024;
    symbol->SizeOfStruct = sizeof(*symbol);

    Win32ImageHlpLine *line = (Win32ImageHlpLine*)malloc(sizeof(Win32ImageHlpLine));
    line->SizeOfStruct = sizeof(*line);

    DWORD64 disp = 0;
    DWORD disp2 = 0;
    for (WORD frame_index = 0; frame_index < num_frames; ++frame_index) {
        int hr = sym_from_addr(process, (DWORD64)stack[frame_index], &disp, symbol);
        if (!hr) {
            break;
        }

        hr = sym_get_line_from_addr(process, (DWORD64)stack[frame_index], &disp2, line);
        if (!hr) {
            break;
        }

        StringBuilder result = StringBuilder(get_system_allocator())
            .add(LIT("At "))
            .add(Str(line->FileName))
            .add(LIT(" in "))
            .add(Str(((char*)&symbol->Name), symbol->NameLen))
            .add(LIT("("))
            .add((int)line->LineNumber)
            .add(LIT(")"));

        log->print(result);
    }
}

#endif