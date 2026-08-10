#include "Systems/Combat/Mechanics/TurnManager.h"
#include "Domain/Characters/Character.h"
#include <algorithm>

int ShiftManager::calculateMaxDexterityEnemies(const std::vector<std::unique_ptr<Character>>& enemies) {
    int maxDexterity = 0;
    for (const auto& enemyPtr : enemies) {
        if (enemyPtr->getDexterity() > maxDexterity) {
            maxDexterity = enemyPtr->getDexterity();
        }
    }
    return maxDexterity;
}

bool ShiftManager::enemiesActFirst(Character* player, int maxEnemyDexterity) {
    return maxEnemyDexterity > player->getDexterity();
}

bool ShiftManager::enemiesHaveDoubleAgility(Character* player, int maxEnemyDexterity) {
    return maxEnemyDexterity > (player->getDexterity() * 2);
}

bool ShiftManager::playerHasExtraTurnAtStart(Character* player, int maxEnemyDexterity) {
    return player->getDexterity() > (maxEnemyDexterity * 2);
}
