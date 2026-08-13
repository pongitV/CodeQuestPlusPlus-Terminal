#include "World/Systems/MapPhysics.h"
#include <algorithm>

/*
 * Garante que a posicao atual (X,Y) nao extrapole as bordas matriz do mapa.
 * Previne falhas de segmentacao durante a movimentacao do player ou entidades.
 */
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

/*
 * Movimentacao Autonoma de Inimigos (Roaming):
 * Varre a matriz atras de entidades definidas em 'enemySymbols' e aplica logica de patrulha.
 */
void MapPhysics::moveEnemiesRandomly(std::vector<std::string>& currentMapMatrix, const std::vector<std::string>& originalMatrix, const std::string& enemySymbols, int playerX, int playerY) {
    if (enemySymbols.empty()) return;

    struct Pos { int x, y; char c; };
    std::vector<Pos> currentEnemies;
    
    for (int y = 0; y < static_cast<int>(currentMapMatrix.size()); ++y) {
        for (int x = 0; x < static_cast<int>(currentMapMatrix[y].size()); ++x) {
            if (enemySymbols.find(currentMapMatrix[y][x]) != std::string::npos) {
                // Ignora caracteres que fazem parte de um rotulo de mapa/teleporte (ex: ^S, ^Vila)
                if (x > 0 && currentMapMatrix[y][x-1] == '^') continue;

                currentEnemies.push_back({x, y, currentMapMatrix[y][x]});
            }
        }
    }

    for (const auto& enemy : currentEnemies) {
        if (currentMapMatrix[enemy.y][enemy.x] != enemy.c) continue; 
        
        int originX = -1, originY = -1;
        /*
         * Mapeamento de Ponto de Origem:
         * Inimigos so podem patrulhar proximos de sua area original de "spawn".
         * Aqui comparamos com a matriz original do mapa para encontrar o centro do perimetro.
         */
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
                
                // Valida se a futura celula eh andavel ('.') e se o espaco esta livre
                if (ty >= 0 && ty < static_cast<int>(currentMapMatrix.size()) && tx >= 0 && tx < static_cast<int>(currentMapMatrix[ty].size())) {
                    if (currentMapMatrix[ty][tx] == '.' && (tx != playerX || ty != playerY)) {
                        // Limita o movimento a celulas diretamente adjacentes a posicao atual
                        if (std::abs(tx - enemy.x) <= 1 && std::abs(ty - enemy.y) <= 1) {
                            possibleMoves.push_back({tx, ty});
                        }
                    }
                }
            }
        }

        possibleMoves.push_back({enemy.x, enemy.y}); // Opcao de permanecer parado

        int choice = RandomGenerator::getInteger(0, static_cast<int>(possibleMoves.size()) - 1);
        int nx = possibleMoves[choice].first;
        int ny = possibleMoves[choice].second;

        if (nx != enemy.x || ny != enemy.y) {
            currentMapMatrix[enemy.y][enemy.x] = '.';
            currentMapMatrix[ny][nx] = enemy.c;
        }
    }
}
