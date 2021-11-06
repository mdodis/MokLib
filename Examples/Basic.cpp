#include "Log.h"

void example_str(void);


int main(int argc, char const *argv[])
{
    Str a = STATIC_STR("77Mitch Taylor77");
    a = a.chop_middle(2, 14);

    PRINT("Hello, " + a + "!");

    example_str();

    return 0;
}

void example_str(void) {
    // Strings are modelled as immutable buffer + length containers and nothing more
    // operations on strings

    // STATIC_STR is used for string literals, to avoid a strlen call.
    Str a = STATIC_STR("1337hay hay hay hay hay7331");

    // Finding stuff!
    int32 last_hay = a.last_of(STATIC_STR("hay"));
    int32 first_hay = a.first_of(STATIC_STR("hay"));

    PRINT("Last hay:  " + last_hay);
    PRINT("First hay: " + first_hay);
}


#include "Compile.inc"