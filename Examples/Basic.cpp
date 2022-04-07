#define MOK_LIB_STATIC 1
#include "Containers/Array.h"
#include "Containers/Map.h"
#include "Log.h"
#include "Memory/Base.h"
#include "Str.h"
#include "Containers/Bitset.h"

void example_str(void);         // Strings
void example_arrays(void);      // Arrays 1
void example_arrays2(void);     // Arrays 2
void example_maps(void);        // Maps
void example_bitsets(void);     // Bitsets

int main(int argc, char const *argv[])
{
    Str a = LIT("77Mitch Taylor77");
    a = a.chop_middle(2, 14);

    PRINTLN("Hello, " + a + "!");

    example_str();
    example_arrays();
    example_maps();
    example_bitsets();

    return 0;
}

// Example - Strings
void example_str(void) {
    // Strings are modelled as immutable buffer + length containers
    // and nothing more

    // STATIC_STR is used for string literals, to avoid a strlen call.
    Str a = LIT("1337hay hay hay hay hay7331");

    // Finding stuff!
    int32 last_hay = a.last_of(LIT("hay"));
    int32 first_hay = a.first_of(LIT("hay"));

    PRINTLN("Last hay:  " + last_hay);
    PRINTLN("First hay: " + first_hay);
}

// Example - Arrays 1
void example_arrays(void) {

    // Everything that would normally deal with allocations requires a ref to
    // an allocator.
    // get_system_allocator() is a very thin wrapper around stdlib's malloc
    TArray<int> integers = {get_system_allocator(), {1, 2, 3, 4}};

    integers.add(5);

    int *last_int = integers.add();
    *last_int = 6;

    int accumulator = 0;
    for (int &i : integers) {
        accumulator += i;
    }

    PRINTLN("Sum: " + accumulator);

    // Note that most containers will _not_ free memory at the end of their
    // scope (explicitly). This is because you are not expected to use the
    // system allocator wherever! Instead, use it as the base for other
    // allocators like Arenas and Pools.
    integers.release();

    example_arrays2();
}

// Example - Arrays 2
// For very small operations, or for declaring arrays that store data inline
// You can use TInlineArray<T, Size>:
struct ExampleArray2Struct {
    TInlineArray<int, 16> inline_array;
};

void example_arrays2(void) {

    // Capacity is handled up front
    ExampleArray2Struct the_struct;
    PRINTLN("Inline Array Capacity: " + the_struct.inline_array.capacity);

    for (int32 i = 0; i < 16; ++i) {
        the_struct.inline_array.add(i);
    }

    // We expect this to fail
    int32 last_index = the_struct.inline_array.add(16);
    ASSERT(last_index == -1);

    PRINTLN("Elements = {");
    for (const int32 &elem : the_struct.inline_array) {
        PRINTLN("  " + elem + ",");
    }
    PRINTLN("}");

    PRINTLN("In reverse order:");

    for (const i32 &elem : reverse(the_struct.inline_array)) {
        PRINTLN("  " + elem + ",");
    }
}

// Example - Maps
void example_maps(void) {
    TMap<Str, int32> map(*get_system_allocator(), 5);

    map.add(LIT("Mitch"),    45);
    map.add(LIT("Kenny"),    12);
    map.add(LIT("Haley"),    25);
    map.add(LIT("Michael"),  23);
    map.add(LIT("Achilles"), 23);

    ASSERT(map[LIT("Michael")] == 23);
}

void example_bitsets(void) {
    Bitset<32> a = 32;

    PRINTLN("Bitset: " + a.to_integral());
}

#include "Compile.inc"