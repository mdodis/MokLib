#include "Reflection.h"

IDescriptor *IDescriptor::find_descriptor(Str name) {
    auto subs = subdescriptors();

    for (IDescriptor *sub : subs) {
        if (sub->name == name) {
            return sub;
        }
    }

    return 0;
}
