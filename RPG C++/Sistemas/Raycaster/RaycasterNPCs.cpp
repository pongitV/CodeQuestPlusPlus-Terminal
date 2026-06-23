#include "RaycasterNPCs.h"
#include "../../Entidades/NPCs/Morgana/NPCMorganaLayout.h"
#include "../../Entidades/NPCs/Bjorn/NPCBjornLayout.h"
#include "../../Entidades/NPCs/Franchesco/NPCFranchescoLayout.h"
#include "../../Entidades/NPCs/CavaleiroGenerico/NPCCavaleiroGenericoLayout.h"
#include <vector>
#include <string>


static SpriteCache criarPortaMadeira() {
    return RaycasterSprites::parseArte({
        "  ~~~~~~~~  ",
        " ~\033[38;2;40;20;10m/||||||\\\033[0m~ ",
        " ~\033[38;2;40;20;10m||||||||\033[0m~ ",
        " ~\033[38;2;30;15;5m||||||||\033[0m~ ",
        " ~\033[38;2;40;20;10m||||||||\033[0m~ ",
        " ~\033[38;2;30;15;5m||||||||\033[0m~ ",
        " ~\033[38;2;40;20;10m||||||\033[38;2;255;215;0mOO\033[0m~ ",
        " ~\033[38;2;30;15;5m||||||\033[38;2;255;215;0mOO\033[0m~ ",
        " ~\033[38;2;40;20;10m||||||||\033[0m~ ",
        " ~\033[38;2;30;15;5m||||||||\033[0m~ ",
        " ~\033[38;2;40;20;10m||||||||\033[0m~ ",
        " ~\033[38;2;30;15;5m||||||||\033[0m~ ",
    });
}

void RaycasterNPCs::inicializarSprites(std::map<char, SpriteCache>& cache) {
    cache['B'] = RaycasterSprites::parseSprite(NPCBjornLayouts::arteBjorn, 100, 200, 255); // Ciano Bjorn
    cache['W'] = RaycasterSprites::parseSprite(NPCMorganaLayouts::arteMorgana, 200, 100, 255); // Roxo Morgana
    
    cache['V'] = RaycasterSprites::parseSprite(NPCFranchescoLayouts::arteFranchesco, 255, 200, 50); // Amarelo Franchesco
    
    cache['X'] = RaycasterSprites::parseSprite({
        "  ____  ",
        " / || \\ ",
        " ====== ",
        " \\____/ "
    }, 200, 150, 50); // Bau de Tesouro
    
    cache['^'] = criarPortaMadeira();
    cache['1'] = criarPortaMadeira();
    cache['2'] = criarPortaMadeira();
    cache['3'] = criarPortaMadeira();
    cache['4'] = criarPortaMadeira();
    cache['5'] = criarPortaMadeira();

    cache['C'] = RaycasterSprites::parseSprite(NPCCavaleiroGenericoLayouts::arteCavaleiro, 200, 200, 220); // Cavaleiro Real

    cache['P'] = RaycasterSprites::parseSprite({
        "  ____  ",
        " |%%%%| ",
        " |____| ",
        "   ||   ",
        "   ||   "
    }, 160, 90, 40); // Placa de madeira detalhada

    cache['*'] = RaycasterSprites::parseArte({
        "         \033[38;2;50;180;50m##\033[0m         ",
        "       \033[38;2;34;139;34m######\033[0m       ",
        "     \033[38;2;34;139;34m##########\033[0m     ",
        "    \033[38;2;28;115;28m############\033[0m    ",
        "   \033[38;2;28;115;28m##############\033[0m   ",
        "  \033[38;2;22;89;22m################\033[0m  ",
        "  \033[38;2;22;89;22m################\033[0m  ",
        "   \033[38;2;14;60;14m##############\033[0m   ",
        "    \033[38;2;14;60;14m############\033[0m    ",
        "      \033[38;2;10;40;10m########\033[0m      ",
        "         \033[38;2;60;35;15m||\033[0m         ",
        "         \033[38;2;60;35;15m||\033[0m         ",
        "         \033[38;2;45;25;10m||\033[0m         ",
        "         \033[38;2;45;25;10m||\033[0m         ",
        "         \033[38;2;30;15;5m||\033[0m         "
    }); // Arvore padrao
}
