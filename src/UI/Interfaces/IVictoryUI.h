#pragma once
#include <string>
#include <vector>
#include <utility>
#include "Domain/Characters/Character.h"

class IVictoryUI {
public:
    virtual ~IVictoryUI() = default;
    virtual void display(Character* currentPlayer, int quantityDeGoldObtained, int quantityDeXpObtained, int totalDeDamageCaused, int totalDeDamageReceived, int cureTotalReceived, int shiftsCombat, const std::vector<std::string>& enemiesDefeated, int parriesPerfect, int biggerDamage, int parriesTempted, int parriesEffective, int itemsConsumed, const std::vector<std::pair<std::string, int>>& dropsUnique, bool canRiseLevel, const std::vector<std::string>& newDiscoveries, const std::string& titleMap) = 0;
};
