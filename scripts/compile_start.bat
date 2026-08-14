@echo off
setlocal
cd /d "%~dp0\.."
set PATH=C:\msys64\ucrt64\bin;%PATH%

echo ============================================================
echo  CodeQuestPlusPlus-Terminal - Clean Build (From Scratch)
echo ============================================================
echo.

echo [1/3] Cleaning the build folder...
if exist build rmdir /s /q build

echo.
echo [2/3] Generating build files with CMake...
cmake -G "MinGW Makefiles" -S . -B build
if %errorlevel% neq 0 (
    echo Error generating CMake build files.
    exit /b %errorlevel%
)

echo.
echo [3/3] Compiling from scratch (Clean Build)...
cmake --build build --config Release
if %errorlevel% neq 0 (
    echo.
    echo Error during compilation.
    exit /b %errorlevel%
)

echo.
echo Compilation successfully finished!
echo Executable generated at: bin\CodeQuestPlusPlus-Terminal.exe
