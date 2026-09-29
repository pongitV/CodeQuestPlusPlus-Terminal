# Configuracao completa do projeto (delegada do CMakeLists.txt raiz)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define o destino de todos os binarios (executaveis, bibliotecas e arquivos) como a pasta bin/ na raiz do repositorio
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

# Encontra todos os arquivos .cpp dentro da pasta 'src'
file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")

# Adiciona o arquivo de recursos de icone se existir (especifico para Windows)
if(WIN32 AND EXISTS "${CMAKE_SOURCE_DIR}/assets/icon.rc")
    list(APPEND SOURCES "${CMAKE_SOURCE_DIR}/assets/icon.rc")
endif()

# Adiciona o PNG para visibilidade na arvore de projeto da IDE
if(EXISTS "${CMAKE_SOURCE_DIR}/assets/icon.png")
    list(APPEND SOURCES "${CMAKE_SOURCE_DIR}/assets/icon.png")
endif()

# Cria o executavel principal do jogo
add_executable(CodeQuestPlusPlus-Terminal ${SOURCES})
set_target_properties(CodeQuestPlusPlus-Terminal PROPERTIES OUTPUT_NAME "CodeQuestPlusPlus-Terminal")

# Adiciona as pastas 'src' e 'assets' como diretorios de inclusao de cabecalhos
target_include_directories(CodeQuestPlusPlus-Terminal PRIVATE "${CMAKE_SOURCE_DIR}/src" "${CMAKE_SOURCE_DIR}/assets")
if(WIN32 AND EXISTS "${CMAKE_SOURCE_DIR}/assets/icon.rc")
    set_source_files_properties("${CMAKE_SOURCE_DIR}/assets/icon.rc" PROPERTIES COMPILE_FLAGS "-I \"${CMAKE_SOURCE_DIR}/assets\"")
endif()

# Flags de compilacao recomendadas (otimizacao O2 e avisos rigorosos)
target_compile_options(CodeQuestPlusPlus-Terminal PRIVATE -Wall -Wextra -O2)

# Vinculacao estatica para permitir execucao sem depender de DLLs externas do MinGW
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++ -s")

if(WIN32)
    target_link_libraries(CodeQuestPlusPlus-Terminal PRIVATE winmm)
endif()
