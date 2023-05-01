#include "Test/Test.h"
#include "FileSystem/FileSystem.h"

BENCHMARK("Lib/Tape/BufferedFileTape", "IO Throughput", 1, 1, 6) {
    const u64 num_bytes = KILOBYTES(result.iteration) * 64;
    result.units_processed = num_bytes;

    // auto tape = open_write_tape("./tmp.txt");
    auto tape = BufferedWriteTape<true>(open_file_write("./tmp.txt"));

    for (u64 i = 0; i < num_bytes; ++i) {
        char c = 'H';
        tape.write(&c, 1);
    }
}