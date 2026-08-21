#pragma once

#include <string>
#include <algorithm>
#include <ostream>
#include "Core/Utils/Appearance.h"

/*
 * BaseMenuScreen — utilitarios compartilhados de layout para as telas de menu.
 * Centraliza calculos de posicionamento horizontal para evitar repeticao
 * em cada tela concreta (Raycaster, IDE, futuras perspectivas).
 */
class ScreenBaseMenu {
public:
    // [PT-BR] Retorna o offset X para centralizar horizontalmente um bloco de texto dentro do terminal
    // [EN-US] Returns X offset to horizontally center a text block inside terminal
    static int calculateOffsetCentral(int textLength, int widthConsole) {
        return std::max(0, (widthConsole - textLength) / 2);
    }

    // [PT-BR] Sobrecarga: aceita string diretamente e ignora codigos de cor ANSI no calculo
    // [EN-US] Overload: accepts string directly and ignores ANSI color codes in calculation
    static int calculateOffsetCentral(const std::string& text, int widthConsole) {
        return calculateOffsetCentral(Appearance::getVisualLength(text), widthConsole);
    }

    // [PT-BR] Desenha uma caixa preta com bordas brancas usando posicionamento ANSI com limites seguros
    // [EN-US] Draws a black box with white borders using ANSI positioning with safe clamps
    static void drawBoxBlack(std::ostream& out, int y, int x, int width, int height) {
        int termW = Appearance::getTerminalWidth();
        int termH = Appearance::getTerminalHeight();
        
        if (x < 0) x = 0;
        if (y < 0) y = 0;
        if (x >= termW || y >= termH) return;
        if (x + width > termW) width = termW - x;
        if (y + height > termH) height = termH - y;
        if (width < 3 || height < 3) return;

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

        // [PT-BR] Topo da caixa
        // [EN-US] Top of box
        out << "\033[" << (y + 1) << ";" << (x + 1) << "H" << colorEdge << colorBackground << edgeTop << reset;
        // [PT-BR] Linhas intermediarias
        // [EN-US] Middle lines
        for (int i = 1; i < height - 1; ++i) {
            out << "\033[" << (y + i + 1) << ";" << (x + 1) << "H" 
                << colorEdge << colorBackground << "\u2502" 
                << middle 
                << "\u2502" << reset;
        }
        // [PT-BR] Base da caixa
        // [EN-US] Bottom of box
        out << "\033[" << (y + height) << ";" << (x + 1) << "H" << colorEdge << colorBackground << edgeBot << reset;
    }
};
