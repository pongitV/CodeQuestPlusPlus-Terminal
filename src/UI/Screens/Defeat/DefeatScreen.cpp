#include "UI/Screens/Defeat/DefeatScreen.h"
#include "UI/PerspectiveManager.h"

void DefeatScreen::display(Character* currentPlayer, int obtainedGoldQuantity, int obtainedXpQuantity, int totalDamageCaused, int totalDamageReceived, int totalHealingReceived, int combatTurns)
{
    PerspectiveManager::getDefeatUI().display(currentPlayer, obtainedGoldQuantity, obtainedXpQuantity, totalDamageCaused, totalDamageReceived, totalHealingReceived, combatTurns);
}
