#include "Memory/Arena.h"

#include "Test/Test.h"

TEST_CASE("Lib/Memory/Arena", "Ensure dynamic arena doesn't crash")
{
    Arena<ArenaMode::Dynamic> arena(System_Allocator, 10);
    arena.init();

    volatile u8* p = (u8*)arena.push(10);
    for (u8 i = 0; i < 10; ++i) {
        p[i] = (u8)i;
    }

    arena.push(10);

    volatile u8* x = ((u8*)arena.header()->prev) +
                     sizeof(Arena<ArenaMode::Dynamic>::BlockHeader);

    for (u8 i = 0; i < 10; ++i) {
        REQUIRE(x[i] == i, "");
    }

    return MPASSED();
}

TEST_CASE("Lib/Memory/Arena", "Ensure dynamic arena resets okay") {
    NullAllocator             null;
    SystemAllocator           system;

    Arena<ArenaMode::Dynamic> arena(system, 10);
    arena.init();
 
    arena.push(10);
    arena.push(10);

    arena.reset();

    arena.base = null;
    arena.push(10);
    arena.push(10);

    return MPASSED();
}