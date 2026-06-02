#pragma once
#include <map>
#include "RaycasterSprites.h"

class RaycasterNPCs {
public:
    static void inicializarSprites(std::map<char, SpriteCache>& cache);
};
