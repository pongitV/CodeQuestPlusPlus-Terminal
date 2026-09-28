#pragma once

#include <vector>
#include <string>
#include <functional>

class Character;

class IDEMapExplorationRenderer {
public:
    // Renderiza a exploracao Top-Down no formato Opcao A (Mapa a esquerda + Inspector a direita)
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
