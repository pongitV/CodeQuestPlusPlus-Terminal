#pragma once

#include <string>
#include <vector>
#include <cstddef>

struct MapFlags {
    std::string upperTitle;
    bool isKingdom = false;
    bool isCave = false;
    bool isLabyrinth = false;
    bool isRoomBoss = false;
    bool isSpawn = false;
    bool isEarth = false;
    bool isForest = false;
    bool isChurch = false;
    bool isHeart = false;
    bool isBridge = false;
    bool isPatio = false;
    int themeSky = 0;
};

namespace CacheMap {

    inline const MapFlags& getFlags(const std::string& titleMap) {
        static thread_local MapFlags flags;
        static thread_local std::string lastTitle;
        if (titleMap != lastTitle) {
            lastTitle = titleMap;
            flags = MapFlags();
            flags.upperTitle = titleMap;
            for (auto& c : flags.upperTitle) c = toupper((unsigned char)c);

            const auto& u = flags.upperTitle;
            if (u.find("REINO") != std::string::npos) flags.isKingdom = true;
            if (u.find("CAVERNA") != std::string::npos || u.find("CORACAO") != std::string::npos) flags.isCave = true;
            if (u.find("LABIRINTO") != std::string::npos) flags.isLabyrinth = true;
            if (u.find("CHEFE") != std::string::npos) flags.isRoomBoss = true;
            if (u.find("INICIO") != std::string::npos) flags.isSpawn = true;
            if (u.find("FLORESTA") != std::string::npos || u.find("BOSQUE") != std::string::npos ||
                u.find("VILA") != std::string::npos || u.find("INICIO") != std::string::npos) flags.isEarth = true;
                
            if (u.find("FLORESTA") != std::string::npos) flags.isForest = true;
            if (u.find("IGREJA") != std::string::npos) flags.isChurch = true;
            if (u.find("CORACAO") != std::string::npos) flags.isHeart = true;
            if (u.find("PONTE") != std::string::npos) flags.isBridge = true;
            if (u.find("PATIO") != std::string::npos) flags.isPatio = true;

            if (flags.isCave || flags.isLabyrinth || flags.isRoomBoss || flags.isChurch || u.find("CABANA") != std::string::npos) {
                flags.themeSky = 3; // Static Indoors
            } else {
                flags.themeSky = 0; // Dynamic Outdoors
            }
        }
        return flags;
    }

    inline char getNPCNext(const std::string& title, int mapX, int mapY,
                                const std::vector<std::string>& layout) {
        if (layout.empty()) return ' ';

        static thread_local std::string lastTitleCache;
        static thread_local std::vector<std::vector<char>> npcGrid;

        int mapW = layout[0].size();
        int mapH = layout.size();

        if (title != lastTitleCache || npcGrid.size() != (size_t)mapH || (mapH > 0 && npcGrid[0].size() != (size_t)mapW)) {
            lastTitleCache = title;
            npcGrid.assign(mapH, std::vector<char>(mapW, 0));
        }

        if (mapY < 0 || mapY >= mapH || mapX < 0 || mapX >= mapW) return ' ';

        if (npcGrid[mapY][mapX] != 0) {
            return npcGrid[mapY][mapX] == '-' ? ' ' : npcGrid[mapY][mapX];
        }

        int bestDist = 12 * 12;
        char bestChar = ' ';

        for (int dy = -12; dy <= 12; ++dy) {
            for (int dx = -12; dx <= 12; ++dx) {
                int ny = mapY + dy, nx = mapX + dx;
                if (ny < 0 || ny >= mapH || nx < 0 || nx >= mapW) continue;
                char c = layout[ny][nx];
                if (c == 'B' || c == 'F' || c == 'Q' || c == 'A' || c == 'N' ||
                    c == 'I' || c == 'P' || c == 'C' || c == 'T' || c == 'M') {
                    int dist = dx * dx + dy * dy;
                    if (dist < bestDist) { bestDist = dist; bestChar = c; }
                }
            }
        }
        npcGrid[mapY][mapX] = (bestChar == ' ') ? '-' : bestChar;
        return bestChar;
    }

    inline size_t calculateHash(const std::vector<std::string>& matrix) {
        size_t h = 0;
        for (const auto& line : matrix)
            for (char c : line)
                h = h * 31 + (unsigned char)c;
        return h;
    }

}
