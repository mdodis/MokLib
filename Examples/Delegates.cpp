#include "Delegates.h"
#include "FileSystem/FileSystem.h"

void do_something_deferred(Delegate<void> d, Delegate<float, int> d2);

struct Something {

    float provide_something(int i) {
        PRINTLN("Calculation variable provided...");
        return float(i + 2.5f);
    }

    void react_to_something() {
        PRINTLN("Calculation finished.");
    }
};

int main(int argc, char *argv[]) {

    Something something;

    Delegate<void> del;
    del.bind_raw(&something, &Something::react_to_something);

    Delegate<float, int> del2;
    del2.bind_raw(&something, &Something::provide_something);

    do_something_deferred(del, del2);
}

void do_something_deferred(Delegate<void> d, Delegate<float, int> d2) {
    PRINTLN("Calculation started...");

    int result = ceilf(d2.call(32));
    PRINTLN("Calculation result as integer is: " + result);

    d.call();
}

#include "Compile.inc"