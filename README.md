# RPG-Cpp

Um jogo RPG em C++ com renderização 3D (Raycaster) e visualização Terminal IDE.

## Build com CMake Presets (Recomendado)

### Pré-requisitos

- **CMake** 3.10+
- **GCC/G++** (MinGW ou MSYS2)
- **MinGW Make** (mingw32-make)

### Compilar rapidamente

```bash
# Configurar (Debug) - cria diretório build/
cmake -B build --preset default

# Compilar
cmake --build build --preset default

# Executar
./bin/JogoRPG.exe
```

### Ou usar os comandos manuais

```bash
# Configurar
cmake -B build

# Compilar
cmake --build build

# Executar
./bin/JogoRPG.exe
```

### Presets disponíveis

| Preset | Descrição |
|--------|-----------|
| `default` | Debug, otimizações mínimas |
| `release` | Release, otimizações máximas |

## Estrutura do Projeto

```
RPG-Cpp/
├── CMakeLists.txt          # Configuração principal do CMake
├── CMakePresets.json       # Presets de build automáticos
├── README.md               # Este arquivo
├── src/
│   ├── Principal.cpp       # Entry point
│   ├── Entidades/          # Personagem, Classes, Racas, Inimigos, NPCs
│   ├── Mundo/              # Mapas, Controle de mapa
│   ├── Sistemas/           # Combate, Inventario, Progresso
│   └── Visoes/             # Telas (Raycaster 3D + Terminal IDE)
│       ├── Raycaster/       # Views 3D (Engine 3D)
│       └── Terminal/       # Views Terminal IDE
│           ├── TelasIDE/    # Telas com estilo editor de código
│           └── EngineIDE/   # Renderizador de IDE
└── bin/                    # Executável compilado
```

## Telas Disponíveis

### View Raycaster (3D)
- Tela de Combate 3D
- Tela de Atributos
- Tela de Inventario
- Tela de Diario
- Tela de Bestiario
- Tela de Mapa
- Tela de Vitoria/Derrota

### View Terminal IDE
- Telas com estilo editor de código C++
- Syntax highlighting (terminal colors)
- Estrutura de classes e funções

## Como adicionar novas telas

1. Criar a tela base em `src/Visoes/TelasBase/`
2. Criar a tela terminal em `src/Visoes/Terminal/TelasIDE/`
3. Adicionar ao `GerenciadorVisao.cpp`
4. Compilar com CMake (detecta automaticamente)

## Como mover o projeto

O projeto usa `CMakePresets.json` para configuração automática. Ao mover para outro diretório:

```bash
# Basta executar:
cmake -B build --preset default
cmake --build build --preset default
```

Não há necessidade de configurar paths manualmente — o CMake detecta tudo automaticamente.
