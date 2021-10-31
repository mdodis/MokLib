-- Library
project "Lib"
    kind "StaticLib"
    language "C++"

    files { "**.h", "**.cpp" }

    DefaultSettings()

    includedirs { "." }