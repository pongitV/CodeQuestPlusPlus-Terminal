#pragma once
#include <string>

struct DamageResult {
    int finalDamage = 0;
    int damageBlocked = 0;
    bool shieldBroken = false;
    std::string brokenShieldName = "";
};
