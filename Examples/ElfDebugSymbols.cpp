#define MOK_LIB_STATIC 1
#include "Extras/Elf.h"
#include "FileSystem/Extras.h"

int main(int argc, char *argv[]) {
    Str file(argv[1]);

    auto tape = open_read_tape(file);
    if (!IS_VALID_FILE(tape.file)) {
        return -1;
    }
    Elf::ReadContext ctx = Elf::open(&tape).unwrap();

    print(LIT("Section table offset = $\nSection Table Entries = $\nclass = $\n"),
        (u64)ctx.header->shoff,
        (u32)ctx.header->shnum,
        (u32)ctx.header->bitclass);

    return 0;
}


#include "Compile.inc"
