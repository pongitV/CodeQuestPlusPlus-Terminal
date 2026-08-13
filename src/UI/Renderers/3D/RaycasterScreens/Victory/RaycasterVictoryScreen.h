#pragma once

#include <string>
#include <vector>
#include <utility>

class Character;

class RaycasterVictoryScreen {
public:
    static void display(Character* currentPlayer, int obtainedGoldQuantity, int obtainedXpQuantity,
        int totalDamageCaused, int totalDamageReceived, int totalHealingReceived, int combatTurns,
        const std::vector<std::string>& enemiesDefeated, int parriesPerfect, int biggerDamage,
        int parriesTempted, int parriesEffective, int itemsConsumed, const std::vector<std::pair<std::string, int>>& dropsUnique,
        bool canRiseLevel, const std::vector<std::string>& newDiscoveries,
        const std::string& titleMap);
};
