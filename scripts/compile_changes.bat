@echo off
setlocal
cd /d "%~dp0\.."
set PATH=C:\msys64\ucrt64\bin;%PATH%

echo ============================================================
echo  CodeQuestPlusPlus-Terminal - Incremental Build
echo ============================================================
echo.

echo Compiling only the changes (Incremental Build)...
cmake --build build --config Release
if %errorlevel% neq 0 (
    echo.
    echo Error during compilation.
    exit /b %errorlevel%
)

echo.
echo Compilation successfully finished!
echo Executable updated at: bin\CodeQuestPlusPlus-Terminal.exe
