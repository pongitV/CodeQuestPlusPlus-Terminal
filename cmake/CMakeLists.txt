# Configuração completa do projeto (delegada do CMakeLists.txt raiz)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define o destino do executável como a pasta bin/ na raiz do repositório
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

# Encontra todos os arquivos .cpp dentro da pasta 'src'
file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")

# Adiciona o arquivo de recursos do icone se ele existir (para Windows)
if(WIN32 AND EXISTS "${CMAKE_SOURCE_DIR}/assets/icone.rc")
    list(APPEND SOURCES "${CMAKE_SOURCE_DIR}/assets/icone.rc")
endif()

# Cria o executável
add_executable(CodeQuestPlusPlus ${SOURCES})
set_target_properties(CodeQuestPlusPlus PROPERTIES OUTPUT_NAME "CodeQuest++")

# Adiciona a pasta 'src' como diretório de inclusão
target_include_directories(CodeQuestPlusPlus PRIVATE "${CMAKE_SOURCE_DIR}/src")

# Flags recomendadas (iguais às do Makefile antigo)
target_compile_options(CodeQuestPlusPlus PRIVATE -Wall -Wextra -O2)

# Linkagem estática (opcional, para rodar sem precisar das DLLs do MinGW)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++ -s")

if(WIN32)
    target_link_libraries(CodeQuestPlusPlus PRIVATE winmm)
endif()
