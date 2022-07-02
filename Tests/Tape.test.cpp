#include "Tests.h"
#include "Tape.h"
#include "Memory/Arena.h"

static bool read_tape_cmp(Tape *tape, Str expect, IAllocator &allocator) {
	umm ptr = allocator.reserve(expect.len);
	DEFER(allocator.release(ptr));
	ASSERT(ptr);

	tape->read(ptr, expect.len);
	return strncmp((char*)ptr, expect.data, expect.len) == 0;
}

TEST_CASE("Lib/Tape/Slice", "{'Hello', 'World'}, Sequential read, In-order read") {
	CREATE_SCOPED_ARENA(&System_Allocator, temp, 1024);

	TArr<Str, 2> strings = {
		LIT("Hello"),
		LIT("World"),
	};

	SliceTape tape(slice(strings));

	REQUIRE(read_tape_cmp(&tape, LIT("Hell"), temp), "Can read partial");
	REQUIRE(read_tape_cmp(&tape, LIT("oWorld"), temp), "Can read multiple");

	return MPASSED();
}

TEST_CASE("Lib/Tape/Slice", "Any string array, Sequential read, Can't overread") {
	CREATE_SCOPED_ARENA(&System_Allocator, temp, 1024);

	{
		TArr<Str, 1> strings = {
			LIT("Hello"),
			LIT("World"),
		};

		SliceTape tape(slice(strings));

		read_tape_cmp(&tape, LIT("HelloWorld"), temp);

		char c;
		REQUIRE(tape.read(&c, 1) == 0, "Can't read past array index");
	}

	return MPASSED();
}