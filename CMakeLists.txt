cmake_minimum_required(VERSION 3.0.0)
include(CTest)

file(GLOB_RECURSE SOURCES ./*.cpp)

set(MOKLIB_COMPILE_STANDALONE_IMPORTERS On CACHE FILEPATH "Additional")

file(GLOB SOURCES
    "*.cpp"
    "Debugging/*.cpp"
    "FileSystem/*.cpp"
    "Memory/*.cpp"
    "Platform/*.cpp"
    "Serialization/*.cpp"
    "Test/*.cpp"
    "Thread/*.cpp"
    "Time/*.cpp"
    "Compat/*.cpp"
    "Containers/*.cpp"
    )

file(GLOB_RECURSE HEADERS
    "*.h")

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

option(MOKLIB_COMPILE_STATIC "Build static library" OFF)

if (MOKLIB_COMPILE_STATIC)
    add_library(MokLib STATIC ${SOURCES} ${HEADERS})
else()
    add_library(MokLib SHARED ${SOURCES} ${HEADERS})
endif()


target_compile_features(MokLib PUBLIC
    cxx_std_17)

target_compile_definitions(MokLib PRIVATE
    MOK_LIB_INTERNAL=1
    _CRT_SECURE_NO_WARNINGS
    )


if (MOKLIB_COMPILE_STATIC)
    target_compile_definitions(MokLib PUBLIC
        MOK_LIB_STATIC=1
        _CRT_SECURE_NO_WARNINGS
        )

endif()

target_include_directories(MokLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules" CACHE INTERNAL "")

add_subdirectory("Tests")
