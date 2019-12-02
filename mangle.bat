@echo off

set ROOT=%~dp0
pushd "%ROOT%"

:: Determine which is newer, between mangle.c and mangle.exe
set SRCFILE=mangle.c
set EXEFILE=mangle.exe
set NEWFILE=
for /F %%F in ('dir /B /O:D %SRCFILE% %EXEFILE%') do set NEWFILE=%%F

if "%NEWFILE%"=="%SRCFILE%" (
	goto Build
) else if "%NEWFILE%"=="%EXEFILE%" (
	goto Run
) else (
	echo error: couldn't find mangle.exe or mangle.c
	goto Error
)

:Build
:: Since we are on Windows, try to build Mangle using Visual Studio
:: TODO: Check for multiple VS versions
if exist "%VS140COMNTOOLS%VSVARS32.bat" (
	call "%VS140COMNTOOLS%VSVARS32.bat"
	goto Compile
)

if exist "%VS120COMNTOOLS%VSVARS32.bat" (
	call "%VS120COMNTOOLS%VSVARS32.bat"
	goto Compile
)

echo "mangle: failed to find Visual Studio comiler"
goto Error

:Compile
cl /nologo /I include mangle.c /link /out:mangle.exe setargv.obj
if %errorlevel% NEQ 0 (
	goto Exit
)
del mangle.obj

:Run
popd
:: We either found mangle.exe, or built it, so we can just invoke it.
"%ROOT%/mangle.exe" %*
goto Exit

:Error
set errorlevel=1

:Exit