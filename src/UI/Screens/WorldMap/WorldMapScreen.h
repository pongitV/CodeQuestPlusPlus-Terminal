#pragma once
#include "World/InteractionMap.h"

class Character;
enum class MapLocation {
    InitialVillage,
    Forest,
    KingdomBridge,
    Kingdom
};

class WorldMapScreen {
public:
    static NextMapTransition display(Character* currentPlayer, MapLocation locationCurrent, int villageProgress, int forestProgress, int kingdomBridgeProgress, int kingdomProgress);
};
