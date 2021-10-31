#pragma once
#include "Containers/List.h"
#include "Str.h"
#include "StringBuilder.h"

#define ENG_LOG(what) do {\
        CREATE_LOCAL_ARENA(_arena, 1024); \
        Eng->Log->print(StringBuilder(_arena.to_alloc()) + what); \
    } while(0)

struct ILog {

    void print(TList<Str> &what);
    _inline void print(StringBuilder &what) {
        print(what.to_list());
    }

    virtual void add_output(const Str &what) = 0;
};