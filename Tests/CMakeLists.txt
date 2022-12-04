cmake_minimum_required(VERSION 3.0.0)

set(SOURCES
    "./Arg.test.cpp"
    "./Cache.test.cpp"
    "./Result.test.cpp"
    "./Str.test.cpp"
    "./Tape.test.cpp"
    "./Tape.bench.cpp"
    "./Types.test.cpp"
    "./TimerGroup.test.cpp"
    "./Tests.cpp"
    )

add_executable(MokLibTests ${SOURCES})

target_link_libraries(MokLibTests
    MokLib)