project "Lib"
    kind "SharedLib"
    language "C++"
    targetdir(IntBinDir)
    objdir(IntObjDir)

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
    }

    includedirs { "." }

    filter {"platforms:Linux64"}
        pic "On"
    InternalSettings()

    filter {}