#pragma once

#include <vector>
#include <string>

struct StatisticsCombat {
    int goldObtained = 0;
    int xpObtained = 0;
    int totalDamageCaused = 0;
    int totalDamageReceived = 0;
    int totalCureReceived = 0;
    int combatTurns = 1;
    std::vector<std::string> obtainedItems;
    std::vector<std::string> enemiesDefeated;
    int parriesAttempted = 0;
    int effectiveParries = 0;
    int perfectParries = 0;
    int highestDamageCaused = 0;
    int itemsConsumed = 0;
    std::vector<std::string> newDiscoveries;
};
