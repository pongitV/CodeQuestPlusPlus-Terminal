#pragma once

#include <vector>
#include <string>
#include <map>
#include "UI/Renderers/3D/EngineRaycaster/RaycasterSprites.h"

class RaycasterRenderer {
public:
    static void render3D(std::vector<Pixel3D>& screen, int SCREEN_WIDTH, int SCREEN_HEIGHT, float playerX, float playerY, float angleVisa, float horizon, int bobbingOffset, float depthMaximum, float timeAbsolute, const std::vector<std::string>& mapMatrix, const std::string& titleMap, bool themeForest, int themeSky, const std::map<char, SpriteCache>& cacheSprites);
};