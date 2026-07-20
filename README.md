# CodeQuestPlusPlus-Terminal

Um RPG de terminal implementado em C++ com duas perspectivas: um renderer estilo raycaster para gameplay e um "Modo IDE" que apresenta, em tempo de execução, como partes do código estão funcionando. O projeto é um exercício de aprendizado em C++ sem dependências externas.

Arquivo principal: [src/Principal.cpp](src/Principal.cpp)

Executável pré-compilado: [bin/CodeQuestPlusPlus-Terminal.exe](bin/CodeQuestPlusPlus-Terminal.exe)

## Build com CMake Presets (Recomendado)

### Pré-requisitos

- CMake 3.10+
- Toolchain C++ (MSVC, MinGW ou equivalente)
- Windows 10/11 (o projeto usa APIs Win32 em `src/Principal.cpp`)

### Compilar rapidamente

```bash
# Configurar usando preset (se suportado)
cmake --preset default

# Compilar
cmake --build --preset default
```

### Comandos manuais

```bash
# Configurar (gera pasta build)
cmake -S . -B build

# Compilar
cmake --build build
```

### Observação sobre toolchains

O código utiliza chamadas Win32 (`ShellExecuteEx`, `IsUserAnAdmin`, etc.), portanto é direcionado a Windows. Compilar com MSVC (Visual Studio) é a opção mais direta; MinGW/MSYS2 pode funcionar dependendo do ambiente.

## Executável pré-compilado

Para executar o binário disponível em `bin/`:

PowerShell / Windows Terminal:

```powershell
\bin\CodeQuestPlusPlus-Terminal.exe
```

cmd.exe (garantir UTF-8):

```bat
chcp 65001
\bin\CodeQuestPlusPlus-Terminal.exe
```

O jogo pode solicitar elevação de privilégios (UAC). A chamada `garantirAdmin()` em [src/Principal.cpp](src/Principal.cpp) tenta iniciar uma instância elevada e encerrar a instância atual.

## Estrutura do projeto

```
CodeQuestPlusPlus-Terminal/
├── CMakeLists.txt
├── CMakePresets.json
├── README.md
├── src/
│   ├── Principal.cpp
│   ├── Core/
│   │   ├── Controladores/
│   │   └── Utilidades/
│   ├── Entidades/
│   │   ├── Classes/
│   │   └── Racas/
│   ├── Mapas/
│   ├── Perspectiva/    # Raycaster + Modo IDE
│   └── Sistemas/
└── bin/
	└── CodeQuestPlusPlus-Terminal.exe
```
### View Raycaster
- Combate 3D
- Atributos
- Inventário
- Diário
- Bestiário
- Mapa
- Vitória/Derrota

### View Terminal IDE (SUSPENSA - EM DESENVOLVIMENTO)
- Telas com estilo de editor de código e realce de sintaxe (cores do terminal)
- Visualização de estruturas de classes e fluxo de execução

## Retrospectiva do Projeto (Post-Mortem)

O desenvolvimento desta versão para terminal serviu como um exercício aprofundado em C++ e arquitetura de software, operando sob restrições extremas de ambiente. O projeto exigiu a superação de obstáculos técnicos significativos:

- Motor de projeção 3D (Raycaster) operando puramente no terminal de texto, processando visuais sem aceleração de hardware.
- Estruturação orientada a objetos com forte uso de polimorfismo e injeção de dependências para separar as lógicas do jogo da camada de renderização.
- Gerenciamento persistente de estado (State Pattern) para alternar entre exploração em tempo real, gerenciamento de inventário e combate em turnos sem comprometer a estabilidade do console.

A decisão de suspender esta versão e migrar a arquitetura base para o Direct2D (em um repositório dedicado) baseia-se nas limitações técnicas do console do Windows (CPU-bound, oscilação visual severa em atualizações rápidas, restrições de framerate e limitação de manipulação de *buffers*). A transição para uma API gráfica real permitirá o reaproveitamento dos padrões arquiteturais robustos desenvolvidos aqui, com o benefício direto do controle sobre o hardware gráfico.

## Como adicionar novas telas

1. Criar a tela base em `src/Perspectiva/TelasBase/` (ou pasta equivalente)
2. Criar a tela terminal em `src/Perspectiva/Terminal/TelasIDE/` (ou caminho equivalente)
3. Registrar a nova tela no gerenciador de perspectivas
4. Compilar o projeto

## Como mover o projeto

O projeto usa `CMakePresets.json` para configuração automática. Ao mover para outro diretório:

```bash
cmake --preset default
cmake --build --preset default
```

Não é necessário ajustar paths manualmente — o CMake deve detectar os arquivos do projeto.

## Contribuição

Projeto pessoal focado em aprendizado. Para contribuições, abra issues e envie pull requests com mudanças pequenas e descritas.

## Licença

Este projeto é licenciado sob a licença GNU GPLv3 - veja o arquivo [LICENSE](LICENSE) para detalhes.
