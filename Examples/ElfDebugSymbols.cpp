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

    print(LIT("Off {}\n"), tape.current_offset);
    auto *debug_line_sec = ctx.get_section_header(LIT(".debug_line"));
    if (!debug_line_sec) {
        print(LIT("Failed to find debug_line section\n"));
        return -1;
    }

    return 0;
}


#include "Compile.inc"
