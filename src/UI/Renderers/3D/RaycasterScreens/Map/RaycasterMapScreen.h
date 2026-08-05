#pragma once

#include <string>

enum class MapLocation;

class ScreenMapRaycaster {
public:
    static int display(MapLocation locationCurrent, int villageProgress, int forestProgress, int kingdomBridgeProgress, int kingdomProgress);
};
