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
    auto find_string_table =
        ReadContext::EnumerateSectionsDelegate::create_lambda(
        [&](SecHeader64 *hdr, u16 index){
            if (index != header->shstrndx)
                return ReadContext::DiscardHeader;

            string_table_addr = hdr->offset;
            string_table_size = hdr->size;
            return ReadContext::DiscardHeader;
        });

    if (!result.enumerate_sections(find_string_table, allocator)) {
        return Err(LIT("Enumerate sections failed. File is probably malformed"));
    }

    tape->move((i64)string_table_addr);
    result.string_table.buffer = allocator.reserve(string_table_size);
    result.string_table.size = string_table_size;
    tape->read_raw(result.string_table);

    print(LIT("String Table: $($)\n"), string_table_addr, string_table_size);

    for (u64 i = 0; i < string_table_size; ++i) {
        char b = ((char*)result.string_table.buffer)[i];
        if (b == 0) {
            print(LIT(" "));

        } else {
            print(LIT("$"), b);
        }
    }
    print(LIT("\n"));

    tape->move(-string_table_addr);

    return Ok(result);
}

bool ReadContext::enumerate_sections(
    EnumerateSectionsDelegate &delegate,
    IAllocator &allocator)
{
    ParseTape pt(tape);
    DEFER(pt.restore());
    print(LIT("shstrndx :: $\n"), (u32)header->shstrndx);
    print(LIT("shnum :: $\n"), (u32)header->shnum);
    print(LIT("shoff :: $\n"), (u64)header->shoff);

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
    if (index > header->shstrndx) {
        return Str::NullStr;
    }

    // Start ptr (skip first null byte)
    auto ptr = (char*)string_table.buffer;
    for (u16 i = 0; i != index; ++i) {
        ptr++;
        while (*ptr) {
            ptr++;
        }
    }
    return Str(ptr);
}


}