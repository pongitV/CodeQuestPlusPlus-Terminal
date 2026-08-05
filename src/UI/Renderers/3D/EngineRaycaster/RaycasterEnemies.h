#pragma once
#include <map>
#include "UI/Renderers/3D/EngineRaycaster/RaycasterSprites.h"

class RaycasterEnemies {
public:
    static void bootSprites(std::map<char, SpriteCache>& cache);
};
