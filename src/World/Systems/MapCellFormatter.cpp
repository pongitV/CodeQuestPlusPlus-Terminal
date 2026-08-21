// [PT-BR] Arquivo: MapCellFormatter.cpp
// [PT-BR] Proposito: Implementacao da formatacao de celulas para 2D, minimapa e modo IDE.
// [EN-US] File: MapCellFormatter.cpp
// [EN-US] Purpose: Implementation of cell formatting for 2D, minimap, and IDE mode.

#include "World/Systems/MapCellFormatter.h"
#include "Core/Utils/Appearance.h"
#include "UI/PerspectiveManager.h"
#include "UI/Renderers/3D/EngineRaycaster/RaycasterWorld.h"
#include <cctype>
#include <algorithm>

std::string MapCellFormatter::extractBaseColorFromRaycaster(char cell, const std::string& mapTitle, bool isForest) {
    std::vector<std::tuple<int, int, int>> emptyLights;
    Pixel3D px = RaycasterWorld::getInternalWallPixel(mapTitle, isForest, 0.0f, 10.0f, cell, 33, 0, 64, 33.0f / 64.0f, 0.0f, emptyLights, 0.0f, 0.0f);
    return "\033[38;2;" + std::to_string(px.r) + ";" + std::to_string(px.g) + ";" + std::to_string(px.b) + "m";
}

std::string MapCellFormatter::formatCell(char cell, int x, int y, const std::string& mapTitle, const std::vector<std::string>& mapMatrix, bool isMinimap) {
    thread_local std::string lastTitle = "";
    thread_local std::string upperTitle = "";
    thread_local bool isKingdom = false, isInterior = false, isForest = false, isVillage = false, isSpawn = false;

    if (lastTitle != mapTitle) {
        lastTitle = mapTitle;
        upperTitle = mapTitle;
        for (char& ch : upperTitle) ch = std::toupper(static_cast<unsigned char>(ch));
        
        isKingdom = (upperTitle.find("Reino") != std::string::npos || upperTitle.find("REINO") != std::string::npos);
        isInterior = (upperTitle.find("LABIRINTO") != std::string::npos || upperTitle.find("CHEFE") != std::string::npos || upperTitle.find("CORACAO") != std::string::npos || upperTitle.find("CAVERNA") != std::string::npos);
        isForest = (upperTitle.find("FLORESTA") != std::string::npos);
        isVillage = (upperTitle.find("VILA") != std::string::npos);
        isSpawn = (upperTitle.find("INICIO") != std::string::npos);
    }
    
    // [PT-BR] --- ESTETICA ENGINE IDE (VISAO TERMINAL) ---
    // [EN-US] --- IDE ENGINE AESTHETICS (TERMINAL VIEW) ---
    bool isEngineIDE = !isMinimap && !PerspectiveManager::getInstance().is3DViewActive();
    if (isEngineIDE) {
        std::string npcs = "GOBFPMSTRCH";
        if (npcs.find(cell) == std::string::npos && cell != ' ' && !RaycasterWorld::isMapLabel(x, y, mapMatrix)) {
            
            if (cell == '.' && (!isInterior || upperTitle.find("CHEFE") != std::string::npos || upperTitle.find("CORACAO") != std::string::npos)) {
                return "\033[38;2;40;40;40m·\033[0m";
            }

            const char syntaxChars[] = "{};/*<>&|!=";
            int idx = (x * 7 + y * 13) % (sizeof(syntaxChars) - 1);
            char ideChar = syntaxChars[idx];
            
            const char* colors[] = {
                "\033[38;2;86;156;214m",   // Blue
                "\033[38;2;197;134;192m",  // Purple
                "\033[38;2;220;220;170m",  // Yellow
                "\033[38;2;78;201;176m",   // Teal
                "\033[38;2;214;157;133m"   // Orange
            };
            int cIdx = (x * 3 + y * 11) % 5;
            
            if (isForest || cell == '*' || cell == '#') {
                return "\033[38;2;96;139;78m" + std::string(1, ideChar) + "\033[0m";
            }
            if (cell == '~') {
                return "\033[38;2;86;156;214m" + std::string(1, '~') + "\033[0m";
            }
            if (cell == '^') {
                return "\033[38;2;197;134;192m" + std::string(1, '^') + "\033[0m";
            }
            
            return std::string(colors[cIdx]) + std::string(1, ideChar) + "\033[0m";
        }
    }

    // Teleporte
    if (cell == '^') return Appearance::color(Color::BOLD, Color::TELEPORT) + "^" + Appearance::color(Color::RESET);
    
    // Agua
    if (cell == '~') return Appearance::colorRGB(50, 150, 255) + "≈" + Appearance::color(Color::RESET);
    
    // [PT-BR] Arvores e Vegetacao
    // [EN-US] Trees and Vegetation
    if (cell == '*') {
        bool isTrunk = false;
        if (y > 0 && mapMatrix[y-1][x] == '*') {
            int countHorizontal = 0;
            if (x > 0 && mapMatrix[y][x-1] == '*') countHorizontal++;
            if (x + 1 < static_cast<int>(mapMatrix[y].length()) && mapMatrix[y][x+1] == '*') countHorizontal++;
            if (countHorizontal <= 1) isTrunk = true;
        }
        if (isTrunk) return Appearance::color(Color::WOOD) + "█" + Appearance::color(Color::RESET);
        return Appearance::color(Color::GREEN) + "▲" + Appearance::color(Color::RESET);
    }
    
    // [PT-BR] Verifica caracteres de rotulo de mapa antes de processar entidades
    // [EN-US] Checks map label characters before processing entities
    if (RaycasterWorld::isMapLabel(x, y, mapMatrix)) {
        return Appearance::color(Color::GRAY) + std::string(1, cell) + Appearance::color(Color::RESET);
    }
    
    // [PT-BR] Renderizacao de Entidades no mapa
    // [EN-US] Entity Rendering on map
    if (isVillage || isSpawn) {
        if (cell == 'G' || cell == 'O') return Appearance::color(Color::BOLD, Color::RED) + std::string(1, cell) + Appearance::color(Color::RESET);
        if (cell == 'B') return Appearance::color(Color::BOLD, Color::CYAN) + "B" + Appearance::color(Color::RESET);
        if (cell == 'F' && x > 0 && mapMatrix[y][x-1] == '{') return Appearance::color(Color::BOLD, Color::YELLOW) + "F" + Appearance::color(Color::RESET);
        if (cell == 'P') return Appearance::color(Color::BOLD, Color::BROWN) + "P" + Appearance::color(Color::RESET);
    } else if (isForest) {
        if (cell == 'S' && (!isInterior || upperTitle.find("CHEFE") != std::string::npos)) return Appearance::color(Color::BOLD, Color::RED) + "S" + Appearance::color(Color::RESET);
        if (cell == 'F' || cell == 'A') return Appearance::color(Color::BOLD, Color::RED) + std::string(1, cell) + Appearance::color(Color::RESET);
        if (cell == 'M') return Appearance::color(Color::BOLD, Color::MAGENTA) + "M" + Appearance::color(Color::RESET);
        if (cell == 'B') return Appearance::color(Color::BOLD, Color::GOLD) + "B" + Appearance::color(Color::RESET);
    } else if (isKingdom) {
        if (cell == 'T') return Appearance::color(Color::BOLD, Color::RED) + "T" + Appearance::color(Color::RESET);
        if (cell == 'G') return Appearance::color(Color::BOLD, Color::YELLOW) + "G" + Appearance::color(Color::RESET);
        if (cell == 'C') return Appearance::color(Color::BOLD, Color::SILVER) + "C" + Appearance::color(Color::RESET);
    }
    
    if (upperTitle == "SALA DO CHEFE" && (cell == 'M' || cell == 'A' || cell == 'H' || cell == 'O' || cell == 'R' || cell == 'G')) {
        return Appearance::color(Color::BOLD, Color::WHITE) + std::string(1, cell) + Appearance::color(Color::RESET);
    }
    
    // [PT-BR] Casas e Estruturas no mapa
    // [EN-US] Houses and Structures on map
    if (!isInterior && !isKingdom) {
        std::string structureColor = extractBaseColorFromRaycaster('|', upperTitle, isForest);
        
        if (cell == '_') return structureColor + "▄" + Appearance::color(Color::RESET);
        if (cell == '|' || cell == '[' || cell == ']') return structureColor + "█" + Appearance::color(Color::RESET);
        std::string structures = "{}/\\<>;=-:+";
        if (structures.find(cell) != std::string::npos) return structureColor + std::string(1, cell) + Appearance::color(Color::RESET);
        
        if (cell == '#') {
            if (isForest) return Appearance::color(Color::GREEN) + "█" + Appearance::color(Color::RESET);
            std::string wallColor = extractBaseColorFromRaycaster('#', upperTitle, isForest);
            return wallColor + "█" + Appearance::color(Color::RESET);
        }
    }
    
    // [PT-BR] Elementos do Reino
    // [EN-US] Kingdom Elements
    if (isKingdom) {
        if (cell == '|') return Appearance::color(Color::WOOD) + "█" + Appearance::color(Color::RESET);
        std::string structures = "_[]{}/\\<>;=-+#";
        if (structures.find(cell) != std::string::npos) {
            std::string kingdomColor = extractBaseColorFromRaycaster(cell, upperTitle, isForest);
            return kingdomColor + "█" + Appearance::color(Color::RESET);
        }
    }
    
    // [PT-BR] Elementos do Labirinto
    // [EN-US] Maze Elements
    if (isInterior) {
        if (upperTitle.find("LABIRINTO") != std::string::npos) {
            std::string mazeColor = extractBaseColorFromRaycaster('|', upperTitle, isForest);
            auto isHWall = [](char c) { return c == '=' || c == '.' || c == '\''; };
            auto isVWall = [](char c) { return c == '|' || c == '+' || c == 'S' || c == 'E'; };

            if (cell == '=') return mazeColor + "─" + Appearance::color(Color::RESET);
            if (cell == '|') {
                bool right = (x + 1 < static_cast<int>(mapMatrix[y].length()) && isHWall(mapMatrix[y][x+1]));
                bool left = (x > 0 && isHWall(mapMatrix[y][x-1]));
                if (right && left) return mazeColor + "┼" + Appearance::color(Color::RESET);
                if (right) return mazeColor + "├" + Appearance::color(Color::RESET);
                if (left) return mazeColor + "┤" + Appearance::color(Color::RESET);
                return mazeColor + "│" + Appearance::color(Color::RESET);
            }
            if (cell == '.') {
                bool right = (x + 1 < static_cast<int>(mapMatrix[y].length()) && isHWall(mapMatrix[y][x+1]));
                bool left = (x > 0 && isHWall(mapMatrix[y][x-1]));
                bool down = (y + 1 < static_cast<int>(mapMatrix.size()) && isVWall(mapMatrix[y+1][x]));
                
                if (left && right && down) return mazeColor + "┬" + Appearance::color(Color::RESET);
                if (right && down) return mazeColor + "┌" + Appearance::color(Color::RESET);
                if (left && down) return mazeColor + "┐" + Appearance::color(Color::RESET);
                if (left && right) return mazeColor + "─" + Appearance::color(Color::RESET);
                return mazeColor + "█" + Appearance::color(Color::RESET);
            }
            if (cell == '\'') {
                bool right = (x + 1 < static_cast<int>(mapMatrix[y].length()) && isHWall(mapMatrix[y][x+1]));
                bool left = (x > 0 && isHWall(mapMatrix[y][x-1]));
                bool up = (y > 0 && isVWall(mapMatrix[y-1][x]));
                
                if (left && right && up) return mazeColor + "┴" + Appearance::color(Color::RESET);
                if (right && up) return mazeColor + "└" + Appearance::color(Color::RESET);
                if (left && up) return mazeColor + "┘" + Appearance::color(Color::RESET);
                if (left && right) return mazeColor + "─" + Appearance::color(Color::RESET);
                return mazeColor + "█" + Appearance::color(Color::RESET);
            }
            if (cell == '+') {
                bool right = (x + 1 < static_cast<int>(mapMatrix[y].length()) && isHWall(mapMatrix[y][x+1]));
                bool left = (x > 0 && isHWall(mapMatrix[y][x-1]));
                bool down = (y + 1 < static_cast<int>(mapMatrix.size()) && isVWall(mapMatrix[y+1][x]));
                bool up = (y > 0 && isVWall(mapMatrix[y-1][x]));
                
                if (left && right && down && up) return mazeColor + "┼" + Appearance::color(Color::RESET);
                if (left && right && down) return mazeColor + "┬" + Appearance::color(Color::RESET);
                if (left && right && up) return mazeColor + "┴" + Appearance::color(Color::RESET);
                if (up && down && left) return mazeColor + "┤" + Appearance::color(Color::RESET);
                if (up && down && right) return mazeColor + "├" + Appearance::color(Color::RESET);
                return mazeColor + "┼" + Appearance::color(Color::RESET);
            }
        }
        else if (upperTitle.find("CAVERNA") != std::string::npos) {
            std::string caveColor = extractBaseColorFromRaycaster('#', upperTitle, isForest);
            if (cell == '#') return caveColor + "█" + Appearance::color(Color::RESET);
            if (cell == '.') {
                if (isMinimap) return "\033[38;2;50;50;50m.\033[0m";
                return "\033[38;2;40;40;40m·\033[0m";
            }
        }
    }
    
    // [PT-BR] Renderizacao do chao e rotulos
    // [EN-US] Ground and label rendering
    if (cell == '.' && (!isInterior || upperTitle.find("CHEFE") != std::string::npos || upperTitle.find("CORACAO") != std::string::npos)) {
        if (isMinimap) return "\033[38;2;50;50;50m.\033[0m";
        return "\033[38;2;40;40;40m·\033[0m";
    }
    
    if (std::isalpha(cell) && cell != ' ' && cell != 'S' && cell != 'F' && cell != 'A' && cell != 'M' && cell != 'B' && cell != 'T' && cell != 'G' && cell != 'C') {
        return Appearance::color(Color::GRAY) + std::string(1, cell) + Appearance::color(Color::RESET);
    }
    
    return std::string(1, cell);
}
