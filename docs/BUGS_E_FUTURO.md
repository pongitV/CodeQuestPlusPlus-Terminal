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

- **Status**: Suspensa e incompleta.
- **Metodologia**: A Perspectiva IDE deve representar o código fonte do jogo em ação durante o gameplay. Seu objetivo é exibir a estrutura de classes, variáveis em tempo real e o fluxo de execução das lógicas de forma didática, simulando como uma IDE exibe o estado de um programa rodando em tempo real no console, tudo formatado com coloração de sintaxe e rastreamento de instâncias e métodos.
