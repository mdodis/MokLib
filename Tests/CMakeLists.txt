cmake_minimum_required(VERSION 3.0.0)

set(SOURCES
    "./Arg.test.cpp"
    "./Result.test.cpp"
    "./Str.test.cpp"
    "./Tape.test.cpp"
    "./Tape.bench.cpp"
    "./Types.test.cpp"
    "./TimerGroup.test.cpp"
    "./Sort.test.cpp"
    "./JSON.test.cpp"
    "./Map.test.cpp"
    "./Arena.test.cpp"
    "./Tests.cpp"
    )

add_executable(MokLibTests ${SOURCES})

target_link_libraries(MokLibTests
    MokLib)
add_test(
    NAME MokLibTests
    COMMAND MokLibTests
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
set_property(TEST MokLibTests PROPERTY PASS_REGULAR_EXPRESSION "Passed")
set_property(TEST MokLibTests PROPERTY FAIL_REGULAR_EXPRESSION "Failed")
