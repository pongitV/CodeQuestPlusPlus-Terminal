#pragma once

#include "Domain/Characters/Character.h"
#include <functional>
#include <string>
#include <vector>
#include <memory>
#include "Core/Utils/Appearance.h"
#include "World/InteractionMap.h"

// Fachada para delegacao dos sistemas de animacao, colisao e renderizacao de mapa.
class MapControl 
{
public:
    static void processCombat(
        Character* currentPlayer, std::vector<std::string>& currentMapMatrix, 
        int& playerPositionX, int& playerPositionY, bool& isExplorationActive,
        const std::string& combatTitle, const std::string& warningMessage, std::vector<std::unique_ptr<Character>> enemiesForBattle, 
        int positionXAfterCombat, int positionYAfterCombat, int initialEnemyPositionX, int occupiedCellsQuantity, int terminalWidth, const std::function<void()>& restoreScreen);

    static bool is3DExplorationActive();
    static void signal3DMapChange();
    static float getCameraPosX3D();
    static inline float getCameraPostX3D() { return getCameraPosX3D(); }
    static float getCameraPosY3D();
    static inline float getCameraPostY3D() { return getCameraPosY3D(); }
    static float getCameraAngle3D();
    static std::string getCurrentMapTitle();
    static std::vector<std::string> getCurrentMapMatrix();

    static std::string formatCell(char cell, int x, int y, const std::string& mapTitle, const std::vector<std::string>& mapMatrix, bool isMinimap = false);

    static NextMapTransition executeExplorationLoop(
        Character* currentPlayer,
        std::vector<std::string>& currentMapMatrix,
        int& playerPositionX,
        int& playerPositionY,
        bool& isExplorationActive,
        const std::string& currentMapTitle,
        const std::function<std::string()>& getEnemySymbols,
        const std::function<std::vector<std::string>()>& getOriginalLayout,
        const std::function<void(int, int, int)>& processInteraction,
        const std::function<std::string(char, int, int)>& formatter,
        const std::function<void()>& restoreScreen,
        int& initialLineToDrawMap,
        bool& needsRender
    );
};
