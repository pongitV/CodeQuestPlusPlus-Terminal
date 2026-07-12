#pragma once
#include <vector>
#include <string>
#include <functional>

class RenderizadorMapa {
public:
    static void renderizarMapa(
        const std::vector<std::string>& matrizDoMapa, int posicaoXDoJogador, int posicaoYDoJogador, 
        int larguraDoTerminal, int alturaDoTerminal, int linhaInicial, 
        const std::function<std::string(char, int, int)>& formatadorCelula);

    static void calcularCameraVertical(int alturaDoTerminal, int linhaInicial, int posicaoYDoJogador, int tamanhoDoMapa, int& startY, int& endY);
    static void calcularCameraHorizontal(int larguraDoTerminal, int posicaoXDoJogador, int larguraDoMapa, int& startX, int& endX);
    static std::string calcularMargemCentralizada(int larguraDoTerminal, int larguraDoTexto);
};
