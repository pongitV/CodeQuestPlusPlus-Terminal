# Configuração completa do projeto (delegada do CMakeLists.txt raiz)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define o destino de todos os binários (executáveis, bibliotecas e arquivos) como a pasta bin/ na raiz do repositório
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

# Encontra todos os arquivos .cpp dentro da pasta 'src' (excluindo PerspectivaIDE)
file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE IDE_SOURCES "${CMAKE_SOURCE_DIR}/src/UI/Renderers/IDE/*.cpp")
list(REMOVE_ITEM SOURCES ${IDE_SOURCES})

# Adiciona o arquivo de recursos do icone se ele existir (para Windows)
if(WIN32 AND EXISTS "${CMAKE_SOURCE_DIR}/assets/icon.rc")
    list(APPEND SOURCES "${CMAKE_SOURCE_DIR}/assets/icon.rc")
endif()

# Adiciona o PNG para visibilidade na IDE
if(EXISTS "${CMAKE_SOURCE_DIR}/assets/CodeQuestPlusPlusIcon.png")
    list(APPEND SOURCES "${CMAKE_SOURCE_DIR}/assets/CodeQuestPlusPlusIcon.png")
endif()

# Cria o executável
add_executable(CodeQuestPlusPlus-Terminal ${SOURCES})
set_target_properties(CodeQuestPlusPlus-Terminal PROPERTIES OUTPUT_NAME "CodeQuestPlusPlus-Terminal")

# Adiciona as pastas 'src' e 'assets' como diretorios de inclusao
target_include_directories(CodeQuestPlusPlus-Terminal PRIVATE "${CMAKE_SOURCE_DIR}/src" "${CMAKE_SOURCE_DIR}/assets")
if(WIN32 AND EXISTS "${CMAKE_SOURCE_DIR}/assets/icon.rc")
    set_source_files_properties("${CMAKE_SOURCE_DIR}/assets/icon.rc" PROPERTIES COMPILE_FLAGS "-I \"${CMAKE_SOURCE_DIR}/assets\"")
endif()

# Flags recomendadas (iguais às do Makefile antigo)
target_compile_options(CodeQuestPlusPlus-Terminal PRIVATE -Wall -Wextra -O2)

# Linkagem estática (opcional, para rodar sem precisar das DLLs do MinGW)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++ -s")

if(WIN32)
    target_link_libraries(CodeQuestPlusPlus-Terminal PRIVATE winmm)
endif()
