#pragma once
#include <map>
#include "UI/Renderers/3D/EngineRaycaster/RaycasterSprites.h"

class RaycasterNPCs {
public:
    static void bootSprites(std::map<char, SpriteCache>& cache);
};
