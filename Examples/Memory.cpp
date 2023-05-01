#define MOK_LIB_STATIC 1
#include "Base.h"
#include "Log.h"
#include "Memory/Arena.h"
#include "Memory/Base.h"

void example_arena(void);

int main(int argc, char* argv[]) { example_arena(); }

void example_arena(void)
{
    CREATE_SCOPED_ARENA(System_Allocator, arena, Arena::Default_Block_Size);

    umm allocations[5];

    allocations[0] = arena.push(MEGABYTES(1) - 1024);
    allocations[1] = arena.push(1024);

    allocations[0][0]                   = 1;
    allocations[0][MEGABYTES(1) - 1023] = 2;

    allocations[1][0]    = 3;
    allocations[1][1023] = 4;

    allocations[0] =
        arena.resize(allocations[0], MEGABYTES(1) - 1024, MEGABYTES(1));

    ASSERT(allocations[0][0] == 1);

    PRINT("Hey!");
}

#include "Compile.inc"