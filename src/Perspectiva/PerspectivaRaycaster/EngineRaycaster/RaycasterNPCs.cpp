#include "RaycasterNPCs.h"
#include "../../../Entidades/NPCs/Maga/NPCMagaLayout.h"
#include "../../../Entidades/NPCs/Padre/NPCPadreLayout.h"
#include "../../../Entidades/NPCs/Ferreiro/NPCFerreiroLayout.h"
#include "../../../Entidades/NPCs/Mercador/NPCMercadorLayout.h"
#include "../../../Entidades/NPCs/Aparencia/NPCAparenciaLayout.h"
#include "../../../Entidades/NPCs/CavaleiroGenerico/NPCCavaleiroGenericoLayout.h"
#include "../../../Entidades/NPCs/Alquimista/NPCAlquimistaLayout.h"
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

static SpriteCache criarMochila() {
    return RaycasterSprites::parseArte({
        "       __       ",
        "     /    \\     ",
        "    |      |    ",
        "   /\\______/\\   ",
        "  /  |    |  \\  ",
        " |   |____|   | ",
        " |  /      \\  | ",
        " | | \033[38;2;255;215;0m(oo)\033[0m | | ",
        " | |        | | ",
        "  \\ \\______/ /  ",
        "   \\________/   ",
        "                "
    });
}

void RaycasterNPCs::inicializarSprites(std::map<char, SpriteCache>& cache) {
    cache['B'] = RaycasterSprites::parseSprite(NPCFerreiroLayouts::arteFerreiro, 100, 200, 255); // Ciano Bjorn
    cache['W'] = RaycasterSprites::parseSprite(NPCMagaLayouts::arteMaga, 200, 100, 255); // Roxo Morgana
    
    cache['V'] = RaycasterSprites::parseSprite(NPCMercadorLayouts::arteMercador, 255, 200, 50); // Amarelo Franchesco
    
    cache['X'] = RaycasterSprites::parseSprite({
        "  ____  ",
        " / || \\ ",
        " ====== ",
        " \\____/ "
    }, 200, 150, 50); // Bau de Tesouro

    cache['Y'] = criarMochila(); // Mochila aberta
    
    cache['^'] = criarPortaMadeira();
    cache['1'] = criarPortaMadeira();
    cache['2'] = criarPortaMadeira();
    cache['3'] = criarPortaMadeira();
    cache['4'] = criarPortaMadeira();
    cache['5'] = criarPortaMadeira();

    cache['C'] = RaycasterSprites::parseSprite(NPCCavaleiroGenericoLayouts::arteCavaleiro, 200, 200, 220); // Cavaleiro Real

    // Sprites customizados para o PATIO DO REINO e Igreja
    cache['A'] = RaycasterSprites::parseSprite(NPCAparenciaLayouts::arteAparencia, 120, 50, 200); // Anok (Roxo Estiloso)

    cache['Q'] = RaycasterSprites::parseSprite(NPCAlquimistaLayouts::arteAlquimista, 180, 50, 200); // Alquimista (Roxo)

    cache['K'] = RaycasterSprites::parseSprite({
        "  /==\\  ",
        " [ oo ] ",
        " [####] ",
        "  ||||  "
    }, 220, 180, 100); // Barraquinha de comida (Marrom claro)

    cache['L'] = RaycasterSprites::parseSprite({
        "   /\\   ",
        "  /  \\  ",
        " /_||_\\ ",
        " | || | ",
        " | || | "
    }, 230, 230, 250); // Capela (Branco MArmore)

    cache['J'] = RaycasterSprites::parseSprite({
        "   ()   ",
        "  /||\\  ",
        "  \\||/  ",
        "  /||\\  ",
        "  /||\\  "
    }, 255, 215, 0); // Padre Benedito (Dourado)

    cache['P'] = RaycasterSprites::parseSprite({
        "  ____  ",
        " |%%%%| ",
        " |____| ",
        "   ||   ",
        "   ||   "
    }, 160, 90, 40); // Placa de madeira detalhada

    cache['!'] = RaycasterSprites::parseSprite({
        "        ",
        "   //   ",
        "  //    ",
        " //     ",
        "        "
    }, 100, 255, 100); // Comentário de Linha (Verde translúcido)

    cache['%'] = RaycasterSprites::parseSprite({
        "        ",
        "   /*   ",
        "  ...   ",
        "   */   ",
        "        "
    }, 100, 255, 100); // Comentário de Bloco (Verde translúcido)

    cache['@'] = RaycasterSprites::parseSprite({
        "  ====  ",
        " |>_  | ",
        " |    | ",
        "  ====  ",
        "  [||]  "
    }, 50, 255, 255); // Terminal Hackeável (Ciano Brilhante)

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
