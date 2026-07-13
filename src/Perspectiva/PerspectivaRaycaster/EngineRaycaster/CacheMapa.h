#pragma once

#include <string>
#include <vector>
#include <cstddef>

struct MapFlags {
    std::string tituloUpper;
    bool isReino = false;
    bool isCaverna = false;
    bool isLabirinto = false;
    bool isSalaChefe = false;
    bool isSpawn = false;
    bool isTerra = false;
    int temaCeu = 0;
};

namespace CacheMapa {

    inline const MapFlags& obterFlags(const std::string& tituloMapa) {
        static thread_local MapFlags flags;
        static thread_local std::string lastTitulo;
        if (tituloMapa != lastTitulo) {
            lastTitulo = tituloMapa;
            flags = MapFlags();
            flags.tituloUpper = tituloMapa;
            for (auto& c : flags.tituloUpper) c = toupper((unsigned char)c);

            const auto& u = flags.tituloUpper;
            if (u.find("REINO") != std::string::npos) flags.isReino = true;
            if (u.find("CAVERNA") != std::string::npos || u.find("CORACAO") != std::string::npos) flags.isCaverna = true;
            if (u.find("LABIRINTO") != std::string::npos) flags.isLabirinto = true;
            if (u.find("CHEFE") != std::string::npos) flags.isSalaChefe = true;
            if (u.find("INICIO") != std::string::npos) flags.isSpawn = true;
            if (u.find("FLORESTA") != std::string::npos || u.find("BOSQUE") != std::string::npos ||
                u.find("VILA") != std::string::npos || u.find("INICIO") != std::string::npos) flags.isTerra = true;

            if (flags.isCaverna || flags.isLabirinto || flags.isSalaChefe || u.find("CABANA") != std::string::npos) {
                flags.temaCeu = 3; // Static Indoors
            } else {
                flags.temaCeu = 0; // Dynamic Outdoors
            }
        }
        return flags;
    }

    inline char obterNPCProximo(const std::string& titulo, int mapX, int mapY,
                                const std::vector<std::string>& layout) {
        if (layout.empty()) return ' ';

        static thread_local std::string lastTituloCache;
        static thread_local std::vector<std::vector<char>> npcGrid;

        int mapW = layout[0].size();
        int mapH = layout.size();

        if (titulo != lastTituloCache) {
            lastTituloCache = titulo;
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
                if (c == 'B' || c == 'F' || c == 'Q' || c == 'A' ||
                    c == 'I' || c == 'P' || c == 'C' || c == 'T' || c == 'M') {
                    int dist = dx * dx + dy * dy;
                    if (dist < bestDist) { bestDist = dist; bestChar = c; }
                }
            }
        }
        npcGrid[mapY][mapX] = (bestChar == ' ') ? '-' : bestChar;
        return bestChar;
    }

    inline size_t calcularHash(const std::vector<std::string>& matriz) {
        size_t h = 0;
        for (const auto& linha : matriz)
            for (char c : linha)
                h = h * 31 + (unsigned char)c;
        return h;
    }

}
