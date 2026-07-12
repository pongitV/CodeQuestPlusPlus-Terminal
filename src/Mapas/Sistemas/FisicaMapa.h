#pragma once
#include <vector>
#include <string>

class FisicaMapa {
public:
    // Garante que a posição não saia dos limites da matriz do mapa
    static void aplicarLimitesDeMapa(int& posicaoX, int& posicaoY, const std::vector<std::string>& matrizDoMapa);

    // Movimenta inimigos na matriz respeitando limites e posições originais
    static void moverInimigosAleatoriamente(std::vector<std::string>& matrizDoMapaAtual, const std::vector<std::string>& matrizOriginal, const std::string& simbolosInimigos, int jogadorX, int jogadorY);
};
