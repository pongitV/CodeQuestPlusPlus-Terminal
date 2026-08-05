#pragma once

#include <string>
#include <vector>
#include <utility>

class Character;

class ScreenVictoryRaycaster {
public:
    static void display(Character* currentPlayer, int quantityDeGoldObtained, int quantityDeXpObtained,
        int totalDeDamageCaused, int totalDeDamageReceived, int cureTotalReceived, int shiftsCombat,
        const std::vector<std::string>& enemiesDefeated, int parriesPerfect, int biggerDamage,
        int parriesTempted, int parriesEffective, int itemsConsumed, const std::vector<std::pair<std::string, int>>& dropsUnique,
        bool canRiseLevel, const std::vector<std::string>& newDiscoveries,
        const std::string& titleMap);
};
