#include "Elf.h"

namespace Elf {

Result<ReadContext, Str> open(Tape *tape, IAllocator &allocator) {
    char identifier[4];
    if (tape->read(identifier, sizeof(identifier)) != sizeof(identifier)) {
        return Err(LIT("Failed to read identifier"));
    }

    if (strncmp(identifier, Precursor, 4) != 0) {
        return Err(LIT("File is not an ELF file"));
    }

    tape->move(-4);

    auto *header = allocator.reserve<Header64>(1);

    if (tape->read(header, sizeof(*header)) != sizeof(*header)) {
        allocator.release(header);
        return Err(LIT("Failed to read header"));
    }

    ReadContext result = {
        tape,
        header
    };

    return Ok(result);
}

}