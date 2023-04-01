#include "Parsing.h"

#include "Containers/Array.h"
#include "Math/Base.h"

u64 parse_cid(const Str& s, u64 i, Str& out)
{
    const char* begin = &s[i++];

    while (i < s.len && is_valid_cid(s[i])) {
        i++;
    }

    i = min(i, s.len - 1);

    const char* end = &s[i];

    out = Str(begin, u64(end - begin));
    return i;
}

Str parse_cid(ReadTape* tape, IAllocator& alloc)
{
    TArray<char> result(&alloc);

    char c;
    while ((c = tape->read_char()) != EOF) {
        if (is_valid_cid(c)) {
            result.add(c);
        } else {
            break;
        }
    }

    tape->seek(-1);

    if (result.size == 0) {
        return Str::NullStr;
    } else {
        return Str(result.data, result.size);
    }
}

Str parse_string(ReadTape* tape, IAllocator& alloc, ProcCharClassIs* predicate)
{
    int32 i = 0;
    char  c;

    TArray<char> result(&alloc);
    while ((c = tape->read_char()) != EOF) {
        if (predicate(c)) {
            result.add(c);
        } else {
            break;
        }
    }

    if (c != EOF) tape->seek(-1);
    return Str(result.data, result.size);
}

bool parse_escaped_string(ReadTape* tape, Str& result, IAllocator& allocator)
{
    ParseReadTape pt(*tape);
    TArray<char>  array(&allocator);

    char c = pt.read_char();

    if (c != '\"') {
        goto PARSE_FAIL;
    }
    c = pt.read_char();

    while ((c != '\"') && c != EOF) {
        if (c == '\\') {
            char escape = pt.read_char();

            switch (escape) {
                case '\\':
                    array.add('\\');
                    break;
                case '\"':
                    array.add('\"');
                    break;
                case 't':
                    array.add('\t');
                    break;
                case 'r':
                    array.add('\r');
                    break;
                case 'f':
                    array.add('\f');
                    break;
                case 'n':
                    array.add('\n');
                    break;
                case 'b':
                    array.add('\b');
                    break;

                default: {
                    array.add(c);
                    array.add(escape);
                } break;
            }

        } else {
            array.add(c);
        }

        c = pt.read_char();
    }

    if (c == EOF) {
        goto PARSE_FAIL;
    }

    result = Str((char*)array.data, array.size);
    return true;

PARSE_FAIL:
    pt.restore();
    return false;
}