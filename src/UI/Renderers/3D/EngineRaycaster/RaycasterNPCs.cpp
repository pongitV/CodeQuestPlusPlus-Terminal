#include "UI/Renderers/3D/EngineRaycaster/RaycasterNPCs.h"
#include "Domain/NPCs/NPCMageLayout.h"
#include "Domain/NPCs/NPCPriestLayout.h"
#include "Domain/NPCs/NPCBlacksmithLayout.h"
#include "Domain/NPCs/NPCMerchantLayout.h"
#include "Domain/NPCs/NPCAppearanceLayout.h"
#include "Domain/NPCs/NPCGenericKnightLayout.h"
#include "Domain/NPCs/NPCAlchemistLayout.h"
#include "UI/Renderers/3D/EngineRaycaster/TextureManager.h"
#include <vector>
#include <string>
#include <cmath>
#include <map>

static SpriteCache createDoorWood() {
    return RaycasterSprites::parsiArt({
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

static SpriteCache createBackpack() {
    return RaycasterSprites::parsiArt({
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

void RaycasterNPCs::bootSprites(std::map<char, SpriteCache>& cache) {
    cache['B'] = RaycasterSprites::parsiSprite(NPCBlacksmithLayouts::artBlacksmith, 100, 200, 255); // Ciano Bjorn
    cache['W'] = RaycasterSprites::parsiSprite(NPCMageLayouts::artMage, 200, 100, 255); // Roxo Morgana
    
    cache['V'] = RaycasterSprites::parsiSprite(NPCMerchantLayouts::artMerchant, 255, 200, 50); // Amarelo Franchesco
    
    cache['X'] = RaycasterSprites::parsiSprite({
        "  ____  ",
        " / || \\ ",
        " ====== ",
        " \\____/ "
    }, 200, 150, 50); // Bau de Tesouro

    cache['Y'] = createBackpack(); // Mochila aberta
    
    cache['^'] = createDoorWood();
    cache['1'] = createDoorWood();
    cache['2'] = createDoorWood();
    cache['3'] = createDoorWood();
    cache['4'] = createDoorWood();
    cache['5'] = createDoorWood();

    cache['C'] = RaycasterSprites::parsiSprite(NPCKnightGenericLayouts::artKnight, 200, 200, 220); // Cavaleiro Real

    // Sprites customizados para o PATIO DO REINO e Igreja
    cache['Z'] = RaycasterSprites::parsiSprite(NPCAppearanceLayouts::artAppearance, 120, 50, 200); // Anok (Roxo Estiloso)

    cache['Q'] = RaycasterSprites::parsiSprite(NPCAlchemistLayouts::artAlchemist, 180, 50, 200); // Alquimista (Roxo)

    cache['K'] = RaycasterSprites::parsiSprite({
        "  /==\\  ",
        " [ oo ] ",
        " [####] ",
        "  ||||  "
    }, 220, 180, 100); // Barraquinha de comida (Marrom claro)

    cache['L'] = RaycasterSprites::parsiSprite({
        "   /\\   ",
        "  /  \\  ",
        " /_||_\\ ",
        " | || | ",
        " | || | "
    }, 230, 230, 250); // Capela (Branco MArmore)

    cache['J'] = RaycasterSprites::parsiSprite(NPCPriestLayouts::artPriest, 255, 215, 0); // Padre Benedito (Dourado)

    cache['P'] = RaycasterSprites::parsiSprite({
        "  ____  ",
        " |%%%%| ",
        " |____| ",
        "   ||   ",
        "   ||   "
    }, 160, 90, 40); // Placa de madeira detalhada

    cache['!'] = RaycasterSprites::parsiSprite({
        "        ",
        "   //   ",
        "  //    ",
        " //     ",
        "        "
    }, 100, 255, 100); // Comentário de Linha (Verde translúcido)

    cache['%'] = RaycasterSprites::parsiSprite({
        "        ",
        "   /*   ",
        "  ...   ",
        "   */   ",
        "        "
    }, 100, 255, 100); // Comentário de Bloco (Verde translúcido)

    cache['@'] = RaycasterSprites::parsiSprite({
        "  ====  ",
        " |>_  | ",
        " |    | ",
        "  ====  ",
        "  [||]  "
    }, 50, 255, 255); // Terminal Hackeável (Ciano Brilhante)

    cache['*'] = RaycasterSprites::parsiArt({
        "           \033[38;2;60;190;60m.o%8O%o.\033[0m           ",
        "        \033[38;2;50;170;50m.o%88888888%Oo.\033[0m        ",
        "      \033[38;2;42;150;42mo%888888888888888%Oo\033[0m      ",
        "     \033[38;2;36;135;36m%88888888888888888888%\033[0m     ",
        "    \033[38;2;30;120;30m%8888888888888888888888%\033[0m    ",
        "   \033[38;2;26;105;26m8888888888888888888888888\033[0m   ",
        "  \033[38;2;22;95;22m%8888888888888888888888888%\033[0m  ",
        "  \033[38;2;18;85;18m%8888888888888888888888888%\033[0m  ",
        "  \033[38;2;15;70;15m%8888888888888888888888888%\033[0m  ",
        "   \033[38;2;12;60;12mY88888888888888888888888Y\033[0m   ",
        "    \033[38;2;10;50;10m`Y88888888888888888888P'\033[0m    ",
        "       \033[38;2;8;35;8m`Y88888888888888P'\033[0m       ",
        "            \033[38;2;80;50;30m||||\033[0m            ",
        "            \033[38;2;70;45;25m||||\033[0m            ",
        "           \033[38;2;60;40;20m/|||\\\033[0m           ",
        "           \033[38;2;50;30;15m\\|||/\033[0m           ",
        "            \033[38;2;40;25;10m||||\033[0m            ",
        "           \033[38;2;35;20;8m_||||_\033[0m           "
    }); // Arvore padrao realistica

    SpriteCache forestTree;
    forestTree.width = 32;
    forestTree.height = 32;
    forestTree.pixels.resize(32 * 32);
    for (int y = 0; y < 32; y++) {
        for (int x = 0; x < 32; x++) {
            SpritePixel px;
            float sx = x * 2.0f;
            float sy = y * 2.0f;
            float cx = sx - 32.0f;
            
            float trunkRadius = 6.0f + (sy / 63.0f) * 6.0f;
            bool isTrunk = (std::abs(cx) <= trunkRadius) && (sy >= 15);
            
            float leafCy = sy - 14.0f;
            float leafRadiusX = 36.0f + 4.0f * ManagerTextures::fastYes(sx * 0.4f + sy * 0.3f);
            float leafRadiusY = 16.0f + 3.0f * ManagerTextures::fastYes(sx * 0.5f);
            
            float distSq = (cx * cx) / (leafRadiusX * leafRadiusX) + (leafCy * leafCy) / (leafRadiusY * leafRadiusY);
            bool isLeaf = (distSq <= 1.0f) && (sy < 35);
            
            if (isLeaf || isTrunk) {
                px.isTransparent = false;
                px.ch = ' ';
                px.hasFg = false;
                if (isLeaf) {
                    float leafNoise = ManagerTextures::fastYes(sx * 0.6f + sy * 0.6f) + ManagerTextures::fastYes(sx * 1.5f + sy * 0.5f);
                    float dist = std::sqrt(distSq);
                    float edgeFade = 1.0f - dist * 0.35f;
                    px.r = std::clamp((int)((30 + leafNoise * 8) * edgeFade), 0, 255);
                    px.g = std::clamp((int)((120 + leafNoise * 15) * edgeFade), 0, 255);
                    px.b = std::clamp((int)((40 + leafNoise * 8) * edgeFade), 0, 255);
                } else {
                    float noiseY = ManagerTextures::fastYes(sy * 0.2f);
                    float barkNoise = ManagerTextures::fastYes(sx * 0.8f + noiseY * 2.0f);
                    float nx = cx / trunkRadius; 
                    float lighting = 1.0f - (nx * nx * 0.6f); 
                    px.r = std::clamp((int)((95 + barkNoise * 12) * lighting), 0, 255);
                    px.g = std::clamp((int)((65 + barkNoise * 8) * lighting), 0, 255);
                    px.b = std::clamp((int)((45 + barkNoise * 8) * lighting), 0, 255);
                }
            } else {
                px.isTransparent = true;
            }
            forestTree.pixels[y * 32 + x] = px;
        }
    }
    cache[(char)127] = forestTree;
}
