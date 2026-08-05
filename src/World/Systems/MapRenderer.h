#pragma once
#include <vector>
#include <string>
#include <functional>

class MapRenderer {
public:
    static void renderMap(
        const std::vector<std::string>& mapMatrix, int playerPositionX, int playerPositionY, 
        int terminalWidth, int terminalHeight, int initialLine, 
        const std::function<std::string(char, int, int)>& cellFormatter);

    static void calculateCameraVertical(int terminalHeight, int initialLine, int playerPositionY, int mapSize, int& startY, int& endY);
    static void calculateCameraHorizontal(int terminalWidth, int playerPositionX, int mapWidth, int& startX, int& endX);
    static std::string calculateCenteredMargin(int terminalWidth, int textWidth);
};
