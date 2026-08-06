#pragma once

#include <string>
#include <algorithm>
#include <ostream>
#include "Core/Utils/Appearance.h"

/*
 * TelaBaseMenu — utilitarios compartilhados de layout para as telas de menu.
 * Centraliza calculos de posicionamento horizontal para evitar repeticao
 * em cada tela concreta (Raycaster, IDE, futuras perspectivas).
 */
class ScreenBaseMenu {
public:
    /*
     * Retorna o offset X para centralizar horizontalmente um bloco de
     * 'comprimentoTexto' caracteres dentro de um terminal de 'larguraConsole' colunas.
     * Nunca retorna valor negativo.
     */
    static int calculateOffsetCentral(int textLength, int widthConsole) {
        return std::max(0, (widthConsole - textLength) / 2);
    }

    /*
     * Sobrecarga conveniente: aceita a string diretamente e automaticamente
     * ignora codigos de cor ANSI no calculo da largura visual.
     */
    static int calculateOffsetCentral(const std::string& text, int widthConsole) {
        return calculateOffsetCentral(Appearance::getVisualLength(text), widthConsole);
    }

    // Desenha uma caixa preta com bordas brancas usando posicionamento ANSI
    static void drawBoxBlack(std::ostream& out, int y, int x, int width, int height) {
        std::string colorEdge = "\033[38;2;255;255;255m"; // Branco
        std::string colorBackground = "\033[48;2;0;0;0m"; // Preto
        std::string reset = "\033[0m";

        std::string edgeTop = "\u250C";
        for (int i = 0; i < width - 2; ++i) edgeTop += "\u2500";
        edgeTop += "\u2510";

        std::string edgeBot = "\u2514";
        for (int i = 0; i < width - 2; ++i) edgeBot += "\u2500";
        edgeBot += "\u2518";

        std::string middle = "";
        for (int i = 0; i < width - 2; ++i) middle += " ";

        // Topo
        out << "\033[" << (y + 1) << ";" << (x + 1) << "H" << colorEdge << colorBackground << edgeTop << reset;
        // Linhas intermediarias
        for (int i = 1; i < height - 1; ++i) {
            out << "\033[" << (y + i + 1) << ";" << (x + 1) << "H" 
                << colorEdge << colorBackground << "\u2502" 
                << middle 
                << "\u2502" << reset;
        }
        // Base
        out << "\033[" << (y + height) << ";" << (x + 1) << "H" << colorEdge << colorBackground << edgeBot << reset;
    }
};
