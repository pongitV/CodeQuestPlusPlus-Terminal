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
    }, 100, 255, 100); // Comentario de Linha (Verde translucido)

    cache['%'] = RaycasterSprites::parsiSprite({
        "        ",
        "   /*   ",
        "  ...   ",
        "   */   ",
        "        "
    }, 100, 255, 100); // Comentario de Bloco (Verde translucido)

    cache['@'] = RaycasterSprites::parsiSprite({
        "  ====  ",
        " |>_  | ",
        " |    | ",
        "  ====  ",
        "  [||]  "
    }, 50, 255, 255); // Terminal Hackeavel (Ciano Brilhante)

    // ==========================================
    // ARVORE DA VILA E SPAWN (cache['*'])
    // Mesma arvore para Vila e Spawn, diferente da floresta
    // Estilo: Copa arredondada e volumosa, folhas verdes vivas/claras, tronco carvalho quente
    // ==========================================
    SpriteCache villageTree;
    villageTree.width = 48;
    villageTree.height = 48;
    villageTree.pixels.resize(48 * 48);

    for (int y = 0; y < 48; y++) {
        for (int x = 0; x < 48; x++) {
            SpritePixel px;
            float sx = static_cast<float>(x);
            float sy = static_cast<float>(y);
            float cx = sx - 24.0f;

            // Tronco de carvalho quente e elegante
            float trunkRadius = 3.0f + (sy / 47.0f) * 2.2f + ((sy > 40.0f) ? (sy - 40.0f) * 0.25f : 0.0f);
            bool isTrunk = (std::abs(cx) <= trunkRadius) && (sy >= 22.0f);

            // Galhos principais que se abrem para os lados da copa
            bool isBranch = false;
            float branchDist = 999.0f;
            if (sy >= 18.0f && sy <= 30.0f) {
                float progL = (30.0f - sy) / 12.0f;
                float bxL = -progL * 13.0f;
                float dL = std::abs(cx - bxL);
                if (dL <= 2.2f * (1.0f - progL * 0.25f)) { isBranch = true; branchDist = std::min(branchDist, dL); }
                
                float progR = (28.0f - sy) / 10.0f;
                float bxR = progR * 14.0f;
                float dR = std::abs(cx - bxR);
                if (dR <= 2.2f * (1.0f - progR * 0.25f)) { isBranch = true; branchDist = std::min(branchDist, dR); }
            }

            // Copa redonda e fofa (Broadleaf / Apple / Oak)
            struct VPuff { float x, y, r; };
            VPuff vPuffs[] = {
                { 0.0f, 9.0f, 10.5f },
                { -9.0f, 12.0f, 11.5f },
                { 9.0f, 12.0f, 11.5f },
                { -14.0f, 18.0f, 12.0f },
                { 14.0f, 18.0f, 12.0f },
                { 0.0f, 17.0f, 14.0f },
                { -8.0f, 25.0f, 11.0f },
                { 8.0f, 25.0f, 11.0f },
                { 0.0f, 26.0f, 12.0f }
            };

            float bestVCov = 0.0f;
            float vPuffShading = 1.0f;

            for (int i = 0; i < 9; i++) {
                float dx = cx - vPuffs[i].x;
                float dy = sy - vPuffs[i].y;
                float distSq = (dx * dx + dy * dy) / (vPuffs[i].r * vPuffs[i].r);
                if (distSq < 1.0f) {
                    float cov = 1.0f - distSq;
                    if (cov > bestVCov) {
                        bestVCov = cov;
                        vPuffShading = 1.0f - (dy / vPuffs[i].r) * 0.45f - (std::abs(dx) / vPuffs[i].r) * 0.3f;
                    }
                }
            }

            bool isLeaf = (bestVCov > 0.0f && sy < 34.0f);
            bool isFgTrunk = (isTrunk && sy >= 28.0f) || (isBranch && sy >= 26.0f);

            if (isLeaf || isTrunk || isBranch) {
                px.isTransparent = false;
                px.ch = ' ';
                px.hasFg = false;

                if (isFgTrunk && !isLeaf) {
                    float normX = (isBranch) ? branchDist / 2.0f : cx / trunkRadius;
                    normX = std::clamp(normX, -1.0f, 1.0f);
                    float cylLight = 1.0f - (normX * normX) * 0.5f;

                    float barkNoise = ManagerTextures::fastYes(sx * 0.8f + sy * 0.3f) * 1.2f;
                    px.r = std::clamp((int)((130 + barkNoise * 18) * cylLight), 0, 255);
                    px.g = std::clamp((int)((86 + barkNoise * 12) * cylLight), 0, 255);
                    px.b = std::clamp((int)((48 + barkNoise * 8) * cylLight), 0, 255);
                } else {
                    float leafNoise = ManagerTextures::fastYes(sx * 0.7f + sy * 0.7f) * 1.2f + ManagerTextures::fastYes(sx * 1.4f - sy * 0.8f) * 0.5f;
                    float shade = std::clamp(vPuffShading + leafNoise * 0.25f, 0.45f, 1.45f);
                    float heightFactor = 1.0f - (sy / 40.0f) * 0.25f;
                    float finalLight = shade * heightFactor;

                    bool isApple = (bestVCov > 0.3f && bestVCov < 0.7f && (std::abs(ManagerTextures::fastYes(sx * 1.8f + sy * 1.4f)) > 0.88f) && sy > 12.0f && sy < 28.0f);

                    if (isApple) {
                        px.r = 220;
                        px.g = 45;
                        px.b = 35;
                    } else {
                        int baseR = 55, baseG = 165, baseB = 42;
                        if (finalLight > 1.05f) {
                            baseR = 105; baseG = 215; baseB = 58;
                        } else if (finalLight < 0.65f) {
                            baseR = 28; baseG = 105; baseB = 25;
                        }

                        px.r = std::clamp((int)((baseR + leafNoise * 14) * finalLight), 0, 255);
                        px.g = std::clamp((int)((baseG + leafNoise * 22) * finalLight), 0, 255);
                        px.b = std::clamp((int)((baseB + leafNoise * 10) * finalLight), 0, 255);
                    }
                }
            } else {
                px.isTransparent = true;
            }
            villageTree.pixels[y * 48 + x] = px;
        }
    }
    cache['*'] = villageTree;

    // ==========================================
    // ARVORE DA FLORESTA (cache[(char)127])
    // Estilo compativel e identico a parede de limite da floresta
    // ==========================================
    SpriteCache forestTree;
    forestTree.width = 48;
    forestTree.height = 48;
    forestTree.pixels.resize(48 * 48);

    for (int y = 0; y < 48; y++) {
        for (int x = 0; x < 48; x++) {
            SpritePixel px;
            float sx = static_cast<float>(x);
            float sy = static_cast<float>(y);
            float cx = sx - 24.0f;

            // Tronco gnarled da floresta com raizes largas
            float sway = ManagerTextures::fastYes(sy * 0.15f) * 1.5f;
            float trunkCx = cx - sway;
            float flare = (sy > 36.0f) ? (sy - 36.0f) * 0.35f : 0.0f;
            float trunkRadius = 3.5f + (sy / 47.0f) * 2.0f + flare;
            bool isTrunk = (std::abs(trunkCx) <= trunkRadius) && (sy >= 16.0f);

            // Galhos estruturais
            bool isBranch = false;
            float branchDist = 999.0f;
            if (sy >= 15.0f && sy <= 28.0f) {
                float progL = (28.0f - sy) / 13.0f;
                float bxL = -progL * 14.0f;
                float dL = std::abs(cx - bxL);
                if (dL <= 2.2f * (1.0f - progL * 0.3f)) { isBranch = true; branchDist = std::min(branchDist, dL); }
                
                float progR = (26.0f - sy) / 11.0f;
                float bxR = progR * 13.0f;
                float dR = std::abs(cx - bxR);
                if (dR <= 2.2f * (1.0f - progR * 0.3f)) { isBranch = true; branchDist = std::min(branchDist, dR); }
            }

            // Tufos de folhagem densa em camadas (estilo copa floresta antiga)
            struct LeafPuff { float x, y, rx, ry; };
            LeafPuff fPuffs[] = {
                { 0.0f, 9.0f, 13.0f, 8.5f },
                { -8.0f, 15.0f, 14.0f, 9.0f },
                { 8.0f, 14.0f, 14.0f, 9.0f },
                { 0.0f, 16.0f, 15.0f, 9.5f },
                { -14.0f, 23.0f, 13.0f, 8.5f },
                { 14.0f, 22.0f, 13.0f, 8.5f },
                { 0.0f, 24.0f, 16.0f, 9.5f },
                { -6.0f, 29.0f, 11.0f, 7.5f },
                { 6.0f, 28.0f, 11.0f, 7.5f }
            };

            float bestLeafCov = 0.0f;
            float leafPuffShading = 1.0f;

            for (const auto& p : fPuffs) {
                float dx = cx - p.x;
                float dy = sy - p.y;
                float distSq = (dx * dx) / (p.rx * p.rx) + (dy * dy) / (p.ry * p.ry);
                if (distSq < 1.0f) {
                    float cov = 1.0f - distSq;
                    if (cov > bestLeafCov) {
                        bestLeafCov = cov;
                        leafPuffShading = 1.0f - (dy / p.ry) * 0.4f - (std::abs(dx) / p.rx) * 0.25f;
                    }
                }
            }

            bool isLeaf = (bestLeafCov > 0.0f && sy < 35.0f);
            bool isFgTrunk = (isTrunk && sy >= 26.0f) || isBranch;

            if (isLeaf || isTrunk || isBranch) {
                px.isTransparent = false;
                px.ch = ' ';
                px.hasFg = false;

                if (isFgTrunk && !isLeaf) {
                    float normX = (isBranch) ? branchDist / 2.0f : trunkCx / trunkRadius;
                    normX = std::clamp(normX, -1.0f, 1.0f);
                    float cylLight = 1.0f - (normX * normX) * 0.55f;

                    float barkNoise = ManagerTextures::fastYes(sx * 0.9f + sy * 0.4f) * 1.3f;
                    float mossNoise = ManagerTextures::fastYes(sx * 0.4f + sy * 0.2f);
                    bool hasMoss = (sy > 34.0f && mossNoise > -0.1f) || (sy > 42.0f);

                    if (hasMoss) {
                        float mossFac = std::clamp((sy - 34.0f) / 12.0f, 0.0f, 1.0f);
                        int mR = (int)((35 + barkNoise * 8) * cylLight);
                        int mG = (int)((85 + mossNoise * 18) * cylLight);
                        int mB = (int)((30 + barkNoise * 6) * cylLight);

                        int bR = (int)((72 + barkNoise * 15) * cylLight);
                        int bG = (int)((46 + barkNoise * 10) * cylLight);
                        int bB = (int)((28 + barkNoise * 6) * cylLight);

                        px.r = std::clamp((int)(bR * (1.0f - mossFac) + mR * mossFac), 0, 255);
                        px.g = std::clamp((int)(bG * (1.0f - mossFac) + mG * mossFac), 0, 255);
                        px.b = std::clamp((int)(bB * (1.0f - mossFac) + mB * mossFac), 0, 255);
                    } else {
                        px.r = std::clamp((int)((74 + barkNoise * 16) * cylLight), 0, 255);
                        px.g = std::clamp((int)((48 + barkNoise * 11) * cylLight), 0, 255);
                        px.b = std::clamp((int)((28 + barkNoise * 7) * cylLight), 0, 255);
                    }
                } else {
                    float leafNoise = ManagerTextures::fastYes(sx * 0.7f + sy * 0.6f) * 1.2f + ManagerTextures::fastYes(sx * 1.5f - sy * 0.8f) * 0.5f;
                    float shade = std::clamp(leafPuffShading + leafNoise * 0.25f, 0.45f, 1.4f);
                    float heightFactor = 1.0f - (sy / 40.0f) * 0.3f;
                    float finalLight = shade * heightFactor;

                    int baseR = 32, baseG = 95, baseB = 34;
                    if (finalLight > 1.05f) {
                        baseR = 58; baseG = 145; baseB = 52;
                    } else if (finalLight < 0.65f) {
                        baseR = 15; baseG = 52; baseB = 18;
                    }

                    px.r = std::clamp((int)((baseR + leafNoise * 12) * finalLight), 0, 255);
                    px.g = std::clamp((int)((baseG + leafNoise * 20) * finalLight), 0, 255);
                    px.b = std::clamp((int)((baseB + leafNoise * 10) * finalLight), 0, 255);
                }
            } else {
                px.isTransparent = true;
            }
            forestTree.pixels[y * 48 + x] = px;
        }
    }
    cache[(char)127] = forestTree;
}
