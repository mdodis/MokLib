@echo off

cl Basic.cpp -I.. -Fe:Basic.exe -Zi
cl Memory.cpp -I.. -Fe:Memory.exe -Zi
cl Importers.cpp -I.. -Fe:Importers.exe -Zi
cl Tests.cpp -I.. -Fe:Tests.exe -Zi