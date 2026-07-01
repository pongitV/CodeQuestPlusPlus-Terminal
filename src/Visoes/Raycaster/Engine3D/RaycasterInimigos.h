#pragma once
#include <map>
#include "RaycasterSprites.h"

class RaycasterInimigos {
public:
    static void inicializarSprites(std::map<char, SpriteCache>& cache);
};
