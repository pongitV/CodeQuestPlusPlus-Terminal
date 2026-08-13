#pragma once

#include <string>

#include "Domain/Characters/Character.h"

class DefeatScreen 
{
public:
    static void display(Character* currentPlayer, int obtainedGoldQuantity, int obtainedXpQuantity, int totalDamageCaused, int totalDamageReceived, int totalHealingReceived, int combatTurns);
};
