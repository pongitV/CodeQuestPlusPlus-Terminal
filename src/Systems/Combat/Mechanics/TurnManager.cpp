#include "Systems/Combat/Mechanics/TurnManager.h"
#include "Domain/Characters/Character.h"
#include <algorithm>
#include <ranges>

int ShiftManager::calculateMaxDexterityEnemies(const std::vector<std::unique_ptr<Character>>& enemies) {
    auto validEnemies = enemies | std::views::filter([](const auto& e) { return e != nullptr; });
    auto it = std::ranges::max_element(validEnemies, {}, [](const auto& e) { return e->getDexterity(); });
    return (it != validEnemies.end()) ? (*it)->getDexterity() : 0;
}

bool ShiftManager::enemiesActFirst(Character* player, int maxEnemyDexterity) {
    if (!player) return false;
    return maxEnemyDexterity > player->getDexterity();
}

bool ShiftManager::enemiesHaveDoubleAgility(Character* player, int maxEnemyDexterity) {
    if (!player) return false;
    return maxEnemyDexterity > (player->getDexterity() * 2);
}

bool ShiftManager::playerHasExtraTurnAtStart(Character* player, int maxEnemyDexterity) {
    if (!player) return false;
    return player->getDexterity() > (maxEnemyDexterity * 2);
}
