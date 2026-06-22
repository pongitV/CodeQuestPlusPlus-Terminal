cmake_minimum_required(VERSION 3.10)
project(JogoRPG)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Encontra todos os arquivos .cpp dentro da pasta 'RPG C++'
file(GLOB_RECURSE SOURCES "RPG C++/*.cpp")

# Cria o executável
add_executable(JogoRPG ${SOURCES})

# Adiciona a pasta 'RPG C++' como diretório de inclusão
target_include_directories(JogoRPG PRIVATE "RPG C++")

# Flags recomendadas (iguais às do Makefile antigo)
target_compile_options(JogoRPG PRIVATE -Wall -Wextra -O2)

# Linkagem estática (opcional, para rodar sem precisar das DLLs do MinGW)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++")