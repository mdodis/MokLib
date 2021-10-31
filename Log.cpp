#include "Log.h"

void ILog::print(TList<Str> &what) {

    LIST_FOR_EACH(&what, iter) {
        add_output(iter->data);
    }

    add_output(STATIC_STR("\n"));
}
