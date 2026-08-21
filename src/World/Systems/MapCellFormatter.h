// [PT-BR] Arquivo: MapCellFormatter.h
// [PT-BR] Proposito: Formatacao de celulas para renderizacao de mapas (2D, minimapa e modo IDE).
// [EN-US] File: MapCellFormatter.h
// [EN-US] Purpose: Cell formatting for map rendering (2D, minimap, and IDE mode).

#pragma once

#include <string>
#include <vector>

class MapCellFormatter {
public:
    static std::string extractBaseColorFromRaycaster(char cell, const std::string& mapTitle, bool isForest);
    static std::string formatCell(char cell, int x, int y, const std::string& mapTitle, const std::vector<std::string>& mapMatrix, bool isMinimap = false);
};
