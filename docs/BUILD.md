# Build Guide - RPG-Cpp

Este guia contém todos os comandos necessários para compilar e executar o jogo.

## Quick Start

```bash
# 1. Configurar e compilar
cmake -B build --preset default

# 2. Compilar
cmake --build build --preset default

# 3. Executar
./bin/JogoRPG.exe
```

## Presets Disponíveis

### `default` (Debug)
- Configuração de desenvolvimento
- Otimizações mínimas
- Usar para desenvolvimento e debugging

### `release` (Release)
- Configuração de produção
- Otimizações máximas
- Usar para builds finais

## Comandos Completos

### Configurar (Debug)
```bash
cmake -B build --preset default
```

### Compilar (Debug)
```bash
cmake --build build --preset default
```

### Configurar (Release)
```bash
cmake -B build --preset release
```

### Compilar (Release)
```bash
cmake --build build --preset release
```

### Executar
```bash
./bin/JogoRPG.exe
```

## Estrutura de Arquivos

```
RPG-Cpp/
├── CMakeLists.txt        # Entry point mínimo (delega para cmake/)
├── cmake/                # Configuração completa de build
│   ├── CMakeLists.txt    # Configuração principal do CMake
│   └── CMakePresets.json # Presets de build automáticos
├── docs/                 # Documentação
│   └── BUILD.md          # Este arquivo
├── build/                # Directory de build (ignorar em version control)
│   ├── CMakeCache.txt    # Cache do CMake
│   ├── CMakeFiles/       # Arquivos temporários do CMake
│   └── ...               # Compilação intermediária
├── bin/                  # Executáveis compilados
│   └── JogoRPG.exe       # Executável final
└── src/                  # Código fonte
```

## Mover o Projeto

Quando mover o projeto para outro diretório:

```bash
# Basta executar:
cmake -B build --preset default
cmake --build build --preset default

# O CMake detecta automaticamente todos os arquivos .cpp
# Não há necessidade de configurar paths manualmente
```

## Troubleshooting

### Erro: "generator : MinGW Makefiles Does not match"
```bash
rm -rf build/
rm -rf bin/
cmake -B build --preset default
```

### Erro: "CMAKE_BUILD_TYPE not defined"
```bash
cmake -B build --preset default
```

### Erro: "No CMAKE_CXX_COMPILER"
Verifique se o GCC/MinGW está instalado:
```bash
g++ --version
```

## Dependências

- **CMake** 3.10+
- **GCC/G++** (MinGW ou MSYS2)
- **MinGW Make** (mingw32-make)
