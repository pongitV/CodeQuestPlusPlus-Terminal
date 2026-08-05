#pragma once
#include <cstdint>

struct ColorRGB {
    uint8_t r, g, b;
};

enum class TexID {
    None = 0,
    // Paredes
    LabyrinthWood,
    LabyrinthBowPillar,
    LabyrinthBowBackground,
    MorganaWood,
    ChurchStainedglass,
    BridgeWood,
    Alchemist,
    EntryChurch,
    MannequinAnok,
    Francesco,
    Kiss,
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
    WallInvalidates,
    
    // Chaos e Tetos
    ChaoLabyrinthEdge,
    ChaoLabyrinth,
    ChaoRoomBossOut,
    ChaoRoomBossInside,
    ChaoHeartMoss,
    ChaoHeartEarth,
    ChaoHeartDark,
    ChaoGrassForest,
    ChaoGrassVillage,
    ChaoEarth,
    ChaoPattern,
    CeilingIndoorsHeartMoss,
    CeilingIndoorsHeartWood,
    CeilingIndoorsHeartDark,
    CeilingIndoorsPattern
};

class ManagerTextures {
public:
    static void boot();
    static ColorRGB getColor(TexID id, int tx, int ty);
    
    // Lookup tables para otimização de funções trigonométricas
    static float fastYes(float angle);
    static float fastCos(float angle);

private:
    static bool initialized;
    static ColorRGB cache[256][16384];
    static float tableYes[4096];

    static void generate(TexID id);
};

inline ColorRGB ManagerTextures::getColor(TexID id, int tx, int ty) {
    if (!initialized) boot();
    int res = 128;
    if (tx < 0) tx = 0; 
    if (tx >= res) tx = res - 1;
    if (ty < 0) ty = 0; 
    if (ty >= res) ty = res - 1;
    return cache[static_cast<int>(id)][ty * res + tx];
}
