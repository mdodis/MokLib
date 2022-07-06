#include "Test/Test.h"
#include "Tape.h"
#include "Memory/Arena.h"
#include <string.h>

static bool read_tape_cmp(Tape *tape, Str expect, IAllocator &allocator) {
	umm ptr = allocator.reserve(expect.len);
	DEFER(allocator.release(ptr));
	ASSERT(ptr);

	if (tape->read(ptr, expect.len) != expect.len) {
		return false;
	}

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
		TArr<Str, 2> strings = {
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

TEST_CASE(
	"Lib/Tape/Slice",
	"{'Hello', 'World'}, "
	"Read string and move cursor one back and read string, "
	"Reads correctly")
{
	CREATE_SCOPED_ARENA(&System_Allocator, temp, 1024);

	// Move Forwards
	{
		TArr<Str, 2> strings = {
			LIT("Hello"),
			LIT("World"),
		};

		SliceTape tape(slice(strings));

		tape.move(+5);
		REQUIRE(read_tape_cmp(&tape, LIT("World"), temp), "");
	}

	// Move Backwards
	{
		TArr<Str, 2> strings = {
			LIT("Hello"),
			LIT("World"),
		};

		SliceTape tape(slice(strings));
		tape.move(10);
		tape.move(-5);
		REQUIRE(read_tape_cmp(&tape, LIT("World"), temp), "");
	}

	// Move Backwards partial
	{
		TArr<Str, 2> strings = {
			LIT("Hello"),
			LIT("World"),
		};

		SliceTape tape(slice(strings));

		tape.move(10);
		tape.move(-6);

		REQUIRE(read_tape_cmp(&tape, LIT("oWorld"), temp), "");

	}

	return MPASSED();
}