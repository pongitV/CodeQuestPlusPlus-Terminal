#pragma once

#include <vector>
#include <string>
#include <functional>

class Character;

class IDEMapExplorationRenderer {
public:
    // [PT-BR] Renderiza a exploração Top-Down no formato Opção A (Mapa à esquerda + Inspector à direita)
    static void render(
        const std::vector<std::string>& mapMatrix,
        int playerPositionX,
        int playerPositionY,
        int terminalWidth,
        int terminalHeight,
        int initialLine,
        const std::function<std::string(char, int, int)>& cellFormatter,
        Character* currentPlayer,
        const std::string& mapTitle
    );
};
