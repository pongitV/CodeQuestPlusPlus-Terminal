#pragma once
#include <cstdint>

struct ColorRGB {
    uint8_t r, g, b;
};

enum class TexID {
    None = 0,
    // Paredes
    LabyrinthWood,
    LabyrinthArchPillar,
    LabyrinthBowPillar = LabyrinthArchPillar,
    LabyrinthArchBackground,
    LabyrinthBowBackground = LabyrinthArchBackground,
    MorganaWood,
    ChurchStainedglass,
    BridgeWood,
    Alchemist,
    EntryChurch,
    MannequinAnok,
    Francesco,
    Bjorn,
    Kiss = Bjorn,
    Knight,
    KingdomWood,
    ChurchAltar,
    ChurchWall,
    ChurchWallAltar,
    ChurchCeiling,
    PatioWall,
    ForestStructure,
    PatternStructure,
    TreeHeart,
    TreeForest,
    StoneVillage,
    StoneSpawn,
    RoomBossWall,
    CaveHeartWall,
    DarkBricks,
    KingdomStone,
    BridgeStone,
    InvalidWall,
    WallInvalidates = InvalidWall,
    
    // Chaos e Tetos
    FloorLabyrinthEdge,
    FloorLabyrinth,
    FloorRoomBossOutside,
    FloorRoomBossOut = FloorRoomBossOutside,
    FloorRoomBossInside,
    FloorHeartMoss,
    FloorHeartEarth,
    FloorHeartDark,
    FloorGrassForest,
    FloorGrassVillage,
    FloorEarth,
    FloorPattern,
    CeilingIndoorsHeartMoss,
    CeilingIndoorsHeartWood,
    CeilingIndoorsHeartDark,
    CeilingIndoorsPattern
};

class TextureManager {
public:
    static void initialize();
    static inline void boot() { initialize(); }
    static ColorRGB getColor(TexID id, int tx, int ty);
    
    // Lookup tables para otimizacao de funcoes trigonometricas (fast sine / cosine)
    static float fastSin(float angle);
    static float fastCos(float angle);
    static inline float fastYes(float angle) { return fastSin(angle); }

private:
    static bool initialized;
    static ColorRGB cache[256][16384];
    static float tableSin[4096];

    static void generate(TexID id);
};

// Apelido para compatibilidade retroativa
using ManagerTextures = TextureManager;

inline ColorRGB TextureManager::getColor(TexID id, int tx, int ty) {
    if (!initialized) initialize();
    int res = 128;
    if (tx < 0) tx = 0; 
    if (tx >= res) tx = res - 1;
    if (ty < 0) ty = 0; 
    if (ty >= res) ty = res - 1;
    return cache[static_cast<int>(id)][ty * res + tx];
}
