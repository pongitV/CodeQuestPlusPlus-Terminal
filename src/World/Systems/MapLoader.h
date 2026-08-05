#pragma once
#include <vector>
#include <string>
#include <functional>

class MapLoader {
public:
    static void enterSubMap(
        std::vector<std::string>& currentMapMatrix, std::vector<std::string>& savedMainMapMatrix,
        int& savedPositionXBeforeEnteringSubMap, int& savedPositionYBeforeEnteringSubMap,
        int& playerPositionX, int& playerPositionY, bool& playerIsInsideSubMap,
        std::string& currentMapTitle, std::vector<std::string>& savedSubMapMatrix, bool& subMapAlreadyVisited,
        const std::vector<std::string>& generatedSubMapMatrix, int initialPositionXInSubMap, int initialPositionYInSubMap, const std::string& subMapTitle, const std::function<void()>& restoreScreen);

    static void standardizeMapSize(std::vector<std::string>& mapMatrix);
};
