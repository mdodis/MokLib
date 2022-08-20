#!/bin/bash
cxx="g++"
cwd="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
pushd $cwd
sources="
    "Arg.test.cpp"
    "Cache.test.cpp"
    "Result.test.cpp"
    "Str.test.cpp"
    "Tape.test.cpp"
    "Tests.cpp"
    "Types.test.cpp"
    "Tape.bench.cpp"
    "Lib.cpp"
"
$cxx $sources -I../ -DMOK_LIB_STATIC=1 -std=c++17 -O0 -g -olib_tests

popd