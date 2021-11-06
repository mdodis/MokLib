-- Library
project "Lib"
    kind "StaticLib"
    language "C++"

    files { 
        "*.h", "*.cpp",
        "Containers/*.h", "Containers/*.cpp",
        "FileSystem/*.h", "FileSystem/*.cpp",
        "Math/*.h", "Math/*.cpp",
        "Memory/*.h", "Memory/*.cpp",
        "Thread/*.h", "Thread/*.cpp",
        "Time/*.h", "Time/*.cpp",
    }

    DefaultSettings()

    includedirs { "." }