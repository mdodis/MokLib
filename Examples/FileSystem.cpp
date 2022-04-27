#define MOK_LIB_STATIC 1
#include "Log.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/DirectoryIterator.h"

int main(int argc, char *argv[]) {

    if (argc < 2) {
        return -1;
    }

    Str directory_to_open = Str(argv[1]);

    DirectoryIterator iter = open_dir(directory_to_open);

    FileData file_data;
    while (iter.next_file(&file_data)) {
        PRINTLN("Found file: " + file_data.filename);
    }
}

#include "Compile.inc"