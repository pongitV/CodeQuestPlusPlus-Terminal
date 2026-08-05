#include "Systems/Combat/Mechanics/EnemyMechanics.h"
#include "Domain/Characters/Character.h"
#include "Core/Utils/RandomGenerator.h"

Character* MechanicsEnemy::chooseTarget(const std::vector<Character*>& alliesAlive, Character* currentPlayer) {
    std::vector<Character*> targetsPossible;
    std::vector<Character*> minionsAlive;
    std::vector<Character*> alliesNormalAlive;

    for (auto* ally : alliesAlive) {
        if (ally->isMinion()) {
            minionsAlive.push_back(ally);
        } else {
            alliesNormalAlive.push_back(ally);
        }
    }

    if (!minionsAlive.empty()) {
        targetsPossible = minionsAlive;
    } else if (!alliesNormalAlive.empty()) {
        targetsPossible = alliesNormalAlive;
    } else {
        targetsPossible.push_back(currentPlayer);
    }

    return targetsPossible[RandomGenerator::getInteger(0, static_cast<int>(targetsPossible.size()) - 1)];
}
