#pragma once
#include "Domain/Characters/Character.h"

class IDefeatUI {
public:
    virtual ~IDefeatUI() = default;
    virtual void display(Character* currentPlayer, int quantityDeGoldObtained, int quantityDeXpObtained, int totalDeDamageCaused, int totalDeDamageReceived, int cureTotalReceived, int shiftsCombat) = 0;
};
