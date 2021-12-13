#!/bin/bash

g++ ./Basic.cpp     -I.. -oBasic -lpthread
g++ ./Memory.cpp    -I.. -oMemory -lpthread
g++ ./Importers.cpp -I.. -oImporters -lpthread
g++ ./Tests.cpp     -I.. -oTests -lpthread