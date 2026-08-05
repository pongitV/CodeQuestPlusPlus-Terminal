#pragma once

#include <vector>
#include <string>
#include <utility>

#include "Domain/Characters/Character.h"

class ScreenVictory 
{
public:
    static void display(Character* currentPlayer, int quantityDeGoldObtained, int quantityDeXpObtained,
        int totalDeDamageCaused, int totalDeDamageReceived, int cureTotalReceived, int shiftsCombat,
        const std::vector<std::string>& obtainedItems, const std::vector<std::string>& enemiesDefeated,
        int parriesPerfect, int biggerDamage, int parriesTempted, int parriesEffective, int itemsConsumed, const std::vector<std::string>& newDiscoveries);
};
