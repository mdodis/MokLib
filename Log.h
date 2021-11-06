#pragma once
#include "Containers/List.h"
#include "Str.h"
#include "StringBuilder.h"

struct ILog {

    void print(TList<Str> &what);
    _inline void print(StringBuilder &what) {
        print(what.to_list());
    }

    virtual void add_output(const Str &what) = 0;
};