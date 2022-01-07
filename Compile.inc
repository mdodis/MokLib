/**
 * Include this file if you want to compile the library as a single unit
 */

#ifndef MOK_INCLUDE_DLL
    #define MOK_INCLUDE_DLL 0
#endif

#define MOK_LIB_STATIC 1

#include "Arguments.cpp"
#include "Hashing.cpp"
#include "Log.cpp"
#include "Parsing.cpp"
#include "Str.cpp"
#include "StringBuilder.cpp"
#include "FileSystem/FileSystem.cpp"
#include "FileSystem/DirectoryIterator.cpp"
#include "Importers/Importer.cpp"
#include "Memory/Arena.cpp"
#include "Memory/Pool.cpp"
#include "Memory/Sentry.cpp"
#include "Thread/ThreadContext.cpp"
#include "Time/Time.cpp"
#include "Types.cpp"

#if MOK_INCLUDE_DLL
    #include "FileSystem/Dll.cpp"
#endif