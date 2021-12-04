-- Options
-- MokLib_IncludeConverters
-- Library
project "Lib"
    kind "StaticLib"
    language "C++"

    files { 
        "*.h", "*.cpp",
        "Containers/*.h", "Containers/*.cpp",
        "FileSystem/FileSystem.h", "FileSystem/FileSystem.cpp",
        "Math/*.h", "Math/*.cpp",
        "Memory/*.h", "Memory/*.cpp",
        "Thread/*.h", "Thread/*.cpp",
        "Time/*.h", "Time/*.cpp",
        "Importers/*.h", "Importers/*.cpp",
    }

    if (MokLib_IncludeConverters == nil) then
        MokLib_IncludeConverters = true
    end


    if (MokLib_IncludeConverters) then
        files {
            "Converters/*.h", "Converters/*.cpp",
        }
    end

    DefaultSettings()

    includedirs { "." }

    filter {}