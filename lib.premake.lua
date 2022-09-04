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
        "Debugging/*.h", "Debugging/*.cpp",
        "Converters/*.h", "Converters/*.cpp",
        "FileSystem/DirectoryIterator.h", "FileSystem/DirectoryIterator.cpp",
        "FileSystem/Dll.h", "FileSystem/Dll.cpp",
        "Reflection.h", "Reflection.cpp",
        "Serialization/*.h", "Serialization/*.cpp",
        "Test/**.h", "Test/**.cpp",
        "Error.h",
    }

    includedirs { "." }

    filter {}