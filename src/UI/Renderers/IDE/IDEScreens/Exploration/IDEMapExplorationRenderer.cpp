#include "UI/Renderers/IDE/IDEScreens/Exploration/IDEMapExplorationRenderer.h"
#include "UI/Renderers/IDE/EngineIDE/IDEInspector.h"
#include "UI/Renderers/IDE/IDETheme.h"
#include "Core/Utils/Appearance.h"
#include <iostream>
#include <algorithm>
#include <sstream>

void IDEMapExplorationRenderer::render(
    const std::vector<std::string>& mapMatrix,
    int playerPositionX,
    int playerPositionY,
    int terminalWidth,
    int terminalHeight,
    int /*linhaInicial*/,
    const std::function<std::string(char, int, int)>& cellFormatter,
    Character* currentPlayer,
    const std::string& mapTitle
) {
    // 1. Layout de Abas no Topo
    std::string safeTitle = mapTitle.empty() ? "MapMain.cpp" : mapTitle + ".cpp";
    for (char& c : safeTitle) if (c == ' ') c = '_';

    std::vector<std::string> tabs = {
        safeTitle,
        "PlayerState.hpp",
        "MemoryInspector.cpp"
    };
    std::string tabLine = IDETheme::renderTabBar(tabs, 0, terminalWidth);

    // 2. Divisao de largura do layout
    // Esquerda: ~55% da largura (minimo 36 cols); Direita: restante para o Watch/Inspector
    int mapTargetWidth = std::clamp((terminalWidth * 55) / 100, 36, terminalWidth - 30);
    int rightPanelWidth = std::max(26, terminalWidth - mapTargetWidth - 3);

    // 3. Calculo da camera do mapa (esquerda)
    int mapTotalWidth = mapMatrix.empty() ? 0 : static_cast<int>(mapMatrix[0].length());
    int mapTotalHeight = static_cast<int>(mapMatrix.size());

    int startX = 0, endX = mapTotalWidth;
    if (mapTotalWidth > mapTargetWidth) {
        startX = std::max(0, playerPositionX - (mapTargetWidth / 2));
        endX = startX + mapTargetWidth;
        if (endX > mapTotalWidth) {
            endX = mapTotalWidth;
            startX = std::max(0, endX - mapTargetWidth);
        }
    }

    // A visualizacao ocupa da linha 1 ate o rodape
    int visibleMapRows = std::max(8, terminalHeight - 3);
    int startY = 0, endY = mapTotalHeight;
    if (mapTotalHeight > visibleMapRows) {
        startY = std::max(0, playerPositionY - (visibleMapRows / 2));
        endY = startY + visibleMapRows;
        if (endY > mapTotalHeight) {
            endY = mapTotalHeight;
            startY = std::max(0, endY - visibleMapRows);
        }
    }

    // 4. Coleta dos Blocos de Inspecao (Direita)
    std::vector<std::string> playerInspector = IDEInspector::inspectPlayer(currentPlayer, playerPositionX, playerPositionY);
    std::vector<std::string> entityInspector = IDEInspector::inspectNearestEntity(mapMatrix, playerPositionX, playerPositionY, mapTitle);

    std::vector<std::string> rightLines;
    rightLines.insert(rightLines.end(), playerInspector.begin(), playerInspector.end());
    rightLines.push_back("");
    rightLines.insert(rightLines.end(), entityInspector.begin(), entityInspector.end());

    // 5. Montagem do Buffer Atomico (Substitui por cima no topo da tela)
    std::ostringstream frame;
    frame << "\033[H"; // Move cursor para (0, 0)
    frame << tabLine << "\033[K\n";

    std::string dividerChar = "\033[38;2;80;80;80m│\033[0m";

    int totalRowsToRender = std::max(endY - startY, static_cast<int>(rightLines.size()));
    totalRowsToRender = std::min(totalRowsToRender, visibleMapRows);

    for (int i = 0; i < visibleMapRows; ++i) {
        int currentMapY = startY + i;
        std::string mapSegment = "";
        int mapVisualLen = 0;

        if (i < totalRowsToRender && currentMapY < endY && currentMapY < mapTotalHeight) {
            for (int x = startX; x < endX && x < mapTotalWidth; ++x) {
                if (x == playerPositionX && currentMapY == playerPositionY) {
                    mapSegment += "\033[1;38;2;78;201;176m@\033[0m";
                    mapVisualLen += 1;
                } else {
                    char c = mapMatrix[currentMapY][x];
                    mapSegment += cellFormatter(c, x, currentMapY);
                    mapVisualLen += 1;
                }
            }
        }

        // Preenche mapa com espacos se for menor que a coluna alvo
        if (mapVisualLen < mapTargetWidth) {
            mapSegment += std::string(mapTargetWidth - mapVisualLen, ' ');
        }

        // Lado direito (Inspector)
        std::string inspectorSegment = "";
        if (i < static_cast<int>(rightLines.size())) {
            inspectorSegment = rightLines[i];
        }

        int inspVisualLen = Appearance::getVisualLength(inspectorSegment);
        if (inspVisualLen < rightPanelWidth) {
            inspectorSegment += std::string(rightPanelWidth - inspVisualLen, ' ');
        }

        frame << mapSegment << " " << dividerChar << " " << inspectorSegment << "\033[K\n";
    }

    // 6. Barra de Status Inferior da IDE
    std::string posInfo = "Ln " + std::to_string(playerPositionY) + ", Col " + std::to_string(playerPositionX) + " | C++23 | UTF-8";
    std::string helpInfo = "[W,A,S,D] Mover | [V] Modo 3D | [I] Inv | [C] Ficha | [B] Diario | [M] Mapa";

    frame << IDETheme::renderStatusBar(posInfo, helpInfo, terminalWidth) << "\033[K";

    Appearance::moveCursor(0, 0);
    std::cout << frame.str() << std::flush;
}
