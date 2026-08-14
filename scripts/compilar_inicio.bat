@echo off
setlocal
cd /d "%~dp0\.."
set PATH=C:\msys64\ucrt64\bin;%PATH%

echo ============================================================
echo  CodeQuestPlusPlus-Terminal - Compilacao Limpa (Do Inicio)
echo ============================================================
echo.

echo [1/3] Limpando pasta de build...
if exist build rmdir /s /q build

echo.
echo [2/3] Gerando arquivos de build com CMake...
cmake -G "MinGW Makefiles" -S . -B build
if %errorlevel% neq 0 (
    echo ERRO ao gerar arquivos de build do CMake.
    exit /b %errorlevel%
)

echo.
echo [3/3] Compilando o projeto do zero...
cmake --build build --config Release
if %errorlevel% neq 0 (
    echo ERRO durante a compilacao.
    exit /b %errorlevel%
)

echo.
echo Compilacao concluida com sucesso!
echo Executavel gerado em: bin\CodeQuestPlusPlus-Terminal.exe
