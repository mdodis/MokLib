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
    bool has_null_term = wstring[string_size - 1] == 0;

    if (has_null_term)
        string_size -= 1;

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

static _inline u32 multibyte_to_wstr(
    const Str &string,
    wchar_t *destination,
    u32 destination_len)
{

    return (u32) MultiByteToWideChar(
        CP_UTF8,
        MB_ERR_INVALID_CHARS,
        string.data,
        string.len,
        destination,
        destination_len);
}

/**
 * Convert a utf8 string to a wide string
 * @param  string  UTF-8 string
 * @param  out_len The length, not including the null terminator of the wide str
 * @param  alloc   Allocator
 * @return         Pointer to the wide string
 */
static _inline wchar_t *multibyte_to_wstr(
    const Str &string,
    u32 *out_len,
    IAllocator &alloc)
{
    u32 len;
    len = MultiByteToWideChar(
        CP_UTF8,
        MB_ERR_INVALID_CHARS,
        string.data,
        string.len,
        0,
        0);

    // Include null-term because a lot of bad APIs need it
    wchar_t *wstr = alloc.reserve<wchar_t>(len + 1);

    MultiByteToWideChar(
        CP_UTF8,
        MB_ERR_INVALID_CHARS,
        string.data,
        string.len,
        wstr,
        len);

    if (out_len)
        *out_len = len;

    wstr[len] = 0;

    return wstr;
}