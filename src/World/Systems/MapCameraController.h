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
    static float getCameraPosX3D();
    static inline float getCameraPostX3D() { return getCameraPosX3D(); }
    static float getCameraPosY3D();
    static inline float getCameraPostY3D() { return getCameraPosY3D(); }
    static float getCameraAngle3D();
    static std::string getCurrentMapTitle();
    static std::vector<std::string> getCurrentMapMatrix();

    static void setCameraState(float x, float y, float angle, const std::string& title, const std::vector<std::string>& matrix);
    static void setCameraPos(float x, float y);
    static void setCameraAngle(float angle);
    static bool hasJustChangedMap();
    static void resetMapChangeFlag();
};
