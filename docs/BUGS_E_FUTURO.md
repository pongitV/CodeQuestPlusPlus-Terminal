# CodeQuest++ (Terminal Version) - Bugs Conhecidos e Coisas Faltando

Este documento lista os bugs conhecidos e funcionalidades incompletas presentes na ultima versao da Engine Raycaster de terminal. Esse registro serve como base caso alguem deseje fazer um fork ou continuar o desenvolvimento desta versao antes da migracao para Direct2D.

## Bugs Conhecidos

- **Popup do Ork na Caverna**: O popup de "aceitar combate" contra o Ork na caverna aparece com o fundo cinza muito esticado para fora da caixa de diálogo, agora apenas na horizontal.
- **Fundo de Combate na Vila**: O fundo 3D renderizado na tela de combate nao esta funcionando corretamente quando o combate ocorre na Vila; ele exibe apenas um fundo cinza em vez do cenário renderizado.

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
