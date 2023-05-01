@echo off
pushd "%~dp0"
set cwd="%cd%"
popd

pushd %cwd%\..

cmake . -BBuild

popd