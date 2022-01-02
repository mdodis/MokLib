#include "Log.h"
#include "FileSystem/FileSystem.h"
#include "Str.h"

void ILog::print(TList<Str> &what, bool newline) {

    FileHandle output_file = {0};

    bool print_to_file = file.len != 0;

    if (print_to_file) {
        output_file = open_file(
            file,
            FileMode::ShareRead     |
            FileMode::Append        |
            FileMode::OpenAlways);
        ASSERT(IS_VALID_FILE(output_file));
    }

    LIST_FOR_EACH(&what, iter) {
        add_output(iter->data);

        if (print_to_file) {
            write_file(output_file, iter->data.data, iter->data.len, 0);
        }
    }

    if (newline) {
        add_output(LIT("\n"));
        if (print_to_file) {
            Str s = LIT("\n");
            write_file(output_file, s.data, s.len, 0);
        }
    }

    if (print_to_file) {
        close_file(output_file);
    }
}
