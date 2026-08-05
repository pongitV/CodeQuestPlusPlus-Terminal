#pragma once

#include <vector>
#include <string>

struct StatisticsCombat {
    int quantityDeGoldObtained = 0;
    int quantityDeXpObtained = 0;
    int totalDeDamageCaused = 0;
    int totalDeDamageReceived = 0;
    int cureTotalReceived = 0;
    int shiftsCombat = 1;
    std::vector<std::string> obtainedItems;
    std::vector<std::string> enemiesDefeated;
    int parriesTempted = 0;
    int parriesEffective = 0;
    int parriesPerfect = 0;
    int biggerDamageCaused = 0;
    int itemsConsumed = 0;
    std::vector<std::string> newDiscoveries;
};
