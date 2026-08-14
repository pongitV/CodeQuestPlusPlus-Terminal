@echo off
setlocal
cd /d "%~dp0\.."
set PATH=C:\msys64\ucrt64\bin;%PATH%

echo ============================================================
echo  CodeQuestPlusPlus-Terminal - Compilacao Incremental
echo ============================================================
echo.

echo Compilando apenas as alteracoes...
cmake --build build --config Release
if %errorlevel% neq 0 (
    echo.
    echo ERRO durante a compilacao das alteracoes.
    exit /b %errorlevel%
)

echo.
echo Compilacao concluida com sucesso!
echo Executavel atualizado em: bin\CodeQuestPlusPlus-Terminal.exe
