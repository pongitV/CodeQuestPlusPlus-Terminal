#include "World/Systems/MapPhysics.h"
#include <algorithm>

// [PT-BR] Garante que a posicao atual (X,Y) nao extrapole as bordas da matriz do mapa.
// [EN-US] Ensures current position (X,Y) does not exceed map matrix boundaries.
void MapPhysics::applyMapLimits(int& positionX, int& positionY, const std::vector<std::string>& mapMatrix) {
    if (positionY < 0) positionY = 0; 
    else if (positionY >= static_cast<int>(mapMatrix.size())) positionY = static_cast<int>(mapMatrix.size()) - 1;
    
    if (mapMatrix.empty()) return;
    
    int maxCols = static_cast<int>(mapMatrix[positionY].length());
    if (positionX < 0) positionX = 0; 
    else if (positionX >= maxCols) positionX = std::max(0, maxCols - 1);
}

#include "Core/Utils/RandomGenerator.h"
#include <cmath>

// [PT-BR] Movimentacao autonoma de inimigos (roaming/patrulha na matriz do mapa).
// [EN-US] Autonomous enemy movement (roaming/patrolling on map matrix).
void MapPhysics::moveEnemiesRandomly(std::vector<std::string>& currentMapMatrix, const std::vector<std::string>& originalMatrix, const std::string& enemySymbols, int playerX, int playerY) {
    if (enemySymbols.empty()) return;

    struct Pos { int x, y; char c; };
    std::vector<Pos> currentEnemies;
    
    for (int y = 0; y < static_cast<int>(currentMapMatrix.size()); ++y) {
        for (int x = 0; x < static_cast<int>(currentMapMatrix[y].size()); ++x) {
            if (enemySymbols.find(currentMapMatrix[y][x]) != std::string::npos) {
                // [PT-BR] Ignora caracteres pertencentes a rotulos de teleporte (ex: ^S, ^Vila)
                // [EN-US] Ignores characters that are part of teleport labels (e.g., ^S, ^Vila)
                if (x > 0 && currentMapMatrix[y][x-1] == '^') continue;

                currentEnemies.push_back({x, y, currentMapMatrix[y][x]});
            }
        }
    }

    for (const auto& enemy : currentEnemies) {
        if (currentMapMatrix[enemy.y][enemy.x] != enemy.c) continue; 
        
        int originX = -1, originY = -1;
        // [PT-BR] Inimigos patrulham ao redor da sua posicao original de spawn
        // [EN-US] Enemies patrol around their original spawn location
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                int oy = enemy.y + dy;
                int ox = enemy.x + dx;
                if (oy >= 0 && oy < static_cast<int>(originalMatrix.size()) && ox >= 0 && ox < static_cast<int>(originalMatrix[oy].size())) {
                    if (originalMatrix[oy][ox] == enemy.c) {
                        originX = ox;
                        originY = oy;
                        break;
                    }
                }
            }
            if (originX != -1) break;
        }

        if (originX == -1) continue;

        std::vector<std::pair<int, int>> possibleMoves;
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                int ty = originY + dy;
                int tx = originX + dx;
                
                // [PT-BR] Valida se a celula de destino eh andavel ('.') e esta livre
                // [EN-US] Validates if destination cell is walkable ('.') and unoccupied
                if (ty >= 0 && ty < static_cast<int>(currentMapMatrix.size()) && tx >= 0 && tx < static_cast<int>(currentMapMatrix[ty].size())) {
                    if (currentMapMatrix[ty][tx] == '.' && (tx != playerX || ty != playerY)) {
                        // [PT-BR] Limita movimento a celulas adjacentes
                        // [EN-US] Restricts movement to adjacent cells
                        if (std::abs(tx - enemy.x) <= 1 && std::abs(ty - enemy.y) <= 1) {
                            possibleMoves.push_back({tx, ty});
                        }
                    }
                }
            }
        }

        // [PT-BR] Opcao de permanecer parado
        // [EN-US] Option to remain stationary
        possibleMoves.push_back({enemy.x, enemy.y});

        int choice = RandomGenerator::getInteger(0, static_cast<int>(possibleMoves.size()) - 1);
        int nx = possibleMoves[choice].first;
        int ny = possibleMoves[choice].second;

        if (nx != enemy.x || ny != enemy.y) {
            currentMapMatrix[enemy.y][enemy.x] = '.';
            currentMapMatrix[ny][nx] = enemy.c;
        }
    }
}
