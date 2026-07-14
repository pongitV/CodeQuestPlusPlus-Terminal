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

    inline void pintarEfeitoNoFrame(int y, int x, const std::string& texto, int corR, int corG, int corB) {
        if (y < 0 || y >= (int)s_fundo3DMenu.size()) return;
        std::string& frameLinha = s_fundo3DMenu[y];
        
        for (size_t col = 0; col < texto.size(); ++col) {
            char c = texto[col];
            if (c == ' ') continue;
            
            int lx = x + (int)col;
            if (lx < 0) continue;
            
            size_t pos = 0;
            for (int cell = 0; cell < lx; ++cell) {
                pos = frameLinha.find("\033[48;2;", pos);
                if (pos == std::string::npos) { pos = frameLinha.size(); break; }
                pos = frameLinha.find("\033[0m", pos);
                if (pos == std::string::npos) { pos = frameLinha.size(); break; }
                pos += 4;
            }
            if (pos >= frameLinha.size()) continue;
            
            size_t endOfCell = frameLinha.find("\033[0m", pos);
            if (endOfCell == std::string::npos) continue;
            
            std::string novoBg = "\033[48;2;" + std::to_string(corR) + ";" +
                                 std::to_string(corG) + ";" + std::to_string(corB) + "m ";
            frameLinha.replace(pos, endOfCell - pos, novoBg);
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

                int rMod = corR, gMod = corG, bMod = corB;
                char c = sprite[linha][col];
                
                if (c == '@' || c == 'M' || c == 'W' || c == '#' || c == '&' || c == '8') { rMod = corR * 0.4; gMod = corG * 0.4; bMod = corB * 0.4; }
                else if (c == '%' || c == 'O' || c == 'X' || c == 'S' || c == 'Q') { rMod = corR * 0.6; gMod = corG * 0.6; bMod = corB * 0.6; }
                else if (c == '*' || c == '+' || c == 'x' || c == 'o' || c == '=' || c == 'H') { rMod = corR * 0.8; gMod = corG * 0.8; bMod = corB * 0.8; }
                else if (c == '-' || c == '~' || c == ':' || c == ';') { rMod = std::min(255, (int)(corR * 1.2)); gMod = std::min(255, (int)(corG * 1.2)); bMod = std::min(255, (int)(corB * 1.2)); }
                else if (c == '.' || c == ',' || c == '\'') { rMod = std::min(255, (int)(corR * 1.5)); gMod = std::min(255, (int)(corG * 1.5)); bMod = std::min(255, (int)(corB * 1.5)); }
                else if (c == '_' || c == '|' || c == '\\' || c == '/' || c == '(' || c == ')' || c == '[' || c == ']' || c == '{' || c == '}' || c == '<' || c == '>') { rMod = corR * 0.5; gMod = corG * 0.5; bMod = corB * 0.5; }
                
                // Variacao suave horizontal e vertical para quebrar o padrao liso
                int smoothNoise = (int)(std::sin(lx * 0.3f) * 10 + std::sin(ly * 0.3f) * 10);
                rMod = std::max(0, std::min(255, rMod + smoothNoise));
                gMod = std::max(0, std::min(255, gMod + smoothNoise));
                bMod = std::max(0, std::min(255, bMod + smoothNoise));

                std::string novoBg = "\033[48;2;" + std::to_string(rMod) + ";" +
                                     std::to_string(gMod) + ";" + std::to_string(bMod) + "m";
                frameLinha.replace(pos, bgEnd - pos + 1, novoBg);
            }
        }
    }

    inline void pintarHeroiNoFrame(int y, int x, const std::vector<std::string>& sprite,
                                     int corR, int corG, int corB) {
        for (size_t linha = 0; linha < sprite.size(); ++linha) {
            int ly = y + (int)linha;
            if (ly < 0 || ly >= (int)s_fundo3DMenu.size()) continue;
            std::string& frameLinha = s_fundo3DMenu[ly];

            for (size_t col = 0; col < sprite[linha].size(); ++col) {
                char c = sprite[linha][col];
                if (c == ' ') continue;
                int lx = x + (int)col;
                if (lx < 0) continue;

                size_t pos = 0;
                for (int cell = 0; cell < lx; ++cell) {
                    pos = frameLinha.find("\033[48;2;", pos);
                    if (pos == std::string::npos) { pos = frameLinha.size(); break; }
                    pos = frameLinha.find("\033[0m", pos);
                    if (pos == std::string::npos) { pos = frameLinha.size(); break; }
                    pos += 4;
                }
                if (pos >= frameLinha.size()) continue;

                size_t endOfCell = frameLinha.find("\033[0m", pos);
                if (endOfCell == std::string::npos) continue;

                int rMod = corR, gMod = corG, bMod = corB;
                
                // Color Palette by Character for Heroes
                if (c == 'O' || c == 'o') { rMod = 230; gMod = 190; bMod = 160; } // Skin
                else if (c == '#') { rMod = 180; gMod = 185; bMod = 190; } // Metal / Silver Armor
                else if (c == '|' || c == '(' || c == ')') { rMod = 139; gMod = 69; bMod = 19; } // Wood (Bow/Staff)
                else if (c == '/') { rMod = 160; gMod = 82; bMod = 45; } // Wood lighter
                else if (c == '\\') { rMod = 120; gMod = 60; bMod = 15; } // Wood darker
                else if (c == ':') { rMod = 100; gMod = 100; bMod = 100; } // Bow string / Iron
                else if (c == '*') { rMod = 50; gMod = 255; bMod = 255; } // Magic Glow Cyan
                else if (c == '+') { rMod = 255; gMod = 215; bMod = 0; } // Gold accent
                else if (c == '&') { rMod = corR; gMod = corG; bMod = corB; } // Primary Clothing
                else if (c == '%') { rMod = (int)(corR * 0.6f); gMod = (int)(corG * 0.6f); bMod = (int)(corB * 0.6f); } // Secondary Clothing
                else if (c == '@') { rMod = (int)(corR * 0.4f); gMod = (int)(corG * 0.4f); bMod = (int)(corB * 0.4f); } // Dark texture
                
                // Variacao suave horizontal e vertical
                int smoothNoise = (int)(std::sin(lx * 0.4f) * 12 + std::sin(ly * 0.4f) * 12);
                if (c == '*') smoothNoise = (int)(std::sin(lx * 1.5f) * 20); // Magic shimmers differently
                
                rMod = std::max(0, std::min(255, rMod + smoothNoise));
                gMod = std::max(0, std::min(255, gMod + smoothNoise));
                bMod = std::max(0, std::min(255, bMod + smoothNoise));

                std::string novoBg = "\033[48;2;" + std::to_string(rMod) + ";" +
                                     std::to_string(gMod) + ";" + std::to_string(bMod) + "m ";
                frameLinha.replace(pos, endOfCell - pos, novoBg);
            }
        }
    }

    inline void pintarInimigoNoFrame(int y, int x, const std::vector<std::string>& sprite,
                                     int corR, int corG, int corB, bool flashRed = false) {
        for (size_t linha = 0; linha < sprite.size(); ++linha) {
            int ly = y + (int)linha;
            if (ly < 0 || ly >= (int)s_fundo3DMenu.size()) continue;
            std::string& frameLinha = s_fundo3DMenu[ly];

            for (size_t col = 0; col < sprite[linha].size(); ++col) {
                char c = sprite[linha][col];
                if (c == ' ') continue;
                int lx = x + (int)col;
                if (lx < 0) continue;

                size_t pos = 0;
                for (int cell = 0; cell < lx; ++cell) {
                    pos = frameLinha.find("\033[48;2;", pos);
                    if (pos == std::string::npos) { pos = frameLinha.size(); break; }
                    pos = frameLinha.find("\033[0m", pos);
                    if (pos == std::string::npos) { pos = frameLinha.size(); break; }
                    pos += 4;
                }
                if (pos >= frameLinha.size()) continue;

                size_t endOfCell = frameLinha.find("\033[0m", pos);
                if (endOfCell == std::string::npos) continue;

                int rMod = corR, gMod = corG, bMod = corB;
                
                if (flashRed) {
                    rMod = 255; gMod = 0; bMod = 0;
                } else {
                    if (c == '+') { rMod = 255; gMod = 215; bMod = 0; }
                    else if (c == 'O' || c == 'o') { rMod = 230; gMod = 190; bMod = 160; } 
                    else if (c == '#') { rMod = 180; gMod = 185; bMod = 190; }
                    else if (c == '|') { rMod = 139; gMod = 69; bMod = 19; } 
                    else if (c == '/') { rMod = 160; gMod = 82; bMod = 45; } 
                    else if (c == '\\') { rMod = 120; gMod = 60; bMod = 15; } 
                    else if (c == '&') { rMod = corR; gMod = corG; bMod = corB; } 
                    else if (c == '%') { rMod = (int)(corR * 0.6f); gMod = (int)(corG * 0.6f); bMod = (int)(corB * 0.6f); } 
                    else if (c == '@') { rMod = (int)(corR * 0.4f); gMod = (int)(corG * 0.4f); bMod = (int)(corB * 0.4f); } 
                    
                    int smoothNoise = (int)(std::sin(lx * 0.4f) * 12 + std::sin(ly * 0.4f) * 12);
                    rMod = std::max(0, std::min(255, rMod + smoothNoise));
                    gMod = std::max(0, std::min(255, gMod + smoothNoise));
                    bMod = std::max(0, std::min(255, bMod + smoothNoise));
                }

                std::string novoBg = "\033[48;2;" + std::to_string(rMod) + ";" +
                                     std::to_string(gMod) + ";" + std::to_string(bMod) + "m ";
                frameLinha.replace(pos, endOfCell - pos, novoBg);
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
