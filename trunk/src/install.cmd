@echo off
setlocal

if [%1]==[] echo usage: install.cmd bindir includedir libdir

set bindir="%~1"
set includedir="%~2"
set libdir="%~3"

if not %bindir% == "" (
    if not exist %bindir% (
        echo Creating directory %bindir%...
        mkdir %bindir%
    )
    echo Copying executable to %bindir%...
    copy /B zap.exe %bindir%
)
if not %includedir% == "" (
    if not exist %includedir% (
        echo Creating directory %includedir%...
        mkdir %includedir%
    )
    echo Copying header files to %includedir%...
    copy include\*.h %includedir%
)
if not %libdir% == "" (
    if not exist %libdir% (
        echo Creating directory %libdir%...
        mkdir %libdir%
    )
    echo Copying library to %libdir%...
    copy /B libzap.a %libdir%
)

endlocal
@echo on
