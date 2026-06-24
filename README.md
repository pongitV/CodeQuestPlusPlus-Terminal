# RPG-Cpp

Este é um projeto pessoal desenvolvido em C++ com o objetivo de aprofundar meus conhecimentos na linguagem. O foco principal é a exploração e utilização exclusiva das bibliotecas padrões e métodos da própria linguagem, sem dependências de frameworks ou bibliotecas externas. 

Apesar de utilizar apenas a biblioteca padrão, o projeto é um RPG de console ambicioso e complexo, rodando nativamente no terminal do Windows.

## Funcionalidades e Destaques

O que começou como um projeto de estudo evoluiu para um ecossistema completo com mecânicas avançadas:

*   **Motor Raycaster Pseudo-3D:** Um sistema de renderização 3D no console (similar a Wolfenstein 3D clássico), construído do zero, permitindo a exploração de áreas em primeira pessoa diretamente no terminal.
*   **Arquitetura Robusta:** Utilização do padrão de projeto de Máquina de Estados (State Pattern) para controle do loop do jogo, transição entre mapas (Vila, Floresta, Reino) e menus.
*   **Sistemas de RPG Completos:**
    *   **Combate:** Sistema de turnos com uso de atributos, habilidades dinâmicas, efeitos de status e gerenciamento de inventário em combate.
    *   **Inventário e Itens:** Uso e verificação de equipamentos, consumíveis, melhorias e requisitos de atributos.
    *   **Progressão e Salvamento:** O jogo rastreia flags de história e permite salvar/carregar o progresso do jogador de forma autônoma.

## Princípios de Desenvolvimento

O projeto é estruturado seguindo rigorosamente as boas práticas de engenharia de software:
*   **SOLID:** Princípios de design orientado a objetos para um código robusto e sustentável.
*   **DRY (Don't Repeat Yourself):** Evitando redundâncias e prezando pela reutilização de código.
*   **Organização:** Estrutura modularizada por pastas para facilitar a manutenção.

## Convenções de Nomenclatura (Style Guide)

Para garantir a consistência e a legibilidade do código, adotei o seguinte padrão de sintaxe:

*   **Classes, Structs e Enums:** `PascalCase` (iniciando com a letra maiúscula).
*   **Funções e Métodos:** `camelCase` (iniciando com a letra minúscula).
*   **Variáveis Locais e Parâmetros:** `camelCase` ou `snake_case`.
*   **Variáveis Membro (Classes):** `camelCase` (ocasionalmente `camelCase_` com um underscore no final para indicar contexto interno/privado em structs ou buffers).
*   **Constantes e Macros:** `ALL_CAPS_SNAKE_CASE` (letras maiúsculas separadas por underscores).
*   **Namespaces:** `PascalCase` (ex: `Constantes`, `ArtesMenu`).

## Como Compilar e Executar

O jogo é programado para interagir profundamente com as propriedades do terminal e requer ambiente Windows (utiliza a biblioteca `<windows.h>` para manipulações da tela, cores e requisição de privilégios de Administrador).

**Pré-requisitos:**
*   Compilador C++ compatível com o padrão **C++23**.
*   A ferramenta **CMake** (versão 3.15 ou superior) instalada.
*   Um gerador de build configurado (como MinGW Makefiles, Ninja ou Visual Studio).

**Passos para Compilação via Terminal:**
1. Na raiz do projeto, configure o diretório de build:
   ```bash
   cmake -B build -G "MinGW Makefiles"
   ```
2. Compile o projeto:
   ```bash
   cmake --build build
   ```
3. O executável `JogoRPG.exe` será gerado dentro da pasta `bin/` na raiz do projeto. Você pode iniciá-lo com:
   ```bash
   ./bin/JogoRPG.exe
   ```

**Compilação Direta via GCC (Alternativa sem CMake):**
Caso queira compilar rapidamente usando apenas o `g++` via PowerShell no Windows:
```powershell
g++ -O2 -std=c++23 (Get-ChildItem -Path "src" -Filter "*.cpp" -Recurse | Select-Object -ExpandProperty FullName) -o "bin\JogoRPG.exe" -static -static-libgcc -static-libstdc++ -lwinmm
```

**Compilação via VS Code (Recomendado):**
*   Instale a extensão **CMake Tools** no VS Code.
*   Abra a pasta raiz do projeto. A extensão detectará automaticamente o arquivo `CMakeLists.txt`.
*   Selecione o seu compilador C++23 (Kit).
*   Clique no botão de build na barra de status inferior ou pressione `F7`. O executável será compilado e salvo na pasta `bin/`. Você poderá executá-lo ou depurá-lo diretamente pela extensão.

## Previews Visuals (Screenshots)

> Dica: Coloque imagens e GIFs curtos de gameplay, das telas do jogo e do Motor 3D Raycaster aqui para valorizar muito o seu repositório visualmente!

## Status Atual do Projeto

O repositório está em **manutenção e evolução constante**. Novas funcionalidades, refatorações e melhorias são aplicadas frequentemente de acordo com o histórico de commits.
O progresso e história do jogo estão inacabados com utilização de placeholders para futuras implementações.
