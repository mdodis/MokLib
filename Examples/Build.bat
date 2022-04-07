@echo off

set CFLAGS=-I.. -nologo -Zi

cl Basic.cpp     %CFLAGS% -Fe:Basic.exe
rem cl Memory.cpp    %CFLAGS% -Fe:Memory.exe
rem cl Importers.cpp %CFLAGS% -Fe:Importers.exe
rem cl Tests.cpp     %CFLAGS% -Fe:Tests.exe
rem cl Delegates.cpp %CFLAGS% -Fe:Delegates.exe