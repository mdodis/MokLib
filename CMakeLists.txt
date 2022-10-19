cmake_minimum_required(VERSION 3.0.0)

file(GLOB_RECURSE SOURCES ./*.cpp)

set(SOURCES
    "Arg.cpp"
    "Arguments.cpp"
    "Delegates.cpp"
    "Hashing.cpp"
    "Log.cpp"
    "Parsing.cpp"
    "Reflection.cpp"
    "Str.cpp"
    "StringFormat.cpp"
    "Tape.cpp"
    "Types.cpp"
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
    )

add_library(MokLib SHARED 
    ${SOURCES})

target_compile_features(MokLib PUBLIC
    cxx_std_17)

target_compile_definitions(MokLib PRIVATE
    MOK_LIB_INTERNAL=1
    _CRT_SECURE_NO_WARNINGS
    )

target_include_directories(MokLib PUBLIC
    "./")