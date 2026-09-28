#pragma once

#include <string>
#include <vector>

class MapCellFormatter {
public:
    static std::string extractBaseColorFromRaycaster(char cell, const std::string& mapTitle, bool isForest);
    static std::string formatCell(char cell, int x, int y, const std::string& mapTitle, const std::vector<std::string>& mapMatrix, bool isMinimap = false);
};
