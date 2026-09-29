#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <ostream>
#include <iostream>
#include "Core/Utils/Appearance.h"

// Utilitarios compartilhados de layout para as telas de menu.
// Centraliza calculos de posicionamento horizontal para evitar duplicacao
// entre as implementacoes de telas (Raycaster, IDE e outras perspectivas).
class BaseMenuScreen {
public:
    // Retorna o offset X para centralizar horizontalmente um bloco de texto dentro do terminal
    static int calculateCenterOffset(int textLength, int widthConsole) {
        return std::max(0, (widthConsole - textLength) / 2);
    }
    static inline int calculateOffsetCentral(int textLength, int widthConsole) {
        return calculateCenterOffset(textLength, widthConsole);
    }

    // Sobrecarga: aceita string diretamente e ignora codigos de cor ANSI no calculo
    static int calculateCenterOffset(const std::string& text, int widthConsole) {
        return calculateCenterOffset(Appearance::getVisualLength(text), widthConsole);
    }
    static inline int calculateOffsetCentral(const std::string& text, int widthConsole) {
        return calculateCenterOffset(text, widthConsole);
    }

    // Desenha uma caixa preta com bordas brancas usando posicionamento ANSI com limites seguros
    static void drawBlackBox(std::ostream& out, int y, int x, int width, int height) {
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

        // Topo da caixa
        out << "\033[" << (y + 1) << ";" << (x + 1) << "H" << colorEdge << colorBackground << edgeTop << reset;
        // Linhas intermediarias
        for (int i = 1; i < height - 1; ++i) {
            out << "\033[" << (y + i + 1) << ";" << (x + 1) << "H" 
                << colorEdge << colorBackground << "\u2502" 
                << middle 
                << "\u2502" << reset;
        }
        // Base da caixa
        out << "\033[" << (y + height) << ";" << (x + 1) << "H" << colorEdge << colorBackground << edgeBot << reset;
    }
    static inline void drawBoxBlack(std::ostream& out, int y, int x, int width, int height) {
        drawBlackBox(out, y, x, width, height);
    }

    // Renderiza logotipo ASCII flutuante centralizado (usado em telas como Inventario, Diario, Atributos)
    static void displayFloatingLogo(const std::vector<std::string>& logoLines, int startY, Color titleColor, const std::string& compactFallbackTitle = "") {
        int widthConsole = Appearance::getTerminalWidth();
        int logoHeight = static_cast<int>(logoLines.size());

        int compVisualLogo = 0;
        for (const auto& line : logoLines) {
            int comp = Appearance::getVisualLength(line);
            if (comp > compVisualLogo) compVisualLogo = comp;
        }

        int logoY = startY > 0 ? (startY - 1 - logoHeight) : 1;
        if (logoY < 0) logoY = 0;
        int logoX = (widthConsole - compVisualLogo) / 2;
        if (logoX < 0) logoX = 0;

        if (widthConsole >= compVisualLogo && (startY >= logoHeight + 1 || logoY == 0)) {
            std::string bgDark = "\033[48;2;20;20;20m";
            std::string colorTitle = Appearance::color(titleColor);
            std::string reset = "\033[0m";

            for (int i = 0; i < logoHeight; ++i) {
                Appearance::moveCursor(logoX, logoY + i);
                std::cout << bgDark << colorTitle << logoLines[i] << reset;
            }
            std::cout << std::flush;
        } else if (startY >= 2 && !compactFallbackTitle.empty()) {
            int compCompact = Appearance::getVisualLength(compactFallbackTitle);
            int cx = std::max(0, (widthConsole - compCompact) / 2);
            int cy = std::max(0, startY - 1);
            Appearance::moveCursor(cx, cy);
            std::cout << Appearance::color(titleColor) << "\033[48;2;25;25;25m" << compactFallbackTitle << "\033[0m" << std::flush;
        }
    }
};

// Apelido para compatibilidade retroativa
using ScreenBaseMenu = BaseMenuScreen;
