#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <cstdint>

namespace MenuRaycasterUtils {

    struct GrupoCor {
        std::string caracteres;
        int r, g, b;
    };

    inline double obterBrilhoPorCaractere(char c) {
        switch (c) {
            case '@': case 'M': case 'W': case '#': case '&': case '8': return 0.4;
            case '%': case 'O': case 'X': case 'S': case 'Q': return 0.6;
            case '*': case '+': case 'x': case 'o': case '=': case 'H': return 0.8;
            case '-': case '~': case ':': case ';': return 1.2;
            case '.': case ',': case '\'': return 1.5;
            case '_': case '|': case '\\': case '/': case '(': case ')':
            case '[': case ']': case '{': case '}': case '<': case '>': return 0.5;
            default: return 0.0;
        }
    }

    inline int encontrarBase(const std::vector<GrupoCor>& grupos, char c) {
        for (size_t i = 0; i < grupos.size(); ++i) {
            if (grupos[i].caracteres.find(c) != std::string::npos) return (int)i;
        }
        return -1;
    }

    inline void imprimirArtePixelada(std::ostream& buffer, const std::vector<std::string>& arte,
                                     const std::vector<GrupoCor>& grupos,
                                     int offsetX = 0, int offsetY = 0,
                                     double fadeBrilho = 1.0)
    {
        if (arte.empty()) return;

        int altura = (int)arte.size();
        int largura = 0;
        for (const auto& linha : arte) largura = std::max(largura, (int)linha.size());

        auto isTransparente = [&](int y, int x) -> bool {
            if (y < 0 || y >= altura || x < 0 || x >= (int)arte[y].size()) return true;
            return arte[y][x] == ' ';
        };

        for (int y = 0; y < altura; ++y) {
            const std::string& linha = arte[y];
            for (int x = 0; x < (int)linha.size(); ++x) {
                char c = linha[x];
                if (c == ' ') continue;

                int idx = encontrarBase(grupos, c);
                if (idx < 0) continue;

                double brilho = obterBrilhoPorCaractere(c);
                if (brilho == 0.0) brilho = 0.8;

                bool isEdge = false;
                for (int dy = -1; dy <= 1 && !isEdge; ++dy) {
                    for (int dx = -1; dx <= 1 && !isEdge; ++dx) {
                        if (dx == 0 && dy == 0) continue;
                        if (isTransparente(y + dy, x + dx)) isEdge = true;
                    }
                }

                int r = std::min(255, (int)(grupos[idx].r * brilho * fadeBrilho));
                int g = std::min(255, (int)(grupos[idx].g * brilho * fadeBrilho));
                int b = std::min(255, (int)(grupos[idx].b * brilho * fadeBrilho));
                if (isEdge) {
                    r = r * 2 / 3; g = g * 2 / 3; b = b * 2 / 3;
                }
                buffer << "\033[" << (offsetY + y + 1) << ";" << (offsetX + x + 1) << "H"
                       << "\033[38;2;" << r << ";" << g << ";" << b << "m\u2588\033[0m";
            }
        }
    }

    inline void imprimirArtePixeladaSimples(std::ostream& buffer, const std::vector<std::string>& arte,
                                            int baseR, int baseG, int baseB,
                                            int offsetX = 0, int offsetY = 0,
                                            double fadeBrilho = 1.0)
    {
        std::string todosChars;
        for (char c = 33; c <= 126; ++c) {
            if (c != ' ') todosChars += c;
        }
        std::vector<GrupoCor> grupos;
        grupos.push_back({todosChars, baseR, baseG, baseB});
        imprimirArtePixelada(buffer, arte, grupos, offsetX, offsetY, fadeBrilho);
    }

    inline void imprimirArtePixeladaCentralizada(std::ostream& buffer, const std::vector<std::string>& arte,
                                                  const std::vector<GrupoCor>& grupos,
                                                  int larguraConsole, int y,
                                                  double fadeBrilho = 1.0)
    {
        if (arte.empty()) return;
        int larguraArte = 0;
        for (const auto& linha : arte) larguraArte = std::max(larguraArte, (int)linha.size());
        int offsetX = std::max(0, (larguraConsole - larguraArte) / 2);
        imprimirArtePixelada(buffer, arte, grupos, offsetX, y, fadeBrilho);
    }

}
