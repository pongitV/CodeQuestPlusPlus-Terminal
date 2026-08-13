#pragma once
#include "Domain/Characters/Character.h"

class IDefeatUI {
public:
    virtual ~IDefeatUI() = default;
    virtual void display(Character* currentPlayer, int obtainedGoldQuantity, int obtainedXpQuantity, int totalDamageCaused, int totalDamageReceived, int totalHealingReceived, int combatTurns) = 0;
};
