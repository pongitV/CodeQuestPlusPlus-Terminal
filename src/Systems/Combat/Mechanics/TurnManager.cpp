#include "Systems/Combat/Mechanics/TurnManager.h"
#include "Domain/Characters/Character.h"
#include <algorithm>

int ManagerShifts::calculateMaxDexterityEnemies(const std::vector<std::unique_ptr<Character>>& enemies) {
    int maxDexterity = 0;
    for (const auto& enemyPtr : enemies) {
        if (enemyPtr->getDexterity() > maxDexterity) {
            maxDexterity = enemyPtr->getDexterity();
        }
    }
    return maxDexterity;
}

bool ManagerShifts::enemiesAreMoreAct(Character* player, int maxDexterityEnemies) {
    return maxDexterityEnemies > player->getDexterity();
}

bool ManagerShifts::enemiesHasDoubleDeAgility(Character* player, int maxDexterityEnemies) {
    return maxDexterityEnemies > (player->getDexterity() * 2);
}

bool ManagerShifts::playerHasShiftExtraNoHome(Character* player, int maxDexterityEnemies) {
    return player->getDexterity() > (maxDexterityEnemies * 2);
}
