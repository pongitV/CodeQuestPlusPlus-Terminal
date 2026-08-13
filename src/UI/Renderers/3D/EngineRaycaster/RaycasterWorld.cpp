#include "UI/Renderers/3D/EngineRaycaster/RaycasterWorld.h"
#include "UI/Renderers/3D/EngineRaycaster/TextureManager.h"
#include "UI/Renderers/3D/EngineRaycaster/MapCache.h"
#include "UI/Renderers/3D/EngineRaycaster/Illuminator.h"
#include <algorithm>
#include <cmath>
#include <string_view>
#include "UI/PerspectiveManager.h"

#include "World/Village/Map1VillageLayout.h"
#include "World/Forest/Map2ForestLayout.h"
#include "World/Kingdom/Map3KingdomBridgeLayout.h"
#include "World/Kingdom/Map4KingdomLayout.h"

#include <atomic>

static std::atomic<size_t> g_currentMapHash{0};

char RaycasterWorld::getNPCNext(const std::string& titleMap, int mapX, int mapY, const std::vector<std::string>* mapMatrix) {
    if (mapMatrix != nullptr) {
        return CacheMap::getNPCNext(titleMap, mapX, mapY, *mapMatrix);
    }
    
    static thread_local std::string lastTitle;
    static thread_local std::vector<std::string> layout;

    if (titleMap != lastTitle) {
        lastTitle = titleMap;
        std::string upper = titleMap;
        for (char& c : upper) c = std::toupper(static_cast<unsigned char>(c));

        if (upper == "REINO" || upper == "PATIO DO REINO") layout = Map4KingdomLayouts::getKingdomLayout();
        else if (upper.find("IGREJA") != std::string::npos) layout = Map4KingdomLayouts::getChurchLayout();
        else if (upper.find("PONTE") != std::string::npos || upper == "CAMINHO DO REINO") layout = Map3KingdomBridgeLayouts::getKingdomBridgeLayout();
        else if (upper.find("VILA") != std::string::npos) layout = Map1VillageLayouts::getInitialVillageLayout();
        else if (upper.find("FLORESTA") != std::string::npos) layout = Map2ForestLayouts::getForestLayout();
        else if (upper.find("CAVERNA") != std::string::npos) layout = Map1VillageLayouts::getBasementLayout(false);
        else layout.clear();
    }

    return CacheMap::getNPCNext(titleMap, mapX, mapY, layout);
}

static thread_local std::string g_currentMapTitle = "";

static const MapFlags& getFlagsMap(const std::string& titleMap) {
    static thread_local std::string lastTitle;
    if (titleMap != lastTitle) {
        lastTitle = titleMap;
        g_currentMapTitle = titleMap;
    }
    return CacheMap::getFlags(titleMap);
}


bool RaycasterWorld::isThemeForest(const std::string& titleMap) {
    const auto& flags = getFlagsMap(titleMap);
    return (flags.upperTitle.find("FLORESTA") != std::string::npos || flags.upperTitle.find("BOSQUE") != std::string::npos || flags.upperTitle.find("CABANA") != std::string::npos);
}

bool RaycasterWorld::isEntity(char c) {
    if (c == 'T' && g_currentMapTitle.find("CORACAO") != std::string::npos) return false;
    return (c == 'G' || c == 'O' || c == 'B' || c == 'F' || c == 'S' || c == 'A' || c == 'M' || c == 'T' || c == 'H' || c == 'R' || c == 'P' || c == '^' || c == '*' || c == 'C' || c == 'I' || c == 'Q' || c == 'Y' || c == 'Z' || c == 'V' || c == 'W' || c == 'N');
}

bool RaycasterWorld::isTeleport(char c) { return c == '^'; }

bool RaycasterWorld::isWalkable(int mapX, int mapY, const std::vector<std::string>& mapMatrix) {
    char c = mapMatrix[mapY][mapX];
    if (c == '*') return false;
    
    if (c == '=' || c == '|' || c == '\'' || c == '+') return false;

    if (c == '.' || c == ' ' || c == '^' || c == '~' || isEntity(c)) return true;
    if (isMapLabel(mapX, mapY, mapMatrix)) return true;
    return false;
}

void RaycasterWorld::updateMapHash(const std::vector<std::string>& mapMatrix) {
    size_t hash = 0;
    for (const auto& r : mapMatrix) {
        hash ^= std::hash<std::string_view>{}(r) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    }
    g_currentMapHash = hash;
}

size_t RaycasterWorld::getMapHash() {
    return g_currentMapHash;
}

bool RaycasterWorld::isMapLabel(int mapX, int mapY, const std::vector<std::string>& mapMatrix) {
    static thread_local size_t lastMapHash = 0;
    static thread_local std::vector<std::vector<char>> cachedLabels;

    int height = mapMatrix.size();
    if (height == 0) return false;
    int width = mapMatrix[0].size();
    if (mapY < 0 || mapY >= height || mapX < 0 || mapX >= width) return false;

    size_t hash = g_currentMapHash;
    if (hash == 0) {
        for (const auto& r : mapMatrix) {
            hash ^= std::hash<std::string_view>{}(r) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        }
    }

    if (hash != lastMapHash) {
        lastMapHash = hash;
        cachedLabels.assign(height, std::vector<char>(width, 2));
    }

    if (cachedLabels[mapY][mapX] != 2) {
        return cachedLabels[mapY][mapX] == 1;
    }

    char c = mapMatrix[mapY][mapX];
    bool result = false;
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
        if (isEntity(c)) {
            bool hasAdjacentText = false;
            for (int dx = -1; dx <= 1; dx += 2) {
                int nx = mapX + dx;
                if (mapY >= 0 && mapY < height && nx >= 0 && nx < (int)mapMatrix[mapY].size()) {
                    char adj = mapMatrix[mapY][nx];
                    if (adj == '^' || (adj >= 'a' && adj <= 'z')) {
                        hasAdjacentText = true;
                        break;
                    }
                }
            }
            if (!hasAdjacentText) {
                for (int dy = -1; dy <= 1; dy += 2) {
                    int ny = mapY + dy;
                    if (ny >= 0 && ny < height && mapX >= 0 && mapX < (int)mapMatrix[ny].size()) {
                        char adj = mapMatrix[ny][mapX];
                        if (adj == '^') {
                            hasAdjacentText = true;
                            break;
                        }
                    }
                }
            }
            result = hasAdjacentText;
        } else {
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    if (dx == 0 && dy == 0) continue;
                    int nx = mapX + dx;
                    int ny = mapY + dy;
                    if (ny >= 0 && ny < height && nx >= 0 && nx < (int)mapMatrix[ny].size()) {
                        char adj = mapMatrix[ny][nx];
                        if (adj == '^' || (adj >= 'a' && adj <= 'z')) {
                            result = true;
                            break;
                        }
                    }
                }
                if (result) break;
            }
        }
    }
    cachedLabels[mapY][mapX] = result ? 1 : 0;
    return result;
}

Pixel3D RaycasterWorld::getInternalWallPixel(const std::string& titleMap, bool themeForest, float distanceUntilWall, float depthMaximum, char charWall, int y, int ceiling, int floor, float texX, float timeAnimation, const Highlighter::InfoLight& infoLight, float hitX, float hitY, bool isSideWall, char npcFound, float nx, float ny) {
    (void)isSideWall; (void)distanceUntilWall; (void)depthMaximum; (void)timeAnimation;
    const auto& flags = getFlagsMap(titleMap);
    int heightWall = floor - ceiling;
    
    float texY = 0.0f;
    if (heightWall > 0) texY = (float)(y - ceiling) / (float)heightWall;
    if (texY > 0.999f) texY = 0.999f;
    int tx = (int)(texX * 128.0f) % 128;
    int ty = (int)(texY * 128.0f) % 128;

    bool isKingdom = flags.isKingdom;
    bool isStructure = (charWall == '|' || charWall == '_' || charWall == '[' || charWall == ']' || charWall == '{' || charWall == '}' || charWall == '/' || charWall == '\\' || charWall == '<' || charWall == '>' || charWall == ';' || charWall == '=' || charWall == '-' || charWall == ':' || charWall == '+');
    bool isLabyrinthArch = (!isKingdom && themeForest && charWall == '#' && hitX >= 125.0f && hitX <= 150.0f && hitY >= 5.0f && hitY <= 15.0f);

    TexID texID = TexID::WallInvalidates;

    if (flags.isLabyrinth) {
        texID = TexID::LabyrinthWood;
    } else if (isLabyrinthArch) {
        int mapX = (int)hitX;
        int mapY = (int)hitY;
        bool isPillar = (std::abs(mapY - 12) == 1 && mapX >= 132 && mapX <= 136);
        if (isPillar) texID = TexID::LabyrinthBowPillar;
        else texID = TexID::LabyrinthBowBackground;
    } else if (isStructure && npcFound == 'M') {
        texID = TexID::MorganaWood;
    } else if (isKingdom && (isStructure || charWall == '#' || charWall == '+')) {
        if (charWall == '+') {
            if (flags.isChurch) texID = TexID::ChurchWallAltar;
        } else if (charWall == '|') {
            if (flags.isChurch) texID = TexID::ChurchStainedglass;
            else if (flags.isBridge) texID = TexID::BridgeWood;
            else {
                if (npcFound == 'Q') texID = TexID::Alchemist;
                else if (npcFound == 'I' || npcFound == 'P') texID = TexID::EntryChurch;
                else if (npcFound == 'A' || npcFound == 'N') texID = TexID::MannequinAnok;
                else if (npcFound == 'F') texID = TexID::Francesco;
                else if (npcFound == 'B') texID = TexID::Kiss;
                else if (npcFound == 'C') texID = TexID::Knight;
                else texID = TexID::KingdomWood;
            }
        } else {
            if (flags.isChurch) {
                if (hitX < 10.0f) texID = TexID::ChurchAltar;
                else texID = TexID::ChurchWall;
            } else {
                bool isBattlementGap = (ty < 12 && (tx % 32) >= 16);
                if (isBattlementGap) {
                    Pixel3D px;
                    px.isBackground = true;
                    return px;
                }
                texID = TexID::PatioWall;
            }
        }
    } else if (isStructure) {
        if (themeForest) texID = TexID::ForestStructure;
        else texID = TexID::DarkBricks;
    } else if (!isKingdom && (themeForest || flags.isEarth) && charWall == 'T') {
        if (flags.isHeart) texID = TexID::TreeHeart;
        else texID = TexID::TreeForest;
    } else if (charWall == '#') {
        if (flags.isForest) texID = TexID::TreeForest;
        else texID = TexID::StoneVillage;
    } else if (charWall == 'T') {
        if (flags.isChurch) texID = TexID::ChurchCeiling;
        else texID = TexID::PatternStructure; // Fallback padrao para T
    } else if (!isKingdom) {
        if (flags.isSpawn) texID = TexID::StoneSpawn;
        else if (flags.isRoomBoss) texID = TexID::RoomBossWall;
        else if (flags.isCave) {
            if (flags.isHeart) texID = TexID::CaveHeartWall;
            else texID = TexID::StoneVillage;
        } else {
            if (flags.isForest) texID = TexID::TreeForest;
            else texID = TexID::StoneVillage;
        }
    } else {
        if (flags.isForest) texID = TexID::TreeForest;
        else texID = TexID::StoneVillage;
    }

    ColorRGB color = ManagerTextures::getColor(texID, tx, ty);
    return Highlighter::applyLightPrecalculated(color.r, color.g, color.b, infoLight, false, true, nx, ny);
}
Pixel3D RaycasterWorld::getInternalWallPixel(const std::string& titleMap, bool themeForest, float distanceUntilWall, float depthMaximum, char charWall, int y, int ceiling, int floor, float texX, float timeAnimation, const std::vector<std::tuple<int, int, int>>& lights, float hitX, float hitY, bool isSideWall, char npcFound, float nx, float ny) {
    const auto& flags = getFlagsMap(titleMap);
    Highlighter::InfoLight info = Highlighter::calculateInfoLight(distanceUntilWall * 0.55f, depthMaximum, flags.themeSky, lights, hitX, hitY, nullptr, timeAnimation);
    return getInternalWallPixel(titleMap, themeForest, distanceUntilWall, depthMaximum, charWall, y, ceiling, floor, texX, timeAnimation, info, hitX, hitY, isSideWall, npcFound, nx, ny);
}

Pixel3D RaycasterWorld::getPixelWall(const std::string& titleMap, bool themeForest, float distanceUntilWall, float depthMaximum, char charWall, int y, int ceiling, int floor, float texX, float timeAnimation, bool isSideWall, const Highlighter::InfoLight& infoLight, float hitX, float hitY, char npcFound, float nx, float ny) {
    return getInternalWallPixel(titleMap, themeForest, distanceUntilWall, depthMaximum, charWall, y, ceiling, floor, texX, timeAnimation, infoLight, hitX, hitY, isSideWall, npcFound, nx, ny);
}

Pixel3D RaycasterWorld::getPixelWall(const std::string& titleMap, bool themeForest, float distanceUntilWall, float depthMaximum, char charWall, int y, int ceiling, int floor, float texX, float timeAnimation, bool isSideWall, const std::vector<std::tuple<int, int, int>>& lights, float hitX, float hitY, char npcFound, float nx, float ny) {
    return getInternalWallPixel(titleMap, themeForest, distanceUntilWall, depthMaximum, charWall, y, ceiling, floor, texX, timeAnimation, lights, hitX, hitY, isSideWall, npcFound, nx, ny);
}


Pixel3D RaycasterWorld::getFloorPixel(const std::string& titleMap, float currentX, float currentY, float currentDist, float depthMaximum, const std::vector<std::tuple<int, int, int>>& lights, const std::vector<std::string>* mapMatrix, float timeAnimation) {
    const auto& flags = getFlagsMap(titleMap);
    int themeSky = flags.themeSky;
    currentDist *= 0.55f;

    bool isEarth = flags.isEarth;
    bool isLabyrinth = flags.isLabyrinth;
    bool isRoomBoss = flags.isRoomBoss;
    bool isHeart = flags.upperTitle.find("CORACAO") != std::string::npos;

    unsigned int globX = static_cast<unsigned int>(std::abs(currentX * 128.0f));
    unsigned int globY = static_cast<unsigned int>(std::abs(currentY * 128.0f));
    int tx = globX & 127;
    int ty = globY & 127;

    TexID texID = TexID::FloorPattern;
    char c = ' ';
    uint8_t fgR = 0, fgG = 0, fgB = 0;
    uint8_t bgR = 0, bgG = 0, bgB = 0;
    bool isProcedural = false;

    if (isLabyrinth) {
        fgR = 150; fgG = 130; fgB = 90;
        bool edgeX = ((globX & 127) < 2) || ((globX & 127) > 125);
        bool edgeY = ((globY & 31) < 2) || ((globY & 31) > 29);
        if (edgeX || edgeY) texID = TexID::FloorLabyrinthEdge;
        else texID = TexID::FloorLabyrinth;
    } else if (isRoomBoss) {
        float cx = (globX & 127) - 64.0f;
        float cy = (globY & 127) - 64.0f;
        float dist = std::sqrt(cx*cx + cy*cy);
        float angle = std::atan2(cy, cx);
        float spiral = ManagerTextures::fastYes(dist * 0.4f - angle * 3.0f);
        fgR = 50; fgG = 50; fgB = 50; 
        if (spiral > 0.3f) c = '@';
        else if (spiral > 0.0f) c = '%';
        else if (spiral > -0.3f) c = '.';
        if (spiral > 0.0f) texID = TexID::FloorRoomBossInside;
        else texID = TexID::FloorRoomBossOut;
    } else if (isHeart) {
        float cx = (globX & 255) - 128.0f;
        float cy = (globY & 255) - 128.0f;
        float dist = std::sqrt(cx*cx + cy*cy);
        float angle = std::atan2(cy, cx);
        float spiral = ManagerTextures::fastYes(dist * 0.2f + angle * 4.0f + globX * 0.1f);
        bool hasMoss = ((globX * 17 + globY * 13) % 100) < 40 || (spiral > 0.5f);
        if (hasMoss) { texID = TexID::FloorHeartMoss; fgR = 30; fgG = 80; fgB = 20; }
        else if (spiral > 0.0f) { texID = TexID::FloorHeartEarth; fgR = 50; fgG = 30; fgB = 15; }
        else { texID = TexID::FloorHeartDark; fgR = 25; fgG = 15; fgB = 10; }
    } else if (flags.isKingdom || flags.isBridge) {
        float cx = globX * 0.15f;
        float cy = globY * 0.15f;
        float noise = ManagerTextures::fastYes(cx) * ManagerTextures::fastYes(cy);
        
        int row = globY / 24;
        int offset = (row % 2 == 0) ? 0 : 12;
        float blockX = ((globX + offset) % 24) / 24.0f;
        float blockY = (globY % 24) / 24.0f;
        
        bool edgeTile = (blockX < 0.1f || blockX > 0.9f || blockY < 0.1f || blockY > 0.9f);
        
        if (edgeTile) {
            bgR = 20; bgG = 20; bgB = 22; 
        } else {
            int var = (int)(noise * 6.0f);
            bgR = 38 + var; bgG = 40 + var; bgB = 43 + var; 
        }
        c = ' ';
        isProcedural = true;
    } else if (flags.isCave && !isHeart) {
        float cx = globX * 0.06f;
        float cy = globY * 0.06f;
        float cx2 = (globX - globY) * 0.03f;
        float noise = ManagerTextures::fastYes(cx) + ManagerTextures::fastYes(cy) + ManagerTextures::fastYes(cx2);
        
        int var = (int)(noise * 5.0f);
        bgR = 30 + var; bgG = 30 + var; bgB = 30 + var;
        c = ' ';
        isProcedural = true;
    } else if (isEarth) {
        float cx = globX * 0.123f;
        float cy = globY * 0.091f;
        float cx2 = (globX + globY) * 0.054f;
        float cy2 = ((float)globX - globY) * 0.111f;
        float noise = ManagerTextures::fastYes(cx) + ManagerTextures::fastYes(cy) + ManagerTextures::fastYes(cx2) + ManagerTextures::fastYes(cy2);
        bool isGrass = (noise > -3.0f);
        if (isGrass) {
            if (flags.upperTitle.find("FLORESTA") != std::string::npos) { fgR = 6; fgG = 35; fgB = 6; texID = TexID::FloorGrassForest; }
            else { fgR = 12; fgG = 75; fgB = 12; texID = TexID::FloorGrassVillage; }
        } else {
            fgR = 45; fgG = 25; fgB = 10; texID = TexID::FloorEarth;
        }
    } else {
        fgR = 60; fgG = 60; fgB = 60; texID = TexID::FloorPattern;
        if (((globX * 17 + globY * 23) & 63) < 4) c = '.';
        else if (((globX * globX + globY * 13) & 63) < 3) c = '-';
        else if (((globX * 3 + globY * 7) & 31) < 2) c = '`';
    }
    
    ColorRGB color;
    if (isProcedural) {
        color.r = bgR; color.g = bgG; color.b = bgB;
    } else {
        color = ManagerTextures::getColor(texID, tx, ty);
    }
    
    Pixel3D px = Highlighter::applyFog(color.r, color.g, color.b, currentDist, depthMaximum, themeSky, lights, currentX, currentY, false, mapMatrix, false, 0.0f, 0.0f, timeAnimation);
    if (c != ' ' && currentDist <= depthMaximum * 0.5f) {
        px.ch = c;
        px.fgR = fgR;
        px.fgG = fgG;
        px.fgB = fgB;
        px.hasFg = true;
    }
    return px;
}
Pixel3D RaycasterWorld::getFloorPixel(const std::string& titleMap, float currentX, float currentY, float currentDist, float depthMaximum, const Highlighter::InfoLight& infoLight) {
    const auto& flags = getFlagsMap(titleMap);
    currentDist *= 0.55f;

    bool isEarth = flags.isEarth;
    bool isLabyrinth = flags.isLabyrinth;
    bool isRoomBoss = flags.isRoomBoss;
    bool isHeart = flags.upperTitle.find("CORACAO") != std::string::npos;

    unsigned int globX = static_cast<unsigned int>(std::abs(currentX * 128.0f));
    unsigned int globY = static_cast<unsigned int>(std::abs(currentY * 128.0f));

    char c = ' ';
    int r = 0, g = 0, b = 0;
    uint8_t fgR = 0, fgG = 0, fgB = 0;

    if (isLabyrinth) {
        fgR = 150; fgG = 130; fgB = 90;
        bool edgeX = ((globX & 127) < 2) || ((globX & 127) > 125);
        bool edgeY = ((globY & 31) < 2) || ((globY & 31) > 29);
        if (edgeX || edgeY) {
            r = 40; g = 40; b = 30;
            c = ' ';
        } else {
            if (((globX + globY) & 1) == 0) { r = 180; g = 160; b = 110; }
            else                          { r = 160; g = 140; b = 95; }
            c = ' ';
        }
    } else if (isRoomBoss) {
        float cx = (globX & 127) - 64.0f;
        float cy = (globY & 127) - 64.0f;
        float dist = std::sqrt(cx*cx + cy*cy);
        float angle = std::atan2(cy, cx);
        float spiral = std::sin(dist * 0.4f - angle * 3.0f);

        r = 5; g = 5; b = 5;
        fgR = 50; fgG = 50; fgB = 50; 
        if (spiral > 0.3f) c = '@';
        else if (spiral > 0.0f) c = '%';
        else if (spiral > -0.3f) c = '.';
        else c = ' ';
    } else if (isHeart) {
        // Chao de musgo e terra para Coracao da Floresta
        float cx = (globX & 255) - 128.0f;
        float cy = (globY & 255) - 128.0f;
        float dist = std::sqrt(cx*cx + cy*cy);
        float angle = std::atan2(cy, cx);
        float spiral = std::sin(dist * 0.2f + angle * 4.0f + globX * 0.1f);
        
        bool hasMoss = ((globX * 17 + globY * 13) % 100) < 40 || (spiral > 0.5f);
        
        if (hasMoss) {
            r = 30; g = 80; b = 20; // Verde musgo
            fgR = 30; fgG = 80; fgB = 20;
        } else if (spiral > 0.0f) {
            r = 50; g = 30; b = 15; // Madeira/Terra marrom
            fgR = 50; fgG = 30; fgB = 15;
        } else {
            r = 25; g = 15; b = 10; // Madeira/Terra escura
            fgR = 25; fgG = 15; fgB = 10;
        }
        c = ' ';
    } else if (flags.isKingdom || flags.isBridge) {
        float cx = globX * 0.15f;
        float cy = globY * 0.15f;
        float noise = std::sin(cx) * std::sin(cy);
        
        int row = globY / 24;
        int offset = (row % 2 == 0) ? 0 : 12;
        float blockX = ((globX + offset) % 24) / 24.0f;
        float blockY = (globY % 24) / 24.0f;
        
        bool edgeTile = (blockX < 0.1f || blockX > 0.9f || blockY < 0.1f || blockY > 0.9f);
        
        if (edgeTile) {
            r = 20; g = 20; b = 22; 
        } else {
            int var = (int)(noise * 6.0f);
            r = 38 + var; g = 40 + var; b = 43 + var; 
        }
        c = ' ';
    } else if (flags.isCave && !isHeart) {
        float cx = globX * 0.06f;
        float cy = globY * 0.06f;
        float cx2 = (globX - globY) * 0.03f;
        float noise = std::sin(cx) + std::sin(cy) + std::sin(cx2);
        
        int var = (int)(noise * 5.0f);
        r = 30 + var; g = 30 + var; b = 30 + var;
        c = ' ';
    } else if (isEarth) {
        /*
         * Mistura de terra e grama para Vila e Floresta
         * Formula caotica sem padrao repetitivo
         */
        float cx = globX * 0.123f;
        float cy = globY * 0.091f;
        float cx2 = (globX + globY) * 0.054f;
        float cy2 = ((float)globX - globY) * 0.111f;
        float noise = std::sin(cx) + std::sin(cy) + std::sin(cx2) + std::sin(cy2);
        
        bool isGrass = (noise > -3.0f); // Mais grama do que terra
        
        if (isGrass) {
            if (flags.upperTitle.find("FLORESTA") != std::string::npos) { fgR = 6; fgG = 35; fgB = 6; }
            else { fgR = 12; fgG = 75; fgB = 12; }
            
            if (flags.upperTitle.find("FLORESTA") != std::string::npos) {
                bool isPatch = ((globX / 16) + (globY / 16)) % 2 == 0;
                if (isPatch) { r = 5; g = 28; b = 5; } else { r = 4; g = 24; b = 4; }
            } else {
                bool isPatch = ((globX / 16) + (globY / 16)) % 2 == 0;
                if (isPatch) { r = 12; g = 65; b = 12; } else { r = 10; g = 58; b = 10; }
            }
            c = ' ';
        } else {
            fgR = 45; fgG = 25; fgB = 10; // Marrom terra
            if (((globX + globY) & 1) == 0) { r = 28; g = 18; b = 8; }
            else if (((globX * 3 + globY * 5) & 7) < 2) { r = 22; g = 12; b = 4; }
            else { r = 25; g = 15; b = 5; }
            c = ' ';
        }
    } else {
        fgR = 60; fgG = 60; fgB = 60;
        if (((globX + globY) & 1) == 0) { r = 24; g = 24; b = 24; }
        else if (((globX * 3 + globY * 5) & 7) < 2) { r = 16; g = 16; b = 16; }
        else { r = 20; g = 20; b = 20; }
        
        if (((globX * 17 + globY * 23) & 63) < 4) c = '.';
        else if (((globX * globX + globY * 13) & 63) < 3) c = '-';
        else if (((globX * 3 + globY * 7) & 31) < 2) c = '`';
    }
    
    Highlighter::InfoLight infoLocation = infoLight;
    infoLocation.fogPercentage = std::min(1.0f, (currentDist / (depthMaximum * 0.8f)) *
                                             (currentDist / (depthMaximum * 0.8f)));
    Pixel3D px = Highlighter::applyLightPrecalculated(r, g, b, infoLocation);
    if (c != ' ' && currentDist <= depthMaximum * 0.5f) {
        px.ch = c;
        px.fgR = fgR;
        px.fgG = fgG;
        px.fgB = fgB;
        px.hasFg = true;
    }
    return px;
}

Pixel3D RaycasterWorld::getPixelWater(float currentX, float currentY, float currentDist, float depthMaximum, float radiusAngle, float timeAnimation, int themeSky) {
    int baseR=0, baseG=0, baseB=0;
    currentDist *= 0.55f;

    float waveX = ManagerTextures::fastYes(currentX * 4.0f + timeAnimation * 2.0f);
    float waveY = ManagerTextures::fastCos(currentY * 4.0f + timeAnimation * 1.5f);
    float wave = (waveX + waveY) * 0.5f; 

    if (wave > 0.3f) {
        baseR = 100; baseG = 200; baseB = 255;
    } else if (wave > -0.3f) {
        baseR = 60; baseG = 160; baseB = 235;
    } else {
        baseR = 30; baseG = 130; baseB = 215;
    }
    
    float angleOffset = wave * 0.2f; 
    float angReflection = radiusAngle + angleOffset;
    while (angReflection >= 2.0f * 3.14159f) angReflection -= 2.0f * 3.14159f;
    while (angReflection < 0) angReflection += 2.0f * 3.14159f;
    
    if (angReflection < 0.3f || angReflection > (2.0f * 3.14159f - 0.3f)) {
        float dif = (angReflection < 0.3f) ? angReflection : ((2.0f * 3.14159f) - angReflection);
        float intensityReflection = 1.0f - (dif / 0.3f);
        intensityReflection *= (0.5f + (wave + 1.0f) * 0.25f); 
        
        baseR = std::min(255, baseR + (int)(155 * intensityReflection));
        baseG = std::min(255, baseG + (int)(95 * intensityReflection));
        if (themeSky != 1 && themeSky != 2) baseB = std::min(255, baseB + (int)(255 * intensityReflection)); 
    }

    std::vector<std::tuple<int, int, int>> noLights;
    Highlighter::InfoLight info = Highlighter::calculateInfoLight(currentDist, depthMaximum, themeSky, noLights, currentX, currentY, nullptr, timeAnimation);
    
    float nightFactor = std::max(0.4f, info.sunIntensity);
    baseR = (int)(baseR * nightFactor);
    baseG = (int)(baseG * nightFactor);
    baseB = (int)(baseB * nightFactor);
    
    float nf = info.fogPercentage;
    float endR = baseR * (1.0f - nf) + info.fogR * nf;
    float endG = baseG * (1.0f - nf) + info.fogG * nf;
    float endB = baseB * (1.0f - nf) + info.fogB * nf;
    
    Pixel3D px;
    px.r = (uint8_t)std::min(255, std::max(0, (int)endR));
    px.g = (uint8_t)std::min(255, std::max(0, (int)endG));
    px.b = (uint8_t)std::min(255, std::max(0, (int)endB));
    px.ch = ' ';
    return px;
}
int RaycasterWorld::getThemeSky(const std::string& titleMap) {
    const auto& flags = getFlagsMap(titleMap);
    return flags.themeSky;
}

Pixel3D RaycasterWorld::getPixelCeiling(int themeSky, float radiusAngle, float angleSky, int y, int heightScreen, float timeAnimation, bool isMenu, float overrideSunAngle, float overrideSunRatioY, float overrideMoonAngle, float overrideMoonRatioY) {
    (void)angleSky; (void)timeAnimation; (void)isMenu;
    int horizon = heightScreen / 2;
    if (themeSky == 3) { 
        Pixel3D px;
        float ratioY = (horizon > 0) ? (float)y / (float)horizon : 1.0f;
        int tx = (int)(radiusAngle * 60.0f) % 128;
        int ty = (int)(ratioY * 60.0f) % 128;
        if (tx < 0) tx += 128;
        if (ty < 0) ty += 128;
        
        TexID texID = TexID::CeilingIndoorsPattern;
        bool isHeart = g_currentMapTitle.find("CORACAO") != std::string::npos;
        bool isRoomBoss = g_currentMapTitle.find("CAVERNA") != std::string::npos || g_currentMapTitle.find("CHEFE") != std::string::npos;
        
        if (isHeart) {
            float cx = (tx - 64.0f);
            float cy = (ty - 64.0f);
            float dist = std::sqrt(cx*cx + cy*cy);
            float angle = std::atan2(cy, cx);
            float spiral = ManagerTextures::fastYes(dist * 0.2f + angle * 4.0f + tx * 0.1f);
            
            bool hasMoss = ((tx * 17 + ty * 13) % 100) < 20 || (spiral > 0.8f);
            
            if (hasMoss) texID = TexID::CeilingIndoorsHeartMoss;
            else if (spiral > 0.0f) texID = TexID::CeilingIndoorsHeartWood;
            else texID = TexID::CeilingIndoorsHeartDark;
        } else if (isRoomBoss) {
            texID = TexID::RoomBossWall;
        }
        ColorRGB color = ManagerTextures::getColor(texID, tx, ty);
        px.r = color.r; px.g = color.g; px.b = color.b;
        px.ch = ' '; px.isBackground = false;
        return px;
    }


    float ratio = (horizon > 0) ? (float)y / (float)horizon : 1.0f;
    if (ratio < 0.0f) ratio = 0.0f;
    if (ratio > 1.0f) ratio = 1.0f;

    struct ColorSky { float topR, topG, topB, botR, botG, botB; };
    auto mixColor = [](ColorSky a, ColorSky b, float t) -> ColorSky {
        return { a.topR + (b.topR - a.topR) * t, a.topG + (b.topG - a.topG) * t, a.topB + (b.topB - a.topB) * t,
                 a.botR + (b.botR - a.botR) * t, a.botG + (b.botG - a.botG) * t, a.botB + (b.botB - a.botB) * t };
    };

    ColorSky beborn = { 20, 20, 60,   255, 120, 50 };
    ColorSky day =    { 10, 60, 150,  70, 150, 230 };
    ColorSky bySun = { 40, 10, 60,   255, 80, 20 };
    ColorSky night =  { 2, 2, 5,      5, 25, 45 };

    float t = std::fmod(timeAnimation, 120.0f) / 120.0f;
    ColorSky colorCurrent;
    if (t < 0.1f)      colorCurrent = mixColor(beborn, day, t / 0.1f);
    else if (t < 0.4f) colorCurrent = day;
    else if (t < 0.5f) colorCurrent = mixColor(day, bySun, (t - 0.4f) / 0.1f);
    else if (t < 0.6f) colorCurrent = mixColor(bySun, night, (t - 0.5f) / 0.1f);
    else if (t < 0.9f) colorCurrent = night;
    else               colorCurrent = mixColor(night, beborn, (t - 0.9f) / 0.1f);

    int r = (int)(colorCurrent.topR * (1.0f - ratio) + colorCurrent.botR * ratio);
    int g = (int)(colorCurrent.topG * (1.0f - ratio) + colorCurrent.botG * ratio);
    int b = (int)(colorCurrent.topB * (1.0f - ratio) + colorCurrent.botB * ratio);

    float divHorizon = (horizon > 0) ? (float)horizon : 1.0f;
    float ratioY = (float)y / divHorizon;
    float globalRotation = timeAnimation * 0.05f;

    float diffAngleSun;
    float distYSun;
    
    if (overrideSunAngle != -100.0f && overrideSunRatioY != -100.0f) {
        diffAngleSun = radiusAngle - overrideSunAngle;
        while (diffAngleSun < -3.14159f) diffAngleSun += 2.0f * 3.14159f;
        while (diffAngleSun > 3.14159f) diffAngleSun -= 2.0f * 3.14159f;
        distYSun = ratioY - overrideSunRatioY;
    } else {
        if (isMenu) {
            // No menu o Sol nasce na esquerda (colAng=-0.8) as t=0.0 e se poe na direita (colAng=+0.8) as t=0.5
            float colAng = radiusAngle;
            diffAngleSun = colAng - (t - 0.25f) * 3.2f;
        } else {
            diffAngleSun = std::fmod(radiusAngle - globalRotation, 2.0f * 3.14159f);
            if (diffAngleSun < -3.14159f) diffAngleSun += 2.0f * 3.14159f;
            if (diffAngleSun > 3.14159f) diffAngleSun -= 2.0f * 3.14159f;
        }
        
        float sunPhase = (t - 0.25f) * 2.0f * 3.14159f;
        float sunElevation = std::cos(sunPhase); 
        distYSun = isMenu ? (ratioY - (0.5f - 0.2f * sunElevation)) : (ratioY - (0.9f - 1.1f * sunElevation));
    }
    float distSun = std::sqrt(diffAngleSun * diffAngleSun * 6.0f + distYSun * distYSun);
    
    float diffAngleMoon;
    float distYMoon;
    
    if (overrideMoonAngle != -100.0f && overrideMoonRatioY != -100.0f) {
        diffAngleMoon = radiusAngle - overrideMoonAngle;
        while (diffAngleMoon < -3.14159f) diffAngleMoon += 2.0f * 3.14159f;
        while (diffAngleMoon > 3.14159f) diffAngleMoon -= 2.0f * 3.14159f;
        distYMoon = ratioY - overrideMoonRatioY;
    } else {
        if (isMenu) {
            float colAng = radiusAngle;
            diffAngleMoon = colAng - (t - 0.75f) * 3.2f;
        } else {
            diffAngleMoon = std::fmod(radiusAngle - globalRotation + 3.14159f, 2.0f * 3.14159f);
            if (diffAngleMoon < -3.14159f) diffAngleMoon += 2.0f * 3.14159f;
            if (diffAngleMoon > 3.14159f) diffAngleMoon -= 2.0f * 3.14159f;
        }
        
        float moonPhase = (t - 0.75f) * 2.0f * 3.14159f;
        float moonElevation = std::cos(moonPhase);
        distYMoon = isMenu ? (ratioY - (0.5f - 0.2f * moonElevation)) : (ratioY - (0.9f - 1.1f * moonElevation));
    }
    float distMoon = std::sqrt(diffAngleMoon * diffAngleMoon * 6.0f + distYMoon * distYMoon);

    Pixel3D px;
    px.ch = ' '; px.hasFg = false; px.isBackground = false;

    // Draw Moon
    float moonAlpha = 1.0f;
    float moonGlowRadius = 0.11f;

    if (distMoon < 0.10f) {
        float shadowOffset = 0.04f + std::sin(timeAnimation * 0.5f) * 0.01f; 
        float shadowDist = std::sqrt((diffAngleMoon - shadowOffset) * (diffAngleMoon - shadowOffset) * 6.0f + distYMoon * distYMoon);
        if (shadowDist < 0.10f) {
            px.r = 25; px.g = 30; px.b = 40; return px;
        } else {
            float lunarX = diffAngleMoon * 20.0f;
            float lunarY = distYMoon * 20.0f;
            float rotation = timeAnimation * 0.15f; 
            float maryNoise = std::sin(lunarX * 3.0f + lunarY + rotation) * std::cos(lunarY * 4.0f - lunarX - rotation) 
                             + std::sin(lunarX * 7.0f - rotation);
            if (maryNoise > 0.4f) {
                px.r = 140; px.g = 150; px.b = 180;
            } else {
                px.r = 240; px.g = 245; px.b = 255;
            }
            return px;
        }
    } else if (distMoon < moonGlowRadius) {
        float coronaLerp = (distMoon - 0.10f) / (moonGlowRadius - 0.10f);
        px.r = 240 - (int)(40 * coronaLerp); 
        px.g = 245 - (int)(35 * coronaLerp); 
        px.b = 255 - (int)(15 * coronaLerp); 
        return px;
    } else if (distMoon < 0.45f) {
        float glowPulse = std::sin(timeAnimation * 1.5f) * 0.02f;
        float glowDist = (distMoon - moonGlowRadius) / (0.45f - moonGlowRadius + glowPulse);
        float glow = std::max(0.0f, 1.0f - glowDist);
        glow = glow * glow; 
        
        if (glow > 0.0f) {
            r = std::min(255, r + (int)(120 * glow * moonAlpha));
            g = std::min(255, g + (int)(150 * glow * moonAlpha));
            b = std::min(255, b + (int)(220 * glow * moonAlpha));
        }
    }

    // Draw Sun
    float sunAlpha = 1.0f;
    float angleSun = std::atan2(distYSun, diffAngleSun * 2.449f);
    float rays = std::sin(angleSun * 12.0f + timeAnimation * 1.5f) * 0.5f 
               + std::sin(angleSun * 7.0f - timeAnimation * 0.8f) * 0.5f;
    float glowRadius = 0.12f + rays * 0.025f;
    
    if (distSun < 0.08f) {
        px.r = 255; px.g = 255; px.b = 255; return px;
    } else if (distSun < glowRadius) {
        float coronaLerp = (distSun - 0.08f) / (glowRadius - 0.08f);
        px.r = 255; 
        px.g = 255 - (int)(35 * coronaLerp); 
        px.b = 255 - (int)(205 * coronaLerp); 
        return px;
    } else if (distSun < 0.45f) {
        float glowPulse = std::sin(timeAnimation * 2.0f) * 0.03f;
        float glowDist = (distSun - glowRadius) / (0.45f - glowRadius + glowPulse);
        float glow = std::max(0.0f, 1.0f - glowDist);
        glow = glow * glow; 
        
        if (glow > 0.0f) {
            r = std::min(255, r + (int)(180 * glow * sunAlpha));
            g = std::min(255, g + (int)(110 * glow * sunAlpha));
            b = std::min(255, b + (int)(20 * glow * sunAlpha));
        }
    }

    // Clouds
    float wind = timeAnimation * 0.05f; 
    float angleBase = radiusAngle + wind;
    float cloudNoise = std::sin(angleBase * 5.0f) * std::sin((y + wind*10.0f) * 0.1f) 
                     + 0.5f * std::sin(angleBase * 11.0f + (y - wind*5.0f) * 0.13f)
                     + 0.25f * std::sin(angleBase * 23.0f - (y + wind*15.0f) * 0.21f);
    if (cloudNoise > 0.7f) {
        float cloudIntensity = std::min(1.0f, (cloudNoise - 0.7f) * 2.0f);
        cloudIntensity *= (0.5f + (ratio * 0.5f));
        r = r + (int)((255 - r) * cloudIntensity);
        g = g + (int)((255 - g) * cloudIntensity);
        b = b + (int)((255 - b) * cloudIntensity);
    }

    // Stars (only at night)
    if (t > 0.5f && t < 0.95f) {
        float starAlpha = (t > 0.6f && t < 0.85f) ? 1.0f : 0.5f;
        unsigned int starX = static_cast<unsigned int>(radiusAngle * 150.0f);
        unsigned int hash = starX * 374761393U + y * 668265263U;
        hash = (hash ^ (hash >> 13)) * 1274126177U;
        int noise = hash % 1000;
        
        if (noise == 0 && y <= horizon) { px.ch = '*'; px.fgR = 255; px.fgG = 255; px.fgB = 255; px.hasFg = true; }
        else if (noise < 3 && y <= horizon) { px.ch = '+'; px.fgR = 200; px.fgG = 200; px.fgB = 255; px.hasFg = true; }
        else if (noise < 10 && y <= horizon) { px.ch = '.'; px.fgR = 255; px.fgG = 255; px.fgB = 255; px.hasFg = true; }
        else if (noise < 17 && y <= horizon) { px.ch = '.'; px.fgR = 200; px.fgG = 200; px.fgB = 200; px.hasFg = true; }
        else if (noise == 20 && y <= horizon) { px.ch = '\''; px.fgR = 255; px.fgG = 255; px.fgB = 150; px.hasFg = true; }
        
        if (px.ch != ' ') {
            px.fgR = (uint8_t)(px.fgR * starAlpha);
            px.fgG = (uint8_t)(px.fgG * starAlpha);
            px.fgB = (uint8_t)(px.fgB * starAlpha);
            px.r = r; px.g = g; px.b = b;
            return px;
        }
    }

    px.r = r; px.g = g; px.b = b;
    return px;
}

char RaycasterWorld::getSpriteChar(int /*mapX*/, int mapY, char c, const std::string& titleMap) {
    if (c == '^') {
        const auto& flags = getFlagsMap(titleMap);
        if (flags.upperTitle.find("VILA") != std::string::npos) return '2';
        if (flags.upperTitle.find("FLORESTA") != std::string::npos) {
            if (mapY > 15) return '1'; 
            return '5'; 
        }
        if (flags.upperTitle.find("REINO") != std::string::npos) return '1';
        return '^';
    }

    if (c == '!' || c == '%') {
        return c; // Retorna ! ou % para serem desenhados como sprite pelo RaycasterRenderizador (IDE)
    }
    if (c == '@') {
        return '@'; // Terminal hackeavel
    }
    if (c == 'Y' || c == '*') {
        return c;
    }
    const auto& flags = getFlagsMap(titleMap);
    if (flags.upperTitle.find("IGREJA") != std::string::npos) {
        if (c == 'P') return 'J'; // Padre Benedito
    }

    if ((flags.upperTitle.find("VILA") != std::string::npos || flags.upperTitle.find("CASA") != std::string::npos) && c == 'F') {
        return 'V';
    }
    if (flags.upperTitle.find("SALA DO CHEFE") != std::string::npos && (c == 'M' || c == 'A' || c == 'H' || c == 'O' || c == 'R' || c == 'G')) {
        return 'H';
    }
    if ((flags.upperTitle.find("REINO") != std::string::npos || flags.upperTitle.find("PATIO") != std::string::npos) && c == 'N') {
        return 'Z';
    }
    if ((flags.upperTitle.find("CABANA") != std::string::npos || flags.upperTitle.find("FLORESTA") != std::string::npos) && c == 'M') {
        return 'W';
    }
    if (flags.upperTitle.find("LABIRINTO") != std::string::npos && c == 'B') {
        return 'X';
    }

    // Customizacoes para o PATIO DO REINO (e REINO) e Igreja
    if (flags.upperTitle.find("PATIO DO REINO") != std::string::npos || flags.upperTitle == "REINO") {
        if (c == 'F') return 'V'; // Franchesco
        if (c == 'N') return 'Z'; // Anok (Manequim)
        if (c == 'Q') return 'Q'; // Alquimista
        if (c == 'C') return 'C'; // Cavaleiro de Treino
        if (c == 'I') return 'Y'; // Igreja (Capela)
        if (c == 'B') return 'B'; // Bjorn
    }
    if (flags.upperTitle.find("IGREJA") != std::string::npos) {
        if (c == 'P') return 'J'; // Padre Benedito
    }

    bool isKingdom = flags.isKingdom;
    if (isKingdom && flags.upperTitle.find("PATIO DO REINO") == std::string::npos) {
        if (c == 'C' || c == 'G') return 'C';
    }
    return c;
}

std::string RaycasterWorld::getColorMinimapEntity(char c, const std::string& titleMap) {
    const auto& flags = getFlagsMap(titleMap);

    if (flags.upperTitle.find("VILA") != std::string::npos && c == 'F') return "\033[1;38;2;255;200;50m";
    if ((flags.upperTitle.find("VILA") != std::string::npos || flags.upperTitle.find("CAVERNA") != std::string::npos) && c == 'B') return "\033[1;38;2;100;200;255m";
    if (flags.upperTitle.find("SALA DO CHEFE") != std::string::npos) return "\033[1;38;2;255;255;255m";
    if ((flags.upperTitle.find("CABANA") != std::string::npos || flags.upperTitle.find("FLORESTA") != std::string::npos) && c == 'M') return "\033[1;38;2;200;100;255m";
    if (flags.upperTitle.find("LABIRINTO") != std::string::npos && c == 'B') return "\033[1;38;2;200;150;50m";
    if (c == 'P') return "\033[1;38;2;139;69;19m";
    if (c == '*') return "\033[38;2;34;139;34m";
    
    bool isKingdom = flags.isKingdom;
    if (isKingdom && c == 'C') return "\033[1;38;2;200;200;220m";
    if (isKingdom && c == 'G') return "\033[1;38;2;255;200;50m";
    return "\033[1;38;2;255;50;50m";
}
