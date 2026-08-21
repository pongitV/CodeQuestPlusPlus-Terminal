#pragma once
#include <vector>
#include <string>

class MapPhysics {
public:
    // [PT-BR] Garante que a posicao nao saia dos limites da matriz do mapa
    // [EN-US] Ensures position does not exceed map matrix boundaries
    static void applyMapLimits(int& positionX, int& positionY, const std::vector<std::string>& mapMatrix);

    // [PT-BR] Movimenta inimigos na matriz respeitando limites e posicoes originais
    // [EN-US] Moves enemies on map matrix respecting limits and original spawn positions
    static void moveEnemiesRandomly(std::vector<std::string>& currentMapMatrix, const std::vector<std::string>& originalMatrix, const std::string& enemySymbols, int playerX, int playerY);
};
