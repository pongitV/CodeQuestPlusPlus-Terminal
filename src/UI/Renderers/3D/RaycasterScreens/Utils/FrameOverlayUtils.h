#pragma once

#include <vector>
#include <string>
#include <fstream>
#include "Core/Utils/Appearance.h"

namespace MenuRaycasterUtils {

    inline std::vector<std::string> s_background3DMenu;
    inline std::string s_lastBiomeMenu;

    inline void superimposeNoFrame(int y, int x, const std::string& text,
                                 int fgR, int fgG, int fgB) {
        if (y < 0 || y >= (int)s_background3DMenu.size()) return;
        std::string& line = s_background3DMenu[y];

        size_t cellStart = 0;
        for (int c = 0; c < x && cellStart < line.size(); ++c) {
            cellStart = line.find("\033[48;2;", cellStart);
            if (cellStart == std::string::npos) return;
            cellStart = line.find("\033[0m", cellStart);
            if (cellStart == std::string::npos) return;
            cellStart += 4;
        }

        std::string fgCode = "\033[38;2;" + std::to_string(fgR) + ";" +
                             std::to_string(fgG) + ";" + std::to_string(fgB) + "m";

        for (size_t i = 0; i < text.size(); ) {
            if (cellStart >= line.size()) break;
            size_t bgStart = line.find("\033[48;2;", cellStart);
            if (bgStart == std::string::npos) break;
            size_t bgEnd = line.find('m', bgStart);
            if (bgEnd == std::string::npos) break;
            
            size_t cellEnd = line.find("\033[0m", bgEnd);
            if (cellEnd == std::string::npos) break;

            unsigned char lead = (unsigned char)text[i];
            int charLen;
            if (lead < 0x80) charLen = 1;
            else if (lead < 0xC0) { i++; continue; }
            else if (lead < 0xE0) charLen = 2;
            else if (lead < 0xF0) charLen = 3;
            else charLen = 4;
            if (i + charLen > text.size()) break;

            std::string ch = text.substr(i, charLen);

            if (ch != " ") {
                line.erase(bgEnd + 1, cellEnd - (bgEnd + 1));
                line.insert(bgEnd + 1, fgCode + ch);
            }

            cellStart = line.find("\033[0m", bgEnd + 1);
            if (cellStart == std::string::npos) break;
            cellStart += 4;

            i += charLen;
        }
    }

    inline void paintEffectNoFrame(int y, int x, const std::string& text, int colorR, int colorG, int colorB) {
        if (y < 0 || y >= (int)s_background3DMenu.size()) return;
        std::string& frameLine = s_background3DMenu[y];
        
        for (size_t col = 0; col < text.size(); ++col) {
            char c = text[col];
            if (c == ' ') continue;
            
            int lx = x + (int)col;
            if (lx < 0) continue;
            
            size_t post = 0;
            for (int cell = 0; cell < lx; ++cell) {
                post = frameLine.find("\033[48;2;", post);
                if (post == std::string::npos) { post = frameLine.size(); break; }
                post = frameLine.find("\033[0m", post);
                if (post == std::string::npos) { post = frameLine.size(); break; }
                post += 4;
            }
            if (post >= frameLine.size()) continue;
            
            size_t endOfCell = frameLine.find("\033[0m", post);
            if (endOfCell == std::string::npos) continue;
            
            std::string newBg = "\033[48;2;" + std::to_string(colorR) + ";" +
                                 std::to_string(colorG) + ";" + std::to_string(colorB) + "m ";
            frameLine.replace(post, endOfCell - post, newBg);
        }
    }

    inline void paintSpriteNoFrame(int y, int x, const std::vector<std::string>& sprite,
                                     int colorR, int colorG, int colorB) {
        for (size_t line = 0; line < sprite.size(); ++line) {
            int ly = y + (int)line;
            if (ly < 0 || ly >= (int)s_background3DMenu.size()) continue;
            std::string& frameLine = s_background3DMenu[ly];

            for (size_t col = 0; col < sprite[line].size(); ++col) {
                if (sprite[line][col] == ' ') continue;
                int lx = x + (int)col;
                if (lx < 0) continue;

                size_t post = 0;
                for (int c = 0; c < lx; ++c) {
                    post = frameLine.find("\033[48;2;", post);
                    if (post == std::string::npos) { post = frameLine.size(); break; }
                    post = frameLine.find("\033[0m", post);
                    if (post == std::string::npos) { post = frameLine.size(); break; }
                    post += 4;
                }
                if (post >= frameLine.size()) continue;

                size_t bgEnd = frameLine.find('m', post);
                if (bgEnd == std::string::npos) continue;

                int rMod = colorR, gMod = colorG, bMod = colorB;
                char c = sprite[line][col];
                
                if (c == '@' || c == 'M' || c == 'W' || c == '#' || c == '&' || c == '8') { rMod = colorR * 0.4; gMod = colorG * 0.4; bMod = colorB * 0.4; }
                else if (c == '%' || c == 'O' || c == 'X' || c == 'S' || c == 'Q') { rMod = colorR * 0.6; gMod = colorG * 0.6; bMod = colorB * 0.6; }
                else if (c == '*' || c == '+' || c == 'x' || c == 'o' || c == '=' || c == 'H') { rMod = colorR * 0.8; gMod = colorG * 0.8; bMod = colorB * 0.8; }
                else if (c == '-' || c == '~' || c == ':' || c == ';') { rMod = std::min(255, (int)(colorR * 1.2)); gMod = std::min(255, (int)(colorG * 1.2)); bMod = std::min(255, (int)(colorB * 1.2)); }
                else if (c == '.' || c == ',' || c == '\'') { rMod = std::min(255, (int)(colorR * 1.5)); gMod = std::min(255, (int)(colorG * 1.5)); bMod = std::min(255, (int)(colorB * 1.5)); }
                else if (c == '_' || c == '|' || c == '\\' || c == '/' || c == '(' || c == ')' || c == '[' || c == ']' || c == '{' || c == '}' || c == '<' || c == '>') { rMod = colorR * 0.5; gMod = colorG * 0.5; bMod = colorB * 0.5; }
                
                // Variacao suave horizontal e vertical para quebrar o padrao liso
                int smoothNoise = (int)(std::sin(lx * 0.3f) * 10 + std::sin(ly * 0.3f) * 10);
                rMod = std::max(0, std::min(255, rMod + smoothNoise));
                gMod = std::max(0, std::min(255, gMod + smoothNoise));
                bMod = std::max(0, std::min(255, bMod + smoothNoise));

                std::string newBg = "\033[48;2;" + std::to_string(rMod) + ";" +
                                     std::to_string(gMod) + ";" + std::to_string(bMod) + "m";
                frameLine.replace(post, bgEnd - post + 1, newBg);
            }
        }
    }

    inline void paintHeroNoFrame(int y, int x, const std::vector<std::string>& sprite,
                                     int colorR, int colorG, int colorB) {
        for (size_t line = 0; line < sprite.size(); ++line) {
            int ly = y + (int)line;
            if (ly < 0 || ly >= (int)s_background3DMenu.size()) continue;
            std::string& frameLine = s_background3DMenu[ly];

            for (size_t col = 0; col < sprite[line].size(); ++col) {
                char c = sprite[line][col];
                if (c == ' ') continue;
                int lx = x + (int)col;
                if (lx < 0) continue;

                size_t post = 0;
                for (int cell = 0; cell < lx; ++cell) {
                    post = frameLine.find("\033[48;2;", post);
                    if (post == std::string::npos) { post = frameLine.size(); break; }
                    post = frameLine.find("\033[0m", post);
                    if (post == std::string::npos) { post = frameLine.size(); break; }
                    post += 4;
                }
                if (post >= frameLine.size()) continue;

                size_t endOfCell = frameLine.find("\033[0m", post);
                if (endOfCell == std::string::npos) continue;

                int rMod = colorR, gMod = colorG, bMod = colorB;
                
                // Color Palette by Character for Heroes
                if (c == 'O' || c == 'o') { rMod = 230; gMod = 190; bMod = 160; } // Skin
                else if (c == '#') { rMod = 180; gMod = 185; bMod = 190; } // Metal / Silver Armor
                else if (c == '|' || c == '(' || c == ')') { rMod = 139; gMod = 69; bMod = 19; } // Wood (Bow/Staff)
                else if (c == '/') { rMod = 160; gMod = 82; bMod = 45; } // Wood lighter
                else if (c == '\\') { rMod = 120; gMod = 60; bMod = 15; } // Wood darker
                else if (c == ':') { rMod = 100; gMod = 100; bMod = 100; } // Bow string / Iron
                else if (c == '*') { rMod = 50; gMod = 255; bMod = 255; } // Magic Glow Cyan
                else if (c == '+') { rMod = 255; gMod = 215; bMod = 0; } // Gold accent
                else if (c == '&') { rMod = colorR; gMod = colorG; bMod = colorB; } // Primary Clothing
                else if (c == '%') { rMod = (int)(colorR * 0.6f); gMod = (int)(colorG * 0.6f); bMod = (int)(colorB * 0.6f); } // Secondary Clothing
                else if (c == '@') { rMod = (int)(colorR * 0.4f); gMod = (int)(colorG * 0.4f); bMod = (int)(colorB * 0.4f); } // Dark texture
                
                // Variacao suave horizontal e vertical
                int smoothNoise = (int)(std::sin(lx * 0.4f) * 12 + std::sin(ly * 0.4f) * 12);
                if (c == '*') smoothNoise = (int)(std::sin(lx * 1.5f) * 20); // Magic shimmers differently
                
                rMod = std::max(0, std::min(255, rMod + smoothNoise));
                gMod = std::max(0, std::min(255, gMod + smoothNoise));
                bMod = std::max(0, std::min(255, bMod + smoothNoise));

                std::string newBg = "\033[48;2;" + std::to_string(rMod) + ";" +
                                     std::to_string(gMod) + ";" + std::to_string(bMod) + "m ";
                frameLine.replace(post, endOfCell - post, newBg);
            }
        }
    }

    inline void paintEnemyNoFrame(int y, int x, const std::vector<std::string>& sprite,
                                     int colorR, int colorG, int colorB, bool flashRed = false) {
        for (size_t line = 0; line < sprite.size(); ++line) {
            int ly = y + (int)line;
            if (ly < 0 || ly >= (int)s_background3DMenu.size()) continue;
            std::string& frameLine = s_background3DMenu[ly];

            for (size_t col = 0; col < sprite[line].size(); ++col) {
                char c = sprite[line][col];
                if (c == ' ') continue;
                int lx = x + (int)col;
                if (lx < 0) continue;

                size_t post = 0;
                for (int cell = 0; cell < lx; ++cell) {
                    post = frameLine.find("\033[48;2;", post);
                    if (post == std::string::npos) { post = frameLine.size(); break; }
                    post = frameLine.find("\033[0m", post);
                    if (post == std::string::npos) { post = frameLine.size(); break; }
                    post += 4;
                }
                if (post >= frameLine.size()) continue;

                size_t endOfCell = frameLine.find("\033[0m", post);
                if (endOfCell == std::string::npos) continue;

                int rMod = colorR, gMod = colorG, bMod = colorB;
                
                if (flashRed) {
                    rMod = 255; gMod = 0; bMod = 0;
                } else {
                    if (c == '+') { rMod = 255; gMod = 215; bMod = 0; }
                    else if (c == 'O' || c == 'o') { rMod = 230; gMod = 190; bMod = 160; } 
                    else if (c == '#') { rMod = 180; gMod = 185; bMod = 190; }
                    else if (c == '|') { rMod = 139; gMod = 69; bMod = 19; } 
                    else if (c == '/') { rMod = 160; gMod = 82; bMod = 45; } 
                    else if (c == '\\') { rMod = 120; gMod = 60; bMod = 15; } 
                    else if (c == '&') { rMod = colorR; gMod = colorG; bMod = colorB; } 
                    else if (c == '%') { rMod = (int)(colorR * 0.6f); gMod = (int)(colorG * 0.6f); bMod = (int)(colorB * 0.6f); } 
                    else if (c == '@') { rMod = (int)(colorR * 0.4f); gMod = (int)(colorG * 0.4f); bMod = (int)(colorB * 0.4f); } 
                    
                    int smoothNoise = (int)(std::sin(lx * 0.4f) * 12 + std::sin(ly * 0.4f) * 12);
                    rMod = std::max(0, std::min(255, rMod + smoothNoise));
                    gMod = std::max(0, std::min(255, gMod + smoothNoise));
                    bMod = std::max(0, std::min(255, bMod + smoothNoise));
                }

                std::string newBg = "\033[48;2;" + std::to_string(rMod) + ";" +
                                     std::to_string(gMod) + ";" + std::to_string(bMod) + "m ";
                frameLine.replace(post, endOfCell - post, newBg);
            }
        }
    }

    inline std::string getBgEm(int y, int x) {
        if (y < 0 || y >= (int)s_background3DMenu.size()) return "";
        const std::string& line = s_background3DMenu[y];
        size_t cellStart = 0;
        for (int c = 0; c < x; ++c) {
            size_t found = line.find("\033[48;2;", cellStart);
            if (found == std::string::npos) return "";
            cellStart = line.find("\033[0m", found);
            if (cellStart == std::string::npos) return "";
            cellStart += 4;
        }
        size_t bgStart = line.find("\033[48;2;", cellStart);
        if (bgStart == std::string::npos) return "";
        size_t mPost = line.find('m', bgStart);
        if (mPost == std::string::npos) return "";
        return line.substr(bgStart, mPost - bgStart + 1);
    }

    inline void superimposeText3D(std::ostream& out, const std::string& text, int y, int x, int /*larguraTerminal*/) {
        out << "\033[" << (y + 1) << ";" << (x + 1) << "H" << getBgEm(y, x) << text;
    }

    inline void superimposeTextAbsolute(std::ostream& out, const std::string& text, int y, int x) {
        out << "\033[" << (y + 1) << ";" << (x + 1) << "H\033[48;2;0;0;0m" << text;
    }

    inline void displayOptions3D(std::ostream& out, const std::vector<std::string>& options,
                                int selection, int y, int x,
                                const std::string& colorNormal, const std::string& colorSelection) {
        for (int i = 0; i < (int)options.size(); ++i) {
            std::string prefix = (i == selection) ? "> " : "  ";
            std::string color = (i == selection) ? colorSelection : colorNormal;
            superimposeText3D(out, color + prefix + options[i] + "\033[0m", y + i, x, Appearance::getTerminalWidth());
        }
    }

}
