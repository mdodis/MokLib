cmake_minimum_required(VERSION 3.0.0)

file(GLOB_RECURSE SOURCES ./*.cpp)

set(MOKLIB_COMPILE_STANDALONE_IMPORTERS On CACHE FILEPATH "Additional")

set(SOURCES
    "Arg.cpp"
    "Arguments.cpp"
    "Delegates.cpp"
    "Hashing.cpp"
    "Log.cpp"
    "Parsing.cpp"
    "Reflection.h"
    "Reflection.cpp"
    "Str.h"
    "Str.cpp"
    "StringFormat.cpp"
    "Tape.cpp"
    "Types.cpp"
    "Types.h"
    "Converters/ImageConverter.cpp"
    "Debugging/Backtrace.cpp"
    "Debugging/DebugOutput.cpp"
    "Exporters/Exporters.cpp"
    "FileSystem/DirectoryIterator.cpp"
    "FileSystem/Dll.cpp"
    "FileSystem/FileSystem.cpp"
    "Memory/Arena.cpp"
    "Memory/Cache.cpp"
    "Memory/Pool.cpp"
    "Memory/Sentry.cpp"
    "Memory/Var.cpp"
    "Platform/Keycodes.cpp"
    "Serialization/Base.h"
    "Serialization/JSON.h"
    "Serialization/JSON.cpp"
    "Test/Base.cpp"
    "Thread/Job.cpp"
    "Thread/Mutex.cpp"
    "Thread/Process.cpp"
    "Thread/Semaphore.cpp"
    "Thread/Thread.cpp"
    "Thread/ThreadContext.cpp"
    "Thread/TimerQueue.cpp"
    "Time/Time.cpp"
    "Time/TimerGroup.cpp"
    "Importers/Importer.cpp"
    "Compat/Win32Base.h"
    "Compat/Win32Extra.h"
    "Containers/Extras.h"
    )

# if (MOKLIB_COMPILE_STANDALONE_IMPORTERS)
#     set(SOURCES ${SOURCES}
#         "Importers/Image/BMP.cpp"
#         "Importers/Model/OBJ.cpp"
#     )
# endif()

# Add Natvis file if we're on Windows
if (WIN32)
    set(SOURCES ${SOURCES}
        "Lib.natvis"
    )
endif()

add_library(MokLib SHARED 
    ${SOURCES} )

target_compile_features(MokLib PUBLIC
    cxx_std_17)

target_compile_definitions(MokLib PRIVATE
    MOK_LIB_INTERNAL=1
    _CRT_SECURE_NO_WARNINGS
    )

target_include_directories(MokLib PUBLIC
    "./")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules" CACHE INTERNAL "")

add_subdirectory("Tests")