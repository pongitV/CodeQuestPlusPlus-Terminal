#include "World/Systems/MapLoader.h"
#include "UI/PerspectiveManager.h"
#include "Core/Utils/Appearance.h"
#include "World/MapControl.h"
#include "UI/Renderers/3D/EngineRaycaster/RaycasterWorld.h"

void MapLoader::enterSubMap(
    std::vector<std::string>& currentMapMatrix, std::vector<std::string>& savedMainMapMatrix,
    int& savedPositionXBeforeEnteringSubMap, int& savedPositionYBeforeEnteringSubMap,
    int& playerPositionX, int& playerPositionY, bool& playerIsInsideSubMap,
    std::string& currentMapTitle, std::vector<std::string>& savedSubMapMatrix, bool& subMapAlreadyVisited,
    const std::vector<std::string>& generatedSubMapMatrix, int initialPositionXInSubMap, int initialPositionYInSubMap, const std::string& subMapTitle, const std::function<void()>& restoreScreen)
{
    savedMainMapMatrix = currentMapMatrix;
    savedPositionXBeforeEnteringSubMap = playerPositionX;
    savedPositionYBeforeEnteringSubMap = playerPositionY;

    if (!subMapAlreadyVisited) { currentMapMatrix = generatedSubMapMatrix; subMapAlreadyVisited = true; } 
    else { currentMapMatrix = savedSubMapMatrix; }
    standardizeMapSize(currentMapMatrix);

    playerPositionX = initialPositionXInSubMap;
    playerPositionY = initialPositionYInSubMap;
    playerIsInsideSubMap = true;
    currentMapTitle = subMapTitle;
    MapControl::signal3DMapChange();
    RaycasterWorld::updateMapHash(currentMapMatrix);
    if (!PerspectiveManager::getInstance().is3DViewActive()) restoreScreen();
}

void MapLoader::standardizeMapSize(std::vector<std::string>& mapMatrix) {
    Appearance::standardizeVectorSize(mapMatrix);
}
