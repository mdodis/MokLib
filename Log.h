#pragma once
#include "Containers/List.h"
#include "FileSystem/FileSystem.h"
#include "Str.h"
#include "StringBuilder.h"

struct ILog {

    Str file;

    void set_file(const Str &path) {
        file = path;
    }

    void print(TList<Str> &what, bool newline = true);
    _inline void print(StringBuilder &what, bool newline = true) {
        TList<Str> list = what.to_list();
        print(list, newline);
    }

    virtual void add_output(const Str &what) = 0;
};