@echo off
set PATH=C:\msys64\ucrt64\bin;%PATH%
echo Limpando a pasta de build...
if exist build rmdir /s /q build

echo.
echo Gerando arquivos de build com CMake...
cmake -G "MinGW Makefiles" -S . -B build
if %errorlevel% neq 0 (
    echo Erro ao gerar CMake.
    exit /b %errorlevel%
)

echo.
echo Compilando do zero (Clean Build)...
cmake --build build --config Release
if %errorlevel% neq 0 (
    echo.
    echo Erro ao compilar.
    exit /b %errorlevel%
)
echo.
echo Compilacao finalizada com sucesso!
