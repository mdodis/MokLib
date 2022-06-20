project "Lib"
    kind "SharedLib"
    language "C++"
    InternalSettings()

    defines {"MOK_LIB_INTERNAL=1"}

    files {
        "*.h", "*.cpp",
        "Containers/*.h", "Containers/*.cpp",
        "FileSystem/FileSystem.h", "FileSystem/FileSystem.cpp",
        "Math/*.h", "Math/*.cpp",
        "Memory/*.h", "Memory/*.cpp",
        "Thread/*.h", "Thread/*.cpp",
        "Time/*.h", "Time/*.cpp",
        "Importers/*.h", "Importers/*.cpp",
        "Debugging/*.h",
        "Converters/*.h", "Converters/*.cpp",
        "FileSystem/DirectoryIterator.h", "FileSystem/DirectoryIterator.cpp",
        "FileSystem/Dll.h", "FileSystem/Dll.cpp",
        "Reflection.h", "Reflection.cpp",
        "Test/**.h", "Test/**.cpp",
        "Error.h"
    }

    includedirs { "." }

    filter {"platforms:Linux64"}
        pic "On"

    filter {}

project "TestLib"
    kind "ConsoleApp"
    language "C++"
    InternalSettings()

    files {
        "Tests/**.h", "Tests/**.cpp"
    }

    filter {}
    includedirs {LibDir}
    links {"Lib"}