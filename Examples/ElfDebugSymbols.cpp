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

    return 0;
}


#include "Compile.inc"
