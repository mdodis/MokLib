#include "FileSystem/FileSystem.h"

#include "Test/Test.h"

TEST_CASE("Lib/FileSystem/get_file_name", "Test case './my/last/directory/'")
{
    Str test = LIT("./my/last/directory/");
    Str name = get_file_name(test);
    REQUIRE(name == LIT("directory"), "");
    return MPASSED();
}

TEST_CASE("Lib/FileSystem/get_file_name", "Test case './my/last/directory'")
{
    Str test = LIT("./my/last/directory");
    Str name = get_file_name(test);
    REQUIRE(name == LIT("directory"), "");
    return MPASSED();
}

TEST_CASE("Lib/FileSystem/get_file_name", "Test case './my/last/directory/\\'")
{
    Str test = LIT("./my/last/directory/\\");
    Str name = get_file_name(test);
    REQUIRE(name == LIT("directory"), "");
    return MPASSED();
}

TEST_CASE(
    "Lib/FileSystem/get_file_name", "Test case '.\\my\\last\\directory\\\\'")
{
    Str test = LIT(".\\my\\last\\directory\\\\");
    Str name = get_file_name(test);
    REQUIRE(name == LIT("directory"), "");
    return MPASSED();
}