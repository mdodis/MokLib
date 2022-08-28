#!/bin/bash
cwd="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
pushd $cwd
# g++ ./Basic.cpp      -I.. -oBasic -lpthread
# g++ ./Memory.cpp     -I.. -oMemory -lpthread
# g++ ./Importers.cpp  -I.. -oImporters -lpthread
# g++ ./Tests.cpp      -I.. -oTests -lpthread
# g++ ./FileSystem.cpp -I.. -oFileSystem -lpthread
g++ ./ElfDebugSymbols.cpp -I.. -oElfDebugSymbols -lpthread

popd