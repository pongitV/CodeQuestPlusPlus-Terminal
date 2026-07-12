#include "RenderizadorMapa.h"
#include "../../Core/Utilidades/Aparencia.h"
#include <iostream>
#include <algorithm>

namespace {
    void calcularCameraAxis(int maxVisivel, int posicaoJogador, int tamanhoMapa, int& start, int& end) {
        start = 0;
        end = tamanhoMapa;

        if (end > maxVisivel) {
            start = std::max(0, posicaoJogador - (maxVisivel / 2));
            end = start + maxVisivel;
            if (end > tamanhoMapa) {
                end = tamanhoMapa;
                start = std::max(0, end - maxVisivel);
            }
        }
    }
}

void RenderizadorMapa::calcularCameraVertical(int alturaDoTerminal, int linhaInicial, int posicaoYDoJogador, int tamanhoDoMapa, int& startY, int& endY) {
    int maxLinhasVisiveis = std::max(5, alturaDoTerminal - linhaInicial - 4);
    calcularCameraAxis(maxLinhasVisiveis, posicaoYDoJogador, tamanhoDoMapa, startY, endY);
}

void RenderizadorMapa::calcularCameraHorizontal(int larguraDoTerminal, int posicaoXDoJogador, int larguraDoMapa, int& startX, int& endX) {
    int maxColunasVisiveis = std::max(10, larguraDoTerminal); // Usa a largura total do terminal
    calcularCameraAxis(maxColunasVisiveis, posicaoXDoJogador, larguraDoMapa, startX, endX);
}

std::string RenderizadorMapa::calcularMargemCentralizada(int larguraDoTerminal, int larguraDoTexto) {
    int espacos = (larguraDoTerminal - larguraDoTexto) / 2;
    return std::string(espacos > 0 ? espacos : 0, ' ');
}

void RenderizadorMapa::renderizarMapa(const std::vector<std::string>& matrizDoMapa, int posicaoXDoJogador, int posicaoYDoJogador, int larguraDoTerminal, int alturaDoTerminal, int linhaInicial, const std::function<std::string(char, int, int)>& formatadorCelula) {
    int startX, endX;
    calcularCameraHorizontal(larguraDoTerminal, posicaoXDoJogador, matrizDoMapa.empty() ? 0 : static_cast<int>(matrizDoMapa[0].length()), startX, endX);

    std::string margemEsquerdaDoMapa = calcularMargemCentralizada(larguraDoTerminal, endX - startX);

    std::string textoDeControles = "W,A,S,D: Mover | V: Visao | I: Inventario | C: Ficha | B: Diario | M: Mapa";
    std::string margemEsquerdaControles = calcularMargemCentralizada(larguraDoTerminal, textoDeControles.length());

    Aparencia::moverCursor(0, linhaInicial);

    int startY, endY;
    calcularCameraVertical(alturaDoTerminal, linhaInicial, posicaoYDoJogador, static_cast<int>(matrizDoMapa.size()), startY, endY);

    std::cout << margemEsquerdaControles << Aparencia::cor(Cor::CINZA) << textoDeControles << Aparencia::cor(Cor::RESET) << "\n\n";

    for (int y = startY; y < endY; y++) {
        std::string linhaSendoRenderizada = margemEsquerdaDoMapa;
        linhaSendoRenderizada.reserve(margemEsquerdaDoMapa.size() + (endX - startX) * 10);
        for (int x = startX; x < endX; x++) {
            char c = (x < static_cast<int>(matrizDoMapa[y].length())) ? matrizDoMapa[y][x] : ' ';
            linhaSendoRenderizada += formatadorCelula(c, x, y);
        }
        std::cout << linhaSendoRenderizada << "\033[K\n";
    }
    std::cout << "\033[J" << std::flush;
}
