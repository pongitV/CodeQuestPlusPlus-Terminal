// [PT-BR] Arquivo: MapCameraController.h
// [PT-BR] Proposito: Gerenciamento do estado da camera 3D e rastreamento de mapa ativo.
// [EN-US] File: MapCameraController.h
// [EN-US] Purpose: 3D camera state management and active map tracking.

#pragma once

#include <string>
#include <vector>

class MapCameraController {
private:
    static bool s_justChangedMap;
    static float s_cameraPosX3D;
    static float s_cameraPosY3D;
    static float s_cameraAngle3D;
    static std::string s_currentMapTitle;
    static std::vector<std::string> s_currentMapMatrix;

public:
    static void signal3DMapChange();
    static bool is3DExplorationActive();
    static float getCameraPostX3D();
    static float getCameraPostY3D();
    static float getCameraAngle3D();
    static std::string getCurrentMapTitle();
    static std::vector<std::string> getCurrentMapMatrix();

    static void setCameraState(float x, float y, float angle, const std::string& title, const std::vector<std::string>& matrix);
    static void setCameraPos(float x, float y);
    static void setCameraAngle(float angle);
    static bool hasJustChangedMap();
    static void resetMapChangeFlag();
};
