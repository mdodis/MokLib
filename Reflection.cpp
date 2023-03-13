#include "Reflection.h"

IDescriptor* IDescriptor::find_descriptor(umm self, Str name)
{
    auto subs = subdescriptors(self);

    for (IDescriptor* sub : subs) {
        if (sub->name == name) {
            return sub;
        }
    }

    return 0;
}
