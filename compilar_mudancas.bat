@echo off
set PATH=C:\msys64\ucrt64\bin;%PATH%
echo Compilando apenas as mudancas (Incremental Build)...
cmake --build build --config Release
if %errorlevel% neq 0 (
    echo.
    echo Erro ao compilar.
    exit /b %errorlevel%
)
echo.
echo Compilacao finalizada com sucesso!
