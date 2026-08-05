#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <chrono>

namespace MenuRaycasterUtils {

    struct GroupColor {
        std::string characters;
        int r, g, b;
    };

    inline double getShineByCharacter(char c) {
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

    inline int findBase(const std::vector<GroupColor>& groups, char c) {
        for (size_t i = 0; i < groups.size(); ++i) {
            if (groups[i].characters.find(c) != std::string::npos) return (int)i;
        }
        return -1;
    }

    inline void printArtPixelated(std::ostream& buffer, const std::vector<std::string>& art,
                                     const std::vector<GroupColor>& groups,
                                     int offsetX = 0, int offsetY = 0,
                                     double fadeShine = 1.0)
    {
        if (art.empty()) return;

        int height = (int)art.size();
        int width = 0;
        for (const auto& line : art) width = std::max(width, (int)line.size());

        auto isTransparent = [&](int y, int x) -> bool {
            if (y < 0 || y >= height || x < 0 || x >= (int)art[y].size()) return true;
            return art[y][x] == ' ';
        };

        for (int y = 0; y < height; ++y) {
            const std::string& line = art[y];
            for (int x = 0; x < (int)line.size(); ++x) {
                char c = line[x];
                if (c == ' ') continue;

                int idx = findBase(groups, c);
                if (idx < 0) continue;

                double shine = getShineByCharacter(c);
                if (shine == 0.0) shine = 0.8;

                bool isEdge = false;
                for (int dy = -1; dy <= 1 && !isEdge; ++dy) {
                    for (int dx = -1; dx <= 1 && !isEdge; ++dx) {
                        if (dx == 0 && dy == 0) continue;
                        if (isTransparent(y + dy, x + dx)) isEdge = true;
                    }
                }

                int r = std::min(255, (int)(groups[idx].r * shine * fadeShine));
                int g = std::min(255, (int)(groups[idx].g * shine * fadeShine));
                int b = std::min(255, (int)(groups[idx].b * shine * fadeShine));
                if (isEdge) {
                    r = r * 2 / 3; g = g * 2 / 3; b = b * 2 / 3;
                }

                int absX = offsetX + x;
                int absY = offsetY + y;
                int smoothNoise = (int)(std::sin(absX * 0.4f) * 12 + std::sin(absY * 0.4f) * 12);
                
                if (c == '@' || c == '*') {
                    long long ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::system_clock::now().time_since_epoch()).count();
                    smoothNoise = (int)(std::sin(absX * 0.5f + ms * 0.01f) * 25 + std::sin(absY * 0.5f - ms * 0.005f) * 25);
                } else if (c == '%') {
                    long long ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::system_clock::now().time_since_epoch()).count();
                    int scanline = (int)(std::sin(absY * 1.5f + ms * 0.005f) * 15);
                    unsigned int hash = (unsigned int)(x * 374761393U + y * 668265263U + ms / 50 * 1274126177U);
                    int staticNoise = (int)((hash % 100) / 100.0f * 20);
                    smoothNoise = scanline + staticNoise;
                }

                r = std::max(0, std::min(255, r + smoothNoise));
                g = std::max(0, std::min(255, g + smoothNoise));
                b = std::max(0, std::min(255, b + smoothNoise));

                buffer << "\033[" << (offsetY + y + 1) << ";" << (offsetX + x + 1) << "H"
                       << "\033[38;2;" << r << ";" << g << ";" << b << "m\u2588\033[0m";
            }
        }
    }

    inline void printArtPixelatedSimple(std::ostream& buffer, const std::vector<std::string>& art,
                                            int baseR, int baseG, int baseB,
                                            int offsetX = 0, int offsetY = 0,
                                            double fadeShine = 1.0)
    {
        std::string everyoneChars;
        for (char c = 33; c <= 126; ++c) {
            if (c != ' ') everyoneChars += c;
        }
        std::vector<GroupColor> groups;
        groups.push_back({everyoneChars, baseR, baseG, baseB});
        printArtPixelated(buffer, art, groups, offsetX, offsetY, fadeShine);
    }

    inline void printArtPixelatedCentralized(std::ostream& buffer, const std::vector<std::string>& art,
                                                  const std::vector<GroupColor>& groups,
                                                  int widthConsole, int y,
                                                  double fadeShine = 1.0)
    {
        if (art.empty()) return;
        int artWidth = 0;
        for (const auto& line : art) artWidth = std::max(artWidth, (int)line.size());
        int offsetX = std::max(0, (widthConsole - artWidth) / 2);
        printArtPixelated(buffer, art, groups, offsetX, y, fadeShine);
    }

}
