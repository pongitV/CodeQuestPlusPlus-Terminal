#include "UI/Screens/Victory/VictoryScreen.h"
#include <map>
#include "UI/PerspectiveManager.h"
#include "Systems/Combat/Combat.h"
#include "Domain/Characters/Character.h"
#include "UI/Screens/Combat/CombatScreen.h"

void VictoryScreen::display(Character* currentPlayer, int obtainedGoldQuantity, int obtainedXpQuantity,
    int totalDamageCaused, int totalDamageReceived, int totalHealingReceived, int combatTurns,
    const std::vector<std::string>& obtainedItems, const std::vector<std::string>& enemiesDefeated,
    int parriesPerfect, int biggerDamage, int parriesTempted, int parriesEffective, int itemsConsumed, const std::vector<std::string>& newDiscoveries)
{
    std::map<std::string, int> count;
    for (const std::string& item : obtainedItems) count[item]++;
    std::vector<std::pair<std::string, int>> dropsUnique;
    for (auto const& [name, qty] : count) dropsUnique.push_back({name, qty});

    bool canRiseLevel = currentPlayer->canLevelUp();

    const std::string& titleMap = CombatScreen::context.titleMapCurrent;

    PerspectiveManager::getVictoryUI().display(currentPlayer, obtainedGoldQuantity, obtainedXpQuantity,
        totalDamageCaused, totalDamageReceived, totalHealingReceived, combatTurns,
        enemiesDefeated, parriesPerfect, biggerDamage, parriesTempted, parriesEffective, itemsConsumed, dropsUnique, canRiseLevel, newDiscoveries, titleMap);
}
