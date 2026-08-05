#pragma once

#include <string>

#include "Domain/Characters/Character.h"

class ScreenDefeat 
{
public:
    static void display(Character* currentPlayer, int quantityDeGoldObtained, int quantityDeXpObtained, int totalDeDamageCaused, int totalDeDamageReceived, int cureTotalReceived, int shiftsCombat);
};
