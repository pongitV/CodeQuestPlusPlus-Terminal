#pragma once

#include <vector>
#include <string>
#include <sstream>
#include "../../../../Core/Utilidades/Aparencia.h"

namespace MenuRaycasterUtils {

    inline std::vector<std::string> s_fundo3DMenu;
    inline std::string s_ultimoBiomaMenu;

    inline void sobreporNoFrame(int y, int x, const std::string& texto,
                                 int fgR, int fgG, int fgB) {
        if (y < 0 || y >= (int)s_fundo3DMenu.size()) return;
        std::string& linha = s_fundo3DMenu[y];

        size_t cellStart = 0;
        for (int c = 0; c < x && cellStart < linha.size(); ++c) {
            cellStart = linha.find("\033[48;2;", cellStart);
            if (cellStart == std::string::npos) return;
            cellStart = linha.find("\033[0m", cellStart);
            if (cellStart == std::string::npos) return;
            cellStart += 4;
        }

        std::string fgCode = "\033[38;2;" + std::to_string(fgR) + ";" +
                             std::to_string(fgG) + ";" + std::to_string(fgB) + "m";

        for (size_t i = 0; i < texto.size(); ) {
            if (cellStart >= linha.size()) break;
            size_t bgStart = linha.find("\033[48;2;", cellStart);
            if (bgStart == std::string::npos) break;
            size_t bgEnd = linha.find('m', bgStart);
            if (bgEnd == std::string::npos) break;
            
            size_t cellEnd = linha.find("\033[0m", bgEnd);
            if (cellEnd == std::string::npos) break;

            unsigned char lead = (unsigned char)texto[i];
            int charLen;
            if (lead < 0x80) charLen = 1;
            else if (lead < 0xC0) { i++; continue; }
            else if (lead < 0xE0) charLen = 2;
            else if (lead < 0xF0) charLen = 3;
            else charLen = 4;
            if (i + charLen > texto.size()) break;

            std::string ch = texto.substr(i, charLen);

            if (ch != " ") {
                linha.erase(bgEnd + 1, cellEnd - (bgEnd + 1));
                linha.insert(bgEnd + 1, fgCode + ch);
            }

            cellStart = linha.find("\033[0m", bgEnd + 1);
            if (cellStart == std::string::npos) break;
            cellStart += 4;

            i += charLen;
        }
    }

    inline void pintarSpriteNoFrame(int y, int x, const std::vector<std::string>& sprite,
                                     int corR, int corG, int corB) {
        for (size_t linha = 0; linha < sprite.size(); ++linha) {
            int ly = y + (int)linha;
            if (ly < 0 || ly >= (int)s_fundo3DMenu.size()) continue;
            std::string& frameLinha = s_fundo3DMenu[ly];

            for (size_t col = 0; col < sprite[linha].size(); ++col) {
                if (sprite[linha][col] == ' ') continue;
                int lx = x + (int)col;
                if (lx < 0) continue;

                size_t pos = 0;
                for (int c = 0; c < lx; ++c) {
                    pos = frameLinha.find("\033[48;2;", pos);
                    if (pos == std::string::npos) { pos = frameLinha.size(); break; }
                    pos = frameLinha.find("\033[0m", pos);
                    if (pos == std::string::npos) { pos = frameLinha.size(); break; }
                    pos += 4;
                }
                if (pos >= frameLinha.size()) continue;

                size_t bgEnd = frameLinha.find('m', pos);
                if (bgEnd == std::string::npos) continue;

                std::string novoBg = "\033[48;2;" + std::to_string(corR) + ";" +
                                     std::to_string(corG) + ";" + std::to_string(corB) + "m";
                frameLinha.replace(pos, bgEnd - pos + 1, novoBg);
            }
        }
    }

    inline std::string obterBgEm(int y, int x) {
        if (y < 0 || y >= (int)s_fundo3DMenu.size()) return "";
        const std::string& linha = s_fundo3DMenu[y];
        size_t cellStart = 0;
        for (int c = 0; c < x; ++c) {
            size_t found = linha.find("\033[48;2;", cellStart);
            if (found == std::string::npos) return "";
            cellStart = linha.find("\033[0m", found);
            if (cellStart == std::string::npos) return "";
            cellStart += 4;
        }
        size_t bgStart = linha.find("\033[48;2;", cellStart);
        if (bgStart == std::string::npos) return "";
        size_t mPos = linha.find('m', bgStart);
        if (mPos == std::string::npos) return "";
        return linha.substr(bgStart, mPos - bgStart + 1);
    }

    inline void sobreporTexto3D(std::ostream& out, const std::string& texto, int y, int x, int /*larguraTerminal*/) {
        out << "\033[" << (y + 1) << ";" << (x + 1) << "H" << obterBgEm(y, x) << texto;
    }

    inline void sobreporTextoAbsoluto(std::ostream& out, const std::string& texto, int y, int x) {
        out << "\033[" << (y + 1) << ";" << (x + 1) << "H\033[48;2;0;0;0m" << texto;
    }

    inline void exibirOpcoes3D(std::ostream& out, const std::vector<std::string>& opcoes,
                                int selecao, int y, int x,
                                const std::string& corNormal, const std::string& corSelecao) {
        for (int i = 0; i < (int)opcoes.size(); ++i) {
            std::string prefixo = (i == selecao) ? "> " : "  ";
            std::string cor = (i == selecao) ? corSelecao : corNormal;
            sobreporTexto3D(out, cor + prefixo + opcoes[i] + "\033[0m", y + i, x, Aparencia::obterLarguraTerminal());
        }
    }

}
