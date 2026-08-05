#include "UI/Screens/Victory/VictoryScreen.h"
#include <map>
#include "UI/PerspectiveManager.h"
#include "Systems/Combat/Combat.h"
#include "Domain/Characters/Character.h"
#include "UI/Screens/Combat/CombatScreen.h"

void ScreenVictory::display(Character* currentPlayer, int quantityDeGoldObtained, int quantityDeXpObtained,
    int totalDeDamageCaused, int totalDeDamageReceived, int cureTotalReceived, int shiftsCombat,
    const std::vector<std::string>& obtainedItems, const std::vector<std::string>& enemiesDefeated,
    int parriesPerfect, int biggerDamage, int parriesTempted, int parriesEffective, int itemsConsumed, const std::vector<std::string>& newDiscoveries)
{
    std::map<std::string, int> count;
    for (const std::string& item : obtainedItems) count[item]++;
    std::vector<std::pair<std::string, int>> dropsUnique;
    for (auto const& [name, qty] : count) dropsUnique.push_back({name, qty});

    bool canRiseLevel = currentPlayer->canRiseDeLevel();

    const std::string& titleMap = CombatScreen::context.titleMapCurrent;

    PerspectiveManager::getVictoryUI().display(currentPlayer, quantityDeGoldObtained, quantityDeXpObtained,
        totalDeDamageCaused, totalDeDamageReceived, cureTotalReceived, shiftsCombat,
        enemiesDefeated, parriesPerfect, biggerDamage, parriesTempted, parriesEffective, itemsConsumed, dropsUnique, canRiseLevel, newDiscoveries, titleMap);
}
