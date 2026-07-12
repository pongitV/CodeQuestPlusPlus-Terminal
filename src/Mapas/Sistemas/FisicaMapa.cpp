#include "FisicaMapa.h"
#include <algorithm>

void FisicaMapa::aplicarLimitesDeMapa(int& posicaoX, int& posicaoY, const std::vector<std::string>& matrizDoMapa) {
    if (posicaoY < 0) posicaoY = 0; 
    else if (posicaoY >= static_cast<int>(matrizDoMapa.size())) posicaoY = static_cast<int>(matrizDoMapa.size()) - 1;
    
    if (matrizDoMapa.empty()) return;
    
    int maxCols = static_cast<int>(matrizDoMapa[posicaoY].length());
    if (posicaoX < 0) posicaoX = 0; 
    else if (posicaoX >= maxCols) posicaoX = std::max(0, maxCols - 1);
}

#include "../../Core/Utilidades/GeradorAleatorio.h"
#include <cmath>

void FisicaMapa::moverInimigosAleatoriamente(std::vector<std::string>& matrizDoMapaAtual, const std::vector<std::string>& matrizOriginal, const std::string& simbolosInimigos, int jogadorX, int jogadorY) {
    if (simbolosInimigos.empty()) return;

    struct Pos { int x, y; char c; };
    std::vector<Pos> inimigosAtuais;
    
    for (int y = 0; y < static_cast<int>(matrizDoMapaAtual.size()); ++y) {
        for (int x = 0; x < static_cast<int>(matrizDoMapaAtual[y].size()); ++x) {
            if (simbolosInimigos.find(matrizDoMapaAtual[y][x]) != std::string::npos) {
                // Ignora o caractere se ele fizer parte de um marcador de mapa/teleporte (ex: ^S, ^Vila)
                if (x > 0 && matrizDoMapaAtual[y][x-1] == '^') continue;

                inimigosAtuais.push_back({x, y, matrizDoMapaAtual[y][x]});
            }
        }
    }

    for (const auto& inimigo : inimigosAtuais) {
        if (matrizDoMapaAtual[inimigo.y][inimigo.x] != inimigo.c) continue; // Pode ter sido alterado (morto/já movido)
        
        int originX = -1, originY = -1;
        // Encontra o spawn original no raio 3x3
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                int oy = inimigo.y + dy;
                int ox = inimigo.x + dx;
                if (oy >= 0 && oy < static_cast<int>(matrizOriginal.size()) && ox >= 0 && ox < static_cast<int>(matrizOriginal[oy].size())) {
                    if (matrizOriginal[oy][ox] == inimigo.c) {
                        originX = ox;
                        originY = oy;
                        break;
                    }
                }
            }
            if (originX != -1) break;
        }

        if (originX == -1) continue;

        std::vector<std::pair<int, int>> movimentosPossiveis;
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                int ty = originY + dy;
                int tx = originX + dx;
                if (ty >= 0 && ty < static_cast<int>(matrizDoMapaAtual.size()) && tx >= 0 && tx < static_cast<int>(matrizDoMapaAtual[ty].size())) {
                    // Só pode se mover para células vazias, e não pode pisar em cima do jogador
                    if (matrizDoMapaAtual[ty][tx] == '.' && (tx != jogadorX || ty != jogadorY)) {
                        // O inimigo anda 1 de cada vez, então limitamos aos adjacentes atuais dele dentro do raio 3x3 da origem
                        if (std::abs(tx - inimigo.x) <= 1 && std::abs(ty - inimigo.y) <= 1) {
                            movimentosPossiveis.push_back({tx, ty});
                        }
                    }
                }
            }
        }

        movimentosPossiveis.push_back({inimigo.x, inimigo.y}); // Opção de permanecer parado

        int escolha = GeradorAleatorio::obterInteiro(0, static_cast<int>(movimentosPossiveis.size()) - 1);
        int nx = movimentosPossiveis[escolha].first;
        int ny = movimentosPossiveis[escolha].second;

        if (nx != inimigo.x || ny != inimigo.y) {
            matrizDoMapaAtual[inimigo.y][inimigo.x] = '.';
            matrizDoMapaAtual[ny][nx] = inimigo.c;
        }
    }
}
