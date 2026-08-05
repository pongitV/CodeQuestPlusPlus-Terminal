#pragma once
#include <vector>
#include <string>

class MapPhysics {
public:
    // Garante que a posição não saia dos limites da matriz do mapa
    static void applyMapLimits(int& positionX, int& positionY, const std::vector<std::string>& mapMatrix);

    // Movimenta inimigos na matriz respeitando limites e posições originais
    static void moveEnemiesRandomly(std::vector<std::string>& currentMapMatrix, const std::vector<std::string>& originalMatrix, const std::string& enemySymbols, int playerX, int playerY);
};
