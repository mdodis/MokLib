@echo off

pushd "%~dp0"
set cwd="%cd%"
popd

set CFLAGS=-I%cwd%\.. -nologo -Zi /std:c++17

rem cl Basic.cpp      %CFLAGS% -Fe:Basic.exe
rem cl Memory.cpp     %CFLAGS% -Fe:Memory.exe
rem cl Importers.cpp  %CFLAGS% -Fe:Importers.exe
rem cl Tests.cpp      %CFLAGS% -Fe:Tests.exe
rem cl Delegates.cpp  %CFLAGS% -Fe:Delegates.exe
rem cl Reflection.cpp %CFLAGS% -Fe:Reflection.exe
cl %cwd%\ElfDebugSymbols.cpp %CFLAGS% -Fe:%cwd%\ElfDebugSymbols.exe