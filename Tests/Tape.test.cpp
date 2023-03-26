#include "Tape.h"

#include <string.h>

#include "AllocTape.h"
#include "FileSystem/FileSystem.h"
#include "Memory/Arena.h"
#include "SliceTape.h"
#include "Test/Test.h"

static bool read_tape_cmp(Tape* tape, Str expect, IAllocator& allocator)
{
    umm ptr = allocator.reserve(expect.len);
    DEFER(allocator.release(ptr));
    ASSERT(ptr);

    if (tape->read(ptr, expect.len) != expect.len) {
        return false;
    }

    return strncmp((char*)ptr, expect.data, expect.len) == 0;
}

TEST_CASE(
    "Lib/Tape/Slice", "{'Hello', 'World'}, Sequential read, In-order read")
{
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

TEST_CASE("Lib/Tape/Slice", "Any string array, Sequential read, Can't overread")
{
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

TEST_CASE(
    "Lib/Tape/BufferedFileTape",
    "{'AHello world'}, write to file, writes completely")
{
    Str path = LIT("./test.txt");

    {
        TBufferedFileTape<true> tape(open_file_write(path), 5);
        tape.write_str(LIT("A"));
        tape.write_str(LIT("Hello world"));
    }

    auto check = open_read_tape(path);

    REQUIRE(read_tape_cmp(&check, LIT("AHello world"), System_Allocator), "");

    return MPASSED();
}

TEST_CASE(
    "Lib/Tape/RawReadTape",
    "{0123456789}, consecutive out of range reads, reads correctly")
{
    CREATE_SCOPED_ARENA(&System_Allocator, temp, KILOBYTES(1))

    Str s   = LIT("0123456789");
    Raw raw = s;

    RawReadTape tape(raw);

    Str s1 = tape.read_str(temp, 5);

    REQUIRE(s1 == LIT("01234"), "");

    Str s2 = tape.read_str(temp, 10);
    REQUIRE(s2.len == 5, "");
    REQUIRE(s2 == LIT("56789"), "");

    return MPASSED();
}

TEST_CASE(
    "Lib/Tape/RawReadTape",
    "{0123456789}, rd 9, seek -20, rd 1, reads correctly")
{
    CREATE_SCOPED_ARENA(&System_Allocator, temp, KILOBYTES(1))

    Str         s   = LIT("0123456789");
    Raw         raw = s;
    RawReadTape tape(raw);

    Str s1 = tape.read_str(temp, 9);
    REQUIRE(s1 == LIT("012345678"), "");

    REQUIRE(tape.try_seek(-20) == -9, "");

    Str s2 = tape.read_str(temp, 1);
    REQUIRE(s2 == LIT("0"), "");

    return MPASSED();
}

TEST_CASE(
    "Lib/Tape/RawReadTape",
    "{0123456789}, seek 10, read backwards works correctly")
{
    Str         s   = LIT("0123456789");
    Raw         raw = s;
    RawReadTape tape(raw);

    REQUIRE(tape.try_seek(10) == 10, "");

    int i = 9;
    while (i != -1) {
        REQUIRE(tape.try_seek(-1) == -1, "");
        char c = tape.read_char();
        REQUIRE(tape.try_seek(-1) == -1, "");

        REQUIRE(c == s[i], "");
        i--;
    }

    return MPASSED();
}

TEST_CASE(
    "Lib/Tape/ParseReadTape", "{0123456789}, read 5 and restore 3 times works")
{
    RawReadTape inner((Raw)LIT("0123456789"));

    char buf[5];

    for (int i = 0; i < 3; ++i) {
        ParseReadTape outer(inner);
        REQUIRE(outer.read(buf, 5) == 5, "");

        Str c1(buf, 5);
        Str c2 = LIT("01234");

        REQUIRE(c1 == c2, "");

        outer.restore();
    }

    return MPASSED();
}

TEST_CASE(
    "Lib/Tape/RawWriteTape", "{u8[10]}, write 0123456789 is correctly written")
{
    char         buffer[10];
    RawWriteTape tape(Raw(buffer, 10));

    Str s = LIT("0123456789");

    tape.write_str(s);

    Str comp(tape.buffer);

    REQUIRE(comp == s, "");

    return MPASSED();
}

TEST_CASE("Lib/Tape/MeasureWriteTape", "{num_written properly synchronized}")
{
    MeasureWriteTape tape;

    for (u8 i = 0; i < 10; ++i) {
        REQUIRE(tape.write(&i, 1) == 1, "");
    }

    REQUIRE(tape.num_written == 10, "");
    REQUIRE(tape.current_offset == 10, "");

    return MPASSED();
}

TEST_CASE("Lib/Tape/MeasureWriteTape", "{current_offset properly synchronized}")
{
    MeasureWriteTape tape;
    REQUIRE(!tape.seek(-10), "");
    REQUIRE(tape.seek(10), "");

    REQUIRE(tape.write_str(LIT("hello")), "");

    REQUIRE(tape.num_written == 5, "");
    REQUIRE(tape.current_offset == 15, "");

    return MPASSED();
}

TEST_CASE("Lib/FileSystem/StreamWriteTape", "Test stdout")
{
    StreamWriteTape s = get_write_stream(Console::Output);

    s.write_str(LIT("Some string\n"));

    return MPASSED();
}

TEST_CASE("Lib/FileSystem/FileTape", "Test file tape")
{
    {
        FileWriteTape<true> ft(open_file_write(LIT("test.file.test")));
        REQUIRE(ft.write_str(LIT("Hello, world!")), "");
    }

    {
        CREATE_SCOPED_ARENA(&System_Allocator, temp, KILOBYTES(1));

        FileReadTape<true> ft(open_file_read(LIT("test.file.test")));

        Str s = ft.read_str(temp, 13);
        REQUIRE(s != Str::NullStr, "");

        REQUIRE(s == LIT("Hello, world!"), "");
    }

    return MPASSED();
}

TEST_CASE("Lib/FileSystem/BufferedFileTape", "Test buffered read/write")
{
    {
        BufferedWriteTape<true>
            ft(open_file_write(LIT("test.file.test")), System_Allocator, 3);
        REQUIRE(ft.write_str(LIT("Hello, world!")), "");
    }

    {
        CREATE_SCOPED_ARENA(&System_Allocator, temp, KILOBYTES(1));

        BufferedReadTape<true>
            ft(open_file_read(LIT("test.file.test")), System_Allocator, 3);

        Str s = ft.read_str(temp, 13);
        REQUIRE(s != Str::NullStr, "");

        REQUIRE(s == LIT("Hello, world!"), "");
    }
}

TEST_CASE("Lib/Tape/AllocTape", "AllocTape nominal behavior")
{
    AllocWriteTape t(System_Allocator);

    Str test = LIT("Hello, world!");

    REQUIRE(t.write_str(test), "");

    Str s(t.ptr, t.offset);
    REQUIRE(s == test, "");

    return MPASSED();
}