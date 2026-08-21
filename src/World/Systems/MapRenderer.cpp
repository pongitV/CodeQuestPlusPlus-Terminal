#include "World/Systems/MapRenderer.h"
#include "Core/Utils/Appearance.h"
#include <iostream>
#include <algorithm>

namespace {
    void calculateCameraAxis(int maxVisible, int playerPosition, int mapSize, int& start, int& end) {
        start = 0;
        end = mapSize;

        if (end > maxVisible) {
            start = std::max(0, playerPosition - (maxVisible / 2));
            end = start + maxVisible;
            if (end > mapSize) {
                end = mapSize;
                start = std::max(0, end - maxVisible);
            }
        }
    }
}

void MapRenderer::calculateCameraVertical(int terminalHeight, int initialLine, int playerPositionY, int mapSize, int& startY, int& endY) {
    int maxVisibleLines = std::max(5, terminalHeight - initialLine - 4);
    calculateCameraAxis(maxVisibleLines, playerPositionY, mapSize, startY, endY);
}

void MapRenderer::calculateCameraHorizontal(int terminalWidth, int playerPositionX, int mapWidth, int& startX, int& endX) {
    // [PT-BR] Utiliza a largura total disponivel no terminal
    // [EN-US] Uses full available terminal width
    int maxVisibleColumns = std::max(10, terminalWidth);
    calculateCameraAxis(maxVisibleColumns, playerPositionX, mapWidth, startX, endX);
}

std::string MapRenderer::calculateCenteredMargin(int terminalWidth, int textWidth) {
    int spaces = (terminalWidth - textWidth) / 2;
    return std::string(spaces > 0 ? spaces : 0, ' ');
}

void MapRenderer::renderMap(const std::vector<std::string>& mapMatrix, int playerPositionX, int playerPositionY, int terminalWidth, int terminalHeight, int initialLine, const std::function<std::string(char, int, int)>& cellFormatter) {
    int startX, endX;
    calculateCameraHorizontal(terminalWidth, playerPositionX, mapMatrix.empty() ? 0 : static_cast<int>(mapMatrix[0].length()), startX, endX);

    std::string mapLeftMargin = calculateCenteredMargin(terminalWidth, endX - startX);

    std::string controlsText = "W,A,S,D: Mover | V: Visao | I: Inventario | C: Ficha | B: Diario | M: Mapa";
    std::string controlsLeftMargin = calculateCenteredMargin(terminalWidth, controlsText.length());

    Appearance::moveCursor(0, initialLine);

    int startY, endY;
    calculateCameraVertical(terminalHeight, initialLine, playerPositionY, static_cast<int>(mapMatrix.size()), startY, endY);

    std::cout << controlsLeftMargin << Appearance::color(Color::GRAY) << controlsText << Appearance::color(Color::RESET) << "\n\n";

    for (int y = startY; y < endY; y++) {
        std::string lineBeingRendered = mapLeftMargin;
        lineBeingRendered.reserve(mapLeftMargin.size() + (endX - startX) * 10);
        for (int x = startX; x < endX; x++) {
            char c = (x < static_cast<int>(mapMatrix[y].length())) ? mapMatrix[y][x] : ' ';
            lineBeingRendered += cellFormatter(c, x, y);
        }
        std::cout << lineBeingRendered << "\033[K\n";
    }
    std::cout << "\033[J" << std::flush;
}
