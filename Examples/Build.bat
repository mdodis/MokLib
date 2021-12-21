@echo off

set CFLAGS=-I.. -nologo -Zi

cl Basic.cpp     %CFLAGS% -Fe:Basic.exe
cl Memory.cpp    %CFLAGS% -Fe:Memory.exe
cl Importers.cpp %CFLAGS% -Fe:Importers.exe
cl Tests.cpp     %CFLAGS% -Fe:Tests.exe