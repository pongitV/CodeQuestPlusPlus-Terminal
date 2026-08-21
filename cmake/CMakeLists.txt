# [PT-BR] Configuração completa do projeto (delegada do CMakeLists.txt raiz)
# [EN-US] Complete project configuration (delegated from root CMakeLists.txt)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# [PT-BR] Define o destino de todos os binários (executáveis, bibliotecas e arquivos) como a pasta bin/ na raiz do repositório
# [EN-US] Sets the output directory for all binaries (executables, libraries, archives) to the bin/ folder at the repository root
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

# [PT-BR] Encontra todos os arquivos .cpp dentro da pasta 'src' (excluindo PerspectivaIDE)
# [EN-US] Finds all .cpp source files within the 'src' directory (excluding PerspectivaIDE)
file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE IDE_SOURCES "${CMAKE_SOURCE_DIR}/src/UI/Renderers/IDE/*.cpp")
list(REMOVE_ITEM SOURCES ${IDE_SOURCES})

# [PT-BR] Adiciona o arquivo de recursos de ícone se existir (específico para Windows)
# [EN-US] Adds the icon resource file if it exists (Windows specific)
if(WIN32 AND EXISTS "${CMAKE_SOURCE_DIR}/assets/icon.rc")
    list(APPEND SOURCES "${CMAKE_SOURCE_DIR}/assets/icon.rc")
endif()

# [PT-BR] Adiciona o PNG para visibilidade na árvore de projeto da IDE
# [EN-US] Adds the PNG file for visibility in the IDE project tree
if(EXISTS "${CMAKE_SOURCE_DIR}/assets/icon.png")
    list(APPEND SOURCES "${CMAKE_SOURCE_DIR}/assets/icon.png")
endif()

# [PT-BR] Cria o executável principal do jogo
# [EN-US] Creates the main game executable
add_executable(CodeQuestPlusPlus-Terminal ${SOURCES})
set_target_properties(CodeQuestPlusPlus-Terminal PROPERTIES OUTPUT_NAME "CodeQuestPlusPlus-Terminal")

# [PT-BR] Adiciona as pastas 'src' e 'assets' como diretórios de inclusão de cabeçalhos
# [EN-US] Adds 'src' and 'assets' directories to header search paths
target_include_directories(CodeQuestPlusPlus-Terminal PRIVATE "${CMAKE_SOURCE_DIR}/src" "${CMAKE_SOURCE_DIR}/assets")
if(WIN32 AND EXISTS "${CMAKE_SOURCE_DIR}/assets/icon.rc")
    set_source_files_properties("${CMAKE_SOURCE_DIR}/assets/icon.rc" PROPERTIES COMPILE_FLAGS "-I \"${CMAKE_SOURCE_DIR}/assets\"")
endif()

# [PT-BR] Flags de compilação recomendadas (otimização O2 e avisos rigorosos)
# [EN-US] Recommended compiler flags (O2 optimization and strict warnings)
target_compile_options(CodeQuestPlusPlus-Terminal PRIVATE -Wall -Wextra -O2)

# [PT-BR] Vinculação estática para permitir execução sem depender de DLLs externas do MinGW
# [EN-US] Static linking to enable execution without external MinGW DLL dependencies
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++ -s")

if(WIN32)
    target_link_libraries(CodeQuestPlusPlus-Terminal PRIVATE winmm)
endif()
