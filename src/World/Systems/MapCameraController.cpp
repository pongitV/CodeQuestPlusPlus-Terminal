// [PT-BR] Arquivo: MapCameraController.cpp
// [PT-BR] Proposito: Implementacao do gerenciamento da camera 3D e mapas.
// [EN-US] File: MapCameraController.cpp
// [EN-US] Purpose: Implementation of 3D camera and map state management.

#include "World/Systems/MapCameraController.h"
#include "UI/PerspectiveManager.h"

bool MapCameraController::s_justChangedMap = false;
float MapCameraController::s_cameraPosX3D = -1.0f;
float MapCameraController::s_cameraPosY3D = -1.0f;
float MapCameraController::s_cameraAngle3D = 0.0f;
std::string MapCameraController::s_currentMapTitle = "";
std::vector<std::string> MapCameraController::s_currentMapMatrix;

void MapCameraController::signal3DMapChange() { s_justChangedMap = true; }
bool MapCameraController::is3DExplorationActive() { return PerspectiveManager::getInstance().is3DViewActive(); }
float MapCameraController::getCameraPostX3D() { return s_cameraPosX3D; }
float MapCameraController::getCameraPostY3D() { return s_cameraPosY3D; }
float MapCameraController::getCameraAngle3D() { return s_cameraAngle3D; }
std::string MapCameraController::getCurrentMapTitle() { return s_currentMapTitle; }
std::vector<std::string> MapCameraController::getCurrentMapMatrix() { return s_currentMapMatrix; }

void MapCameraController::setCameraState(float x, float y, float angle, const std::string& title, const std::vector<std::string>& matrix) {
    s_cameraPosX3D = x;
    s_cameraPosY3D = y;
    s_cameraAngle3D = angle;
    s_currentMapTitle = title;
    s_currentMapMatrix = matrix;
}

void MapCameraController::setCameraPos(float x, float y) {
    s_cameraPosX3D = x;
    s_cameraPosY3D = y;
}

void MapCameraController::setCameraAngle(float angle) {
    s_cameraAngle3D = angle;
}

bool MapCameraController::hasJustChangedMap() {
    return s_justChangedMap;
}

void MapCameraController::resetMapChangeFlag() {
    s_justChangedMap = false;
}
