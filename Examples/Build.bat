@echo off

set CFLAGS=-I.. -nologo -Zi

rem cl Basic.cpp      %CFLAGS% -Fe:Basic.exe
rem cl Memory.cpp     %CFLAGS% -Fe:Memory.exe
rem cl Importers.cpp  %CFLAGS% -Fe:Importers.exe
rem cl Tests.cpp      %CFLAGS% -Fe:Tests.exe
rem cl Delegates.cpp  %CFLAGS% -Fe:Delegates.exe
cl Reflection.cpp %CFLAGS% -Fe:Reflection.exe