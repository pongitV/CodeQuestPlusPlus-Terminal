#include "RaycasterNPCs.h"
#include "../../Entidades/NPCs/Morgana/NPCMorganaLayout.h"
#include "../../Entidades/NPCs/Bjorn/NPCBjornLayout.h"
#include <vector>
#include <string>

void RaycasterNPCs::inicializarSprites(std::map<char, SpriteCache>& cache) {
    cache['B'] = RaycasterSprites::parseArte(RaycasterSprites::colorirArte(NPCBjornLayouts::arteBjorn, "\033[38;2;100;200;255m"));
    cache['W'] = RaycasterSprites::parseArte(RaycasterSprites::colorirArte(NPCMorganaLayouts::arteMorgana, "\033[38;2;200;100;255m"));
    
    cache['X'] = RaycasterSprites::parseArte({
        "\033[38;2;200;150;50m  ____  \033[0m",
        "\033[38;2;200;150;50m / || \\ \033[0m",
        "\033[38;2;200;150;50m ====== \033[0m",
        "\033[38;2;200;150;50m \\____/ \033[0m"
    }); // Bau de Tesouro
    
    cache['^'] = RaycasterSprites::parseArte({
        "\033[38;2;255;255;50m  ( )  \033[0m", "\033[38;2;255;255;50m (   ) \033[0m", "\033[38;2;255;255;50m  ( )  \033[0m"
    }); // Portal
}
