#pragma once

#include "Domain/Characters/Character.h"

struct PowerCombat {
    int damageFisIs;
    int damageMagIs;
    int defFixed;
    double mitigation;
};

struct DebuffInfo {
    int lostStrength;
    int dexterityLost;
    int resLost;
    int constLost;
    bool hasBuff;
};

class AttributesScreen 
{
public:
    static void display(Character* currentPlayer);
    static void managePlayerCharacterSheet(Character* currentPlayer);

    static PowerCombat calculatePowerCombat(Character* currentPlayer, double multiplier);
    static DebuffInfo calculateDebuff(Character* currentPlayer);
};
