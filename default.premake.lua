project "Lib"
    kind "SharedLib"
    language "C++"
    InternalSettings()
    include "lib.premake.lua"


project "TestLib"
    kind "ConsoleApp"
    language "C++"
    InternalSettings()
    include "tests.premake.lua"