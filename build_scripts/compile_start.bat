@echo off
cd %~dp0\..\..
set PATH=C:\msys64\ucrt64\bin;%PATH%
echo Cleaning the build folder...
if exist build rmdir /s /q build

echo.
echo Generating build files with CMake...
cmake -G "MinGW Makefiles" -S . -B build
if %errorlevel% neq 0 (
    echo Error generating CMake.
    exit /b %errorlevel%
)

echo.
echo Compiling from scratch (Clean Build)...
cmake --build build --config Release
if %errorlevel% neq 0 (
    echo.
    echo Error compiling.
    exit /b %errorlevel%
)
echo.
echo Compilation successfully finished!
