#include "../WinInc.h"

/**
 * Convert wide string to UTF8 string
 * @param  wstring     Pointer to wide string
 * @param  string_size Size of wide string (not including \0)
 * @param  alloc       Allocator
 * @return             UTF8 string
 */
static inline Str wstr_to_multibyte(
    wchar_t *wstring,
    DWORD string_size,
    IAllocator &alloc)
{
    i32 num_bytes = WideCharToMultiByte(
        CP_UTF8,
        WC_ERR_INVALID_CHARS,
        wstring,
        string_size,
        0, 0, 0, 0);

    umm ptr = alloc.reserve(num_bytes);
    if (!ptr)
        return Str::NullStr;

    i32 result = WideCharToMultiByte(
        CP_UTF8,
        WC_ERR_INVALID_CHARS,
        wstring,
        string_size,
        (LPSTR)ptr,
        num_bytes, 0, 0);

    return Str((char*)ptr, (u64)num_bytes);
}