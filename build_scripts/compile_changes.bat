@echo off
cd %~dp0\..\..
set PATH=C:\msys64\ucrt64\bin;%PATH%
echo Compiling only the changes (Incremental Build)...
cmake --build build --config Release
if %errorlevel% neq 0 (
    echo.
    echo Error compiling.
    exit /b %errorlevel%
)
echo.
echo Compilation successfully finished!
