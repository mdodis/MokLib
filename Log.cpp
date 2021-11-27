#include "Log.h"

void ILog::print(TList<Str> &what, bool newline) {

    LIST_FOR_EACH(&what, iter) {
        add_output(iter->data);
    }

    if (newline)
        add_output(STATIC_STR("\n"));
}
