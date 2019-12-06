@echo off

set BUILDDIR=%~dp0
set ROOTDIR="%BUILDDIR%/.."
cd "%ROOTDIR%"

set sources=
for /f "tokens=*" %%f in ('dir /b /a:-d "source\*.md"') do call set sources=%%sources%% "source/%%f"

"./bootstrap.exe" -local-scoping README.md %sources%

call "./mangle.bat" "examples/hello-world/hello-world.md"

:Exit