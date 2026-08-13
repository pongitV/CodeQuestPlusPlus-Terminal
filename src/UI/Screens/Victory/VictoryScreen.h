#pragma once

#include <vector>
#include <string>
#include <utility>

#include "Domain/Characters/Character.h"

class VictoryScreen 
{
public:
    static void display(Character* currentPlayer, int obtainedGoldQuantity, int obtainedXpQuantity,
        int totalDamageCaused, int totalDamageReceived, int totalHealingReceived, int combatTurns,
        const std::vector<std::string>& obtainedItems, const std::vector<std::string>& enemiesDefeated,
        int parriesPerfect, int biggerDamage, int parriesTempted, int parriesEffective, int itemsConsumed, const std::vector<std::string>& newDiscoveries);
};
