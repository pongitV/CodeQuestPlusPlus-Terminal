#pragma once
#include <string>

struct ResultDamage {
    int damageEnd = 0;
    int damageBlocked = 0;
    bool shieldBroke = false;
    std::string nameShieldBroken = "";
};
