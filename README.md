<p align="center">
  <img src="assets/icon.png" alt="CodeQuest++ Terminal Logo" width="128" height="128" style="border-radius: 24px;" />
</p>

<p align="center">
  <strong>RPG tático e motor de renderização 3D Raycaster desenvolvido puramente no terminal em C++23.</strong>
</p>

<p align="center">
  <a href="https://isocpp.org/"><img src="https://img.shields.io/badge/C%2B%2B-23-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white" alt="C++23" /></a>
  <a href="https://cmake.org/"><img src="https://img.shields.io/badge/CMake-3.10+-064F8C?style=for-the-badge&logo=cmake&logoColor=white" alt="CMake" /></a>
  <a href="https://learn.microsoft.com/windows/console/"><img src="https://img.shields.io/badge/Plataforma-Windows_10%2F11-0078D6?style=for-the-badge&logo=windows&logoColor=white" alt="Plataforma Windows" /></a>
  <a href="LICENSE"><img src="https://img.shields.io/badge/Licen%C3%A7a-GNU_GPLv3-blue.svg?style=for-the-badge" alt="Licença GPLv3" /></a>
  <img src="https://img.shields.io/badge/Depend%C3%AAncias-Zero_Externas-success?style=for-the-badge" alt="Zero Dependências Externas" />
</p>

---

<p align="center">
  <strong>Idiomas / Languages:</strong><br>
  <a href="README.md"><strong>Português (Atual)</strong></a> &nbsp;|&nbsp; <a href="README_EN.md"><strong>English</strong></a>
</p>

---

## Sumário

- [Visão Geral](#visão-geral)
- [Recursos Principais](#recursos-principais)
- [Estrutura Arquitetural](#estrutura-arquitetural)
- [Classes, Raças e Sistemas](#classes-raças-e-sistemas)
- [Controles do Jogo](#controles-do-jogo)
- [Compilação e Execução](#compilação-e-execução)
  - [Pré-requisitos](#pré-requisitos)
  - [Scripts Rápidos (Recomendado)](#scripts-rápidos-recomendado)
  - [Compilação Manual via CMake](#compilação-manual-via-cmake)
  - [Executando o Jogo](#executando-o-jogo)
- [Perspectivas de Visualização](#perspectivas-de-visualização)
- [Retrospectiva de Engenharia (Post-Mortem)](#retrospectiva-de-engenharia-post-mortem)
- [Documentação e Futuro](#documentação-e-futuro)
- [Licença](#licença)

---

## Visão Geral

**CodeQuestPlusPlus-Terminal** é um jogo de RPG e um motor de renderização tridimensional construído do zero em **C++23 puro**, operando integralmente dentro da linha de comando do Windows (Win32 Console).

O projeto nasceu como um estudo prático e aprofundado de programação orientada a objetos (POO), padrões de projeto de software (*State Pattern*, *Factory*, *Dependency Injection*, *Screen Registry*) e computação gráfica de baixo nível — projetando um ambiente pseudo-3D (*Raycasting*) sem recorrer a engines prontas (Unity, Unreal) nem a bibliotecas de aceleração gráfica (OpenGL, DirectX, SDL).

---

## Recursos Principais

- **Motor de Renderização Raycaster 3D**: Projeção e rasterização de paredes, portas e iluminação com atenuação de distância diretamente convertida em caracteres e cores ANSI/Win32.
- **Sistema de Combate em Turnos**:
  - Mecânica dinâmica de aparo (*Parry*) com timing reativo.
  - Habilidades de classe exclusivas, magias elementais e consumíveis táticos.
  - IA de monstros com árvores de drop, cálculo de dano físico/mágico e resistências.
- **Sistemas de RPG Abrangentes**:
  - **4 Classes Jogáveis**: Arqueiro (*Archer*), Bardo (*Bard*), Mago (*Mage*), Guerreiro (*Warrior*).
  - **4 Raças com Atributos Únicos**: Anão (*Dwarf*), Elfo (*Elf*), Humano (*Human*), Orc (*Orc*).
  - Gestão de inventário interativa, equipamentos (armaduras, elmos, escudos, armas).
  - Bestiário com catálogo de criaturas enfrentadas e Diário de Missões em tempo real.
- **Mundo Expansivo em Mapas**:
  - Vila Inicial com NPCs interativos (Ferreiro, Vendedor de Comida, Mestre de Treino).
  - Floresta com quebra-cabeça de labirinto, baús de tesouro escondidos e caverna escura.
  - Ponte do Reino e Castelo Real com guardas e eventos de história.
- **Entrada Híbrida (Teclado + Mouse)**:
  - Navegação fluida via teclado e suporte a cliques de mouse diretamente no buffer do console através da API de eventos Win32.

---

## Estrutura Arquitetural

O projeto adota separação estrita de responsabilidades em camadas desacopladas:

```
CodeQuestPlusPlus-Terminal/
├── CMakeLists.txt              # Configuração raiz do CMake
├── assets/                     # Recursos visuais e metadados
│   ├── icon.png                # Ícone do aplicativo em alta resolução (PNG)
│   ├── icon.ico                # Ícone do aplicativo em múltiplas resoluções
│   └── icon.rc                 # Script de recursos Win32
├── bin/                        # Diretório de saída dos binários compilados
│   └── CodeQuestPlusPlus-Terminal.exe
├── cmake/                      # Configurações de compilação modular
│   └── CMakeLists.txt
├── docs/                       # Documentação técnica adicional
│   └── BUGS_E_FUTURO.md        # Relatório de pendências e planejamento
├── scripts/                    # Scripts de automação de build (Windows)
│   ├── compilar_inicio.bat     # Build limpo completo (PT)
│   ├── compilar_mudancas.bat   # Build incremental rápido (PT)
│   ├── compile_start.bat       # Clean build (EN)
│   └── compile_changes.bat     # Incremental build (EN)
└── src/                        # Código-fonte principal em C++23
    ├── Main.cpp                # Ponto de entrada e elevação de privilégios
    ├── Core/                   # Motor central, temporizadores, console e I/O
    │   ├── Engine/             # Game loop, gerenciamento de estado (StateManager)
    │   ├── Terminal/           # Aparência, cores, animações e buffers de tela
    │   └── Utils/              # Despachador de entradas e utilitários
    ├── Domain/                 # Entidades e regras de domínio
    │   ├── Characters/         # Classes de personagens, raças e atributos
    │   ├── Items/              # Fábrica de itens, equipamentos e consumíveis
    │   └── NPCs/               # Lógica de diálogo e interação de NPCs
    ├── Systems/                # Mecânicas de jogo
    │   ├── Combat/             # Máquina de combate, parry e cálculo de turnos
    │   ├── Inventory/          # Inventário de combate e gerenciamento de carga
    │   ├── Minigames/          # Minigame de terminal (Console Hacking)
    │   └── Progression/        # Bestiário, Diário e controle de flags de missão
    ├── UI/                     # Camada de apresentação e interface
    │   ├── PerspectiveManager  # Gerenciador de perspectivas ativas
    │   ├── Renderers/          # Renderizadores gráficos (Engine 3D Raycaster)
    │   └── Screens/            # Telas de UI (Menu, Inventário, Diário, etc.)
    └── World/                  # Mapas, cenários, física e transições
```

---

## Classes, Raças e Sistemas

### Classes Disponíveis
| Classe | Especialidade | Habilidade Chave |
| :--- | :--- | :--- |
| **Archer** | Agilidade e Dano Crítico à Distância | Disparos múltiplos perfurantes e esquiva |
| **Bard** | Suporte Tático e Modificadores de Status | Canções de cura, buffs de moral e atordoamento |
| **Mage** | Alto Dano Elemental Mágico | Feitiços arcanos, manipulação de mana e barreiras |
| **Warrior** | Tanque de Alta Defesa e Força Bruta | Golpes devastadores e maestria em parry/bloqueio |

### Raças
- **Anão (*Dwarf*)**: Alta constituição física e resistências naturais.
- **Elfo (*Elf*)**: Bônus de destreza, evasão elevada e afinidade mágica.
- **Humano (*Human*)**: Atributos equilibrados com alta adaptabilidade.
- **Orc (*Orc*)**: Força física superior e fúria em combate.

---

## Controles do Jogo

| Tecla / Ação | Função | Contexto |
| :---: | :--- | :--- |
| <kbd>W</kbd> / <kbd>↑</kbd> | Mover para Cima / Avançar | Exploração no Mapa |
| <kbd>S</kbd> / <kbd>↓</kbd> | Mover para Baixo / Recuar | Exploração no Mapa |
| <kbd>A</kbd> / <kbd>←</kbd> | Mover para a Esquerda | Exploração no Mapa |
| <kbd>D</kbd> / <kbd>→</kbd> | Mover para a Direita | Exploração no Mapa |
| <kbd>I</kbd> | Abrir Inventário de Itens | Geral |
| <kbd>C</kbd> | Ficha de Atributos do Personagem | Geral |
| <kbd>B</kbd> | Abrir Bestiário e Diário de Missões | Geral |
| <kbd>ESC</kbd> | Menu de Pausa | Geral |
| <kbd>Clique Esquerdo</kbd> | Selecionar Opções em Menus | Telas interativas / Menus |
| <kbd>`</kbd> / <kbd>\</kbd> / <kbd>=</kbd> | Menu de Debug (Desenvolvimento) | Modo Desenvolvedor |

---

## Compilação e Execução

### Pré-requisitos

1. **Sistema Operacional**: Windows 10 ou Windows 11 (requer APIs Win32 nativas de console).
2. **Compilador C++23**: MinGW-w64 (GCC 13+) ou MSVC (Visual Studio 2022+).
3. **CMake**: Versão 3.10 ou superior.

### Scripts Rápidos (Recomendado)

O repositório fornece scripts `.bat` prontos para uso:

- **Build Limpo (Do Zero)**:
  ```cmd
  scripts\compilar_inicio.bat
  ```
- **Build Incremental (Apenas alterações)**:
  ```cmd
  scripts\compilar_mudancas.bat
  ```

### Compilação Manual via CMake

Caso prefira compilar manualmente via linha de comando:

```bash
# 1. Configurar diretório de build
cmake -G "MinGW Makefiles" -S . -B build

# 2. Compilar binário otimizado
cmake --build build --config Release
```

### Executando o Jogo

O binário final com o ícone embutido é gerado em `bin/CodeQuestPlusPlus-Terminal.exe`.

**No Windows Terminal / PowerShell:**
```powershell
.\bin\CodeQuestPlusPlus-Terminal.exe
```

**No CMD (Prompt de Comando com UTF-8):**
```cmd
chcp 65001
bin\CodeQuestPlusPlus-Terminal.exe
```

> [!NOTE]
> O jogo solicita automaticamente elevação de privilégios (UAC / Administrador) na inicialização através de `ensureAdmin()` em [`src/Main.cpp`](src/Main.cpp) para habilitar suporte total a buffers de console, redimensionamento de janela e captura de eventos de mouse.

---

## Perspectivas de Visualização

1. **Visão Raycaster 3D (Ativa e Funcional)**:
   - Projeção tridimensional dos cenários e combate em primeira pessoa.
   - Renderização baseada em colunas com cálculo de distância euclidiana, iluminação e paredes texturizadas por caracteres.
2. **Visão Terminal IDE (Experimental / Suspensa)**:
   - Proposta didática para exibir o fluxo de execução do código-fonte em tempo real com coloração de sintaxe durante o gameplay.

---

## Retrospectiva de Engenharia (Post-Mortem)

O desenvolvimento deste projeto foi um exercício rigoroso de engenharia de software e limites de hardware no ambiente de console:

1. **Restrições de Console**: O terminal do Windows não possui aceleração por GPU. Cada frame 3D é gerado por força bruta na CPU, exigindo otimizações agressivas de buffer de strings e chamadas de console para mitigar oscilações (*screen tearing*).
2. **Design Patterns em C++ Moderno**: Adoção do padrão *State* para orquestrar transições entre menus, exploração em tempo real e combate sem vazamento de estado.
3. **Evolução da Arquitetura**: As lições de arquitetura obtidas nesta versão serviram como fundação para a futura transição da engine para uma API gráfica de hardware (Direct2D/Vulkan).

---

## Documentação e Futuro

Para consultar o relatório detalhado de pendências conhecidas, bugs de layout e sugestões para forks da versão terminal, veja:
- [docs/BUGS_E_FUTURO.md](docs/BUGS_E_FUTURO.md)

---

## Licença

Este projeto é software livre e está licenciado sob os termos da **GNU General Public License v3.0 (GPLv3)**. Para mais detalhes, consulte o arquivo [LICENSE](LICENSE).
