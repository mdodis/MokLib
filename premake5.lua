-- Options
-- MokLib_IncludeConverters
-- Library
project "Lib"
    kind "SharedLib"
    language "C++"

    defines {"MOK_LIB_INTERNAL=1"}
    filter {"platforms:Linux64"}
    pic "On"
    filter {}

    files { 
        "*.h", "*.cpp",
        "Containers/*.h", "Containers/*.cpp",
        "FileSystem/FileSystem.h", "FileSystem/FileSystem.cpp",
        "Math/*.h", "Math/*.cpp",
        "Memory/*.h", "Memory/*.cpp",
        "Thread/*.h", "Thread/*.cpp",
        "Time/*.h", "Time/*.cpp",
        "Importers/*.h", "Importers/*.cpp",
        "Debugging/*.h"
    }

    if (MokLib_IncludeConverters == nil) then
        MokLib_IncludeConverters = true
    end


    if (MokLib_IncludeConverters) then
        files {
            "Converters/*.h", "Converters/*.cpp",
        }
    end

    files {
        "FileSystem/DirectoryIterator.h", "FileSystem/DirectoryIterator.cpp",
    }

    files {
        "FileSystem/Dll.h", "FileSystem/Dll.cpp",
    }

    DefaultSettings()

    includedirs { "." }

    filter {}