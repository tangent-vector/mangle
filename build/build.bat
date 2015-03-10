@echo off

set BUILDDIR=%~dp0
set ROOTDIR="%BUILDDIR%/.."
cd "%ROOTDIR%"

"./bootstrap.exe" README.md source/main.md source/string.md source/reader.md

call "./mangle.bat"

:Exit