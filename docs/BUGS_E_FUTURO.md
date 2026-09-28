# CodeQuest++ (Terminal Version) - Bugs Conhecidos e Coisas Faltando

Este documento lista os bugs conhecidos e funcionalidades incompletas presentes na ultima versao da Engine Raycaster de terminal. Esse registro serve como base caso alguem deseje fazer um fork ou continuar o desenvolvimento desta versao antes da migracao para Direct2D.

## Bugs Resolvidos

- **Popup do Ork na Caverna (Corrigido)**: O estiramento horizontal do fundo cinza foi corrigido resetando as coordenadas/dimensões estáticas em `startPopupInteraction()`, limpando o terminal com `\033[0m` em vez de repintar com background cinza em `cleanPopupPrevious()`, e garantindo que `maxWidth` / `totalWidth` em `BaseScreen::createBox` e `createBoxWithArt` considerem a largura do título, unificando a largura de bordas e linhas.
- **Fundo de Combate na Vila (Corrigido)**: A arena de combate da Vila/Início em `RaycasterCombatRenderer.cpp` utilizava o caractere `'T'` em suas paredes. Como `'T'` estava registrado como entidade (Troll) em `RaycasterWorld::isEntity`, os raios do raycaster atravessavam a parede sem colisão sólida (`hitWall`), atingindo a distância máxima e gerando o fundo cinza. Substituídas as paredes da arena por `'#'` (pedra da vila), renderizando o cenário 3D perfeitamente.

## Bugs Conhecidos

- *(Nenhum bug crítico pendente no momento)*

## Coisas Faltando / Possiveis Melhorias Futuras

- Balanceamento de danos e vida dos monstros em niveis mais avancados.
- Implementar novas classes e racas adicionais.
- Ajuste de responsividade do terminal para diferentes tamanhos de fontes.
- Efeitos sonoros para as transicoes (se um wrapper/biblioteca externa for incluida no fork).

## Perspectiva IDE

- **Status**: Concluída e Operacional.
- **Metodologia**: A Perspectiva IDE representa o código fonte do jogo em ação durante todo o gameplay. Ao pressionar a tecla `V`, o jogo transiciona de forma fluida e persistente para o modo código C++ vivo:
  - **Exploração 2D (Opção A)**: Tela dividida em abas de editor (`[ Map.cpp ] [ PlayerState.hpp ]`), com o grid 2D à esquerda e o inspetor de objetos em tempo real à direita inspecionando a struct `Hero` e a classe do inimigo/entidade mais próxima com cálculo vetorial de distância e atributos em código.
  - **Combate C++ Vivo**: Inimigos modelados lado a lado como instâncias de classes (`class <Nome> : public Monster`), atributos piscando em tempo real com animações de dano/cura, menus expressos como invocações de método (`hero->attack(&target);`) e chamadas de gerenciamento de memória (`delete &enemy;`).
  - **Telas e Menus Temáticos**: Inventário como buffer de memória (`std::vector<std::unique_ptr<Item>>`), ficha de personagem como struct com offsets de memória, bestiário como catálogo de headers C++, diário como trace logs de execução, menu de pause como sistema de breakpoints e vitória/derrota com exit code 0 e stack traces de exceções.
  - **Alternância Universal ('V')**: A tecla `V` alterna a perspectiva de forma transparente tanto na exploração quanto nos turnos de combate, mantendo o estado de forma persistente.
