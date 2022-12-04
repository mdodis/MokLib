#include "Elf.h"

namespace Elf {

Result<ReadContext, Str> open(Tape *tape, IAllocator &allocator) {
    Header64 *header;
    {
        ParseTape pt(tape);
        DEFER(pt.restore());
        char identifier[4];
        if (pt.read(identifier, sizeof(identifier)) != sizeof(identifier)) {
            return Err(LIT("Failed to read identifier"));
        }

        if (strncmp(identifier, Precursor, 4) != 0) {
            return Err(LIT("File is not an ELF file"));
        }

        pt.move(-4);

        header = allocator.reserve<Header64>(1);

        if (pt.read(header, sizeof(*header)) != sizeof(*header)) {
            allocator.release(header);
            return Err(LIT("Failed to read header"));
        }
    }

    ReadContext result = {
        tape,
        header
    };

    // Find the string table
    u64 string_table_addr = 0;
    u64 string_table_size = 0;
    auto find_string_table = ReadContext::EnumerateSectionsDelegate::create_lambda(
    [&] (SecHeader64 *hdr, u16 index) {
        if (index != header->shstrndx)
            return ReadContext::DiscardHeader;

        string_table_addr = hdr->offset;
        string_table_size = hdr->size;
        return ReadContext::DiscardHeader;
    });

    if (!result.enumerate_sections(find_string_table, allocator)) {
        return Err(LIT("Enumerate sections failed. File is probably malformed"));
    }

    {
        ParseTape pt(tape);
        DEFER(pt.restore());

        pt.move((i64)string_table_addr);
        result.string_table.buffer = allocator.reserve(string_table_size);
        result.string_table.size = string_table_size;
        pt.read_raw(result.string_table);

        print(LIT("String Table: {}({})\n"), string_table_addr, string_table_size);

        for (u64 i = 0; i < string_table_size; ++i) {
            char b = ((char*)result.string_table.buffer)[i];
            if (b == 0) {
                print(LIT(" "));

            } else {
                print(LIT("{}"), b);
            }
        }
        print(LIT("\n"));
    }


    return Ok(result);
}

bool ReadContext::enumerate_sections(
    EnumerateSectionsDelegate &delegate,
    IAllocator &allocator)
{
    ParseTape pt(tape);
    DEFER(pt.restore());

    SecHeader64 *section_hdr = allocator.reserve<SecHeader64>(1);

    pt.move((i64)header->shoff);

    for (u16 i = 0; i < header->shnum; ++i) {

        if (!pt.read_struct<SecHeader64>(section_hdr)) {
            return false;
        }


        auto result = delegate.call(section_hdr, i);
        if (result == ReadContext::KeepHeader) {
            // @todo: allocate new memory for next section header
        }
    }

    return true;
}

Str ReadContext::find_string(u16 index) {

    char *str = (char*)string_table.buffer;
    str = str + index;
    return Str(str);
}

SecHeader64 *ReadContext::get_section_header(Str name, IAllocator &allocator) {
    SecHeader64 *result = 0;
    auto del = EnumerateSectionsDelegate::create_lambda(
    [&] (SecHeader64* hdr, u16 index) {
        Str hdr_name = find_string(hdr->name);
        print(LIT("Section: {} ({})\n"), hdr_name, hdr->name);
        if (hdr_name == name) {
            result = hdr;
            return EnumerateSectionsAnswer::KeepHeader;
        } else {
            return EnumerateSectionsAnswer::DiscardHeader;
        }
    });

    if (!enumerate_sections(del, allocator)) {
        return 0;
    }

    return result;
}


}